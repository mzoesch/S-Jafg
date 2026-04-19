// Copyright mzoesch. All rights reserved.

#include "Async/TaskUtility.h"
#include "Async/Runnable.h"
#include "Containers/ComplexQueue.h"
#include "Core/Application.h"
#include "Stats/Stats.h"
#if JAFG_WITH_GCC || PLATFORM_LINUX
    #include <thread>
#endif /* WITH_GCC */
#if PLATFORM_WASM
    #include <emscripten/threading.h>
    // #include <c++/v1/__threading_support>
#endif /* PLATFORM_WASM */

namespace
{

#if JAFG_WITH_MSVC
    static_assert(std::is_same_v<::Jafg::LThreadId, _Thrd_id_t>, "Compiler specific thread id is not the same.");
    #if 0 // !_HAS_CXX23 Even better just use the xthreads.h implementation.
        /* Just use msvc std implementation. */
        #define PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID() \
            std::this_thread::get_id()._Get_underlying_id()
    #else /* !_HAS_CXX23 */
        static_assert(
            sizeof(::Jafg::LThreadId) == sizeof(std::thread::id),
            "The size of std::thread::id and u32 must be the same."
        );
        #define PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID() _Thrd_id()
    #endif /* _HAS_CXX23 */
#elif JAFG_WITH_GCC || JAFG_WITH_CLANG
    #if PLATFORM_WASM
        static_assert(::std::is_same_v<::Jafg::LThreadId, ::std::__libcpp_thread_id>, "Compiler specific thread id is not the same.");
        #define PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID() pthread_self()
    #else /* PLATFORM_WASM */
        static_assert(std::is_same_v<::Jafg::LThreadId, pthread_t>, "Compiler specific thread id is not the same.");
        #define PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID() __gthread_self()
    #endif /* !PLATFORM_WASM */
#endif /* WITH_GCC */

static_assert(
    sizeof(::Jafg::LThreadId) == sizeof(std::thread::id),
    "The size of std::thread::id and u32 must be the same."
);

struct LEngineThread;
struct LTask;

//#
//# If this is true. No new threads can be registered anymore or cleaned up should be done.
//# Just exit as fast as possible without destroying the program state, e.g., forget to close file handles to databases
//# which may lead to data corruption, forget to close network connections which may lead to data loss and/or user
//# time out which is bad for the user experience, etc.
//#
bool bTearingDown = false;

//# Global queue item for tasks that are being executed on the specified thread in the future.
struct LTask final
{
    FORCEINLINE LTask() : Time(Jafg::ETaskTime::Fail) { }
    FORCEINLINE explicit LTask(const Jafg::ETaskTime::Type InTime, Jafg::LTaskDelegate&& InDelegate)
        : Time(InTime), Delegate(std::move(InDelegate))
    {
        check(!!this->Delegate)
    }

    Jafg::ETaskTime::Type Time;
    Jafg::LTaskDelegate   Delegate;
};
struct LEngineThread final
{
    FORCEINLINE LEngineThread(const Jafg::LThreadId InId, const Jafg::ENamedThreads::Type InThreadName)
        : Id(InId), ThreadName(InThreadName)
    {
    }

    FORCEINLINE explicit LEngineThread(
        const Jafg::ENamedThreads::Type InThreadName,
        Jafg::LRunnable*                InRunnable,
        const bool                      bInKillRunnableWhenFinished
    )
        // The #Id is deferred to when the thread is running autarcik. This ctor is called on the aggregating thread,
        // so we have to wait for the platform to assign an id to the thread.
        : Id(0), ThreadName(InThreadName),
          Runnable(InRunnable), bKillRunnableWhenFinished(bInKillRunnableWhenFinished)
    {
    }

    PROHIBIT_COPY(LEngineThread)
    DEFAULT_MOVE(LEngineThread)

    FORCEINLINE ~LEngineThread()
    {
        if (this->bKillRunnableWhenFinished == false)
        {
            this->Runnable.release();
        }

        return;
    }

    FORCEINLINE LString GetDisplayName() const;

    Jafg::LThreadId           Id;
    Jafg::ENamedThreads::Type ThreadName;
    LString                   HumanReadableName;
    Jafg::TMpmcQueue<LTask>   TaskQueue;
    TOptional<std::thread>    Thread;
    TUnique<Jafg::LRunnable>  Runnable;
    bool                      bKillRunnableWhenFinished{ false };
};

//# Mutex for when adding or removing threads from the engine known threads list.
//# Not when adding or running (removing) tasks from the linked list on an already existing thread.
std::shared_mutex EngineThreadsMutex;
//# Map for all queses for all tasks.
TArray<LEngineThread> EngineThreads;

FORCEINLINE LString LEngineThread::GetDisplayName() const
{
    if (this->Runnable && this->Runnable->GetHumanReadableName().empty() == false)
    {
        return this->Runnable->GetHumanReadableName();
    }

    if (this->HumanReadableName.empty() == false)
    {
        return this->HumanReadableName;
    }

    return Jafg::LexToString(this->ThreadName);
}

FORCEINLINE void RenameMe(const LString& InDisplayName)
{
    jassert( InDisplayName.size() * sizeof(LString::value_type) < 16 && "Thread name may not exceed 16 bytes." )

#if PLATFORM_WINDOWS
    LWString WideDisplayName = algo::utf8_to_utf16(InDisplayName);
    ::SetThreadDescription(::GetCurrentThread(), WideDisplayName.c_str());
    LOG_VERBOSE(LogTaskUtility, "Renamed thread to [{}].", InDisplayName)
#elif PLATFORM_LINUX
    pthread_setname_np(pthread_self(), InDisplayName.c_str());
    LOG_VERBOSE(LogTaskUtility, "Renamed thread to [{}].", InDisplayName)
#else /* PLATFORM_LINUX */
    LOG_WARNING(LogTaskUtility, "Failed to rename thread to [{}].", InDisplayName)
#endif /* !PLATFORM_LINUX */

    return;
}

} /* ~Namespace <Anonymous> */

//
// Moved out of a header file into this translation file, so that this translation unit can be the unique
// owner of the v-table pointer from the #LRunnable class.
//
Jafg::LRunnable::LRunnable()
{
    return;
}

Jafg::LRunnable::LRunnable(const LString& InHumanReadableName)
    : HumanReadableName(InHumanReadableName)
{
    return;
}

void Jafg::LRunnable::Join()
{
    /*
     * So this is cheeky. But the this-pointer could be invalid if the thread was removed before the lock was acquired.
     * Therefore, we just search for the this-pointer in our global list of threads without ever accessing the object
     * directly.
     */
    std::shared_lock Lock(::EngineThreadsMutex);

    auto EngineThread { algo::find(::EngineThreads, this, [](auto const& E){ return E.Runnable.get(); }) };
    if (EngineThread == ::EngineThreads.end())
    {
        LOG_WARNING(LogRunnable, "No such runnable.")
        return;
    }

    const ENamedThreads::Type Name{ EngineThread->ThreadName };
    Lock.unlock();

    Tasks::JoinThread(Name);

    return;
}

namespace Jafg::Tasks::Private
{

ENGINE_API i32 CustomThreadCounter = ENamedThreads::Custom + 1;

} /* ~Namespace Jafg::Tasks::Private */

LString Jafg::LexToString(const ENamedThreads::Type Thread)
{
    switch (Thread)
    {
    case ENamedThreads::Master:  { return "Master";  }
    case ENamedThreads::Failure: { return "Failure"; }
    default: { return Jafg::SprintF("Thread_{}", static_cast<LThreadId>(Thread)); }
    }
}

LString Jafg::LexToString(const ETaskTime::Type Time)
{
    if (Time & ETaskTime::Fail)                          { return "Fail"; }
    if (Time & ETaskTime::NoTickDangerous)               { return "NoTick"; }
    if (Time & ETaskTime::AfterCorePackageLoadDangerous) { return "AfterCorePackageLoad"; }
    if (Time & ETaskTime::Early)                         { return "Early"; }
    if (Time & ETaskTime::Late)                          { return "Late"; }

    panic( "Unknown task time." )
    return "Unknown";
}

void Jafg::Tasks::RegisterThread(ENamedThreads::Type InThreadName)
{
    std::unique_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskSystem, "Thread {} cannot be registered anymore.", LexToString(InThreadName))
        return;
    }

    LThreadId Me = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();
    if (algo::contains(::EngineThreads, Me, &LEngineThread::Id))
    {
        LOG_ERROR(LogTaskSystem, "Thread {} already registered under a different name.", LexToString(InThreadName))
        return;
    }

    if (algo::contains(::EngineThreads, InThreadName, &LEngineThread::ThreadName))
    {
        LOG_ERROR(LogTaskSystem, "Thread {} already registered.", LexToString(InThreadName))
        return;
    }

    ::EngineThreads.emplace_back(Me, InThreadName);

#if WITH_STATS
    if (Stats::Private::GTracer)
    {
        Stats::Private::GTracer->AddNamedThread({
            LexToString(InThreadName),
            Me
        });
    }
#endif /* WITH_STATS */

    return;
}

LString Jafg::Tasks::GetCurrentThreadDisplayName()
{
    const LThreadId Me = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();

    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to get current thread display name.")
        return "NotRegistered";
    }

    if (LEngineThread const* Thread { algo::find_pointer(::EngineThreads, Me, &LEngineThread::Id) }; Thread)
    {
        return Thread->GetDisplayName();
    }

    return "NotRegistered";
}

LString Jafg::Tasks::GetCurrentThreadDisplayNameChecked()
{
    const LThreadId Me = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();

    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to get current thread display name.")
        checkNoEntry()
        return "NotRegistered";
    }

    if (const LEngineThread* Thread = algo::find_pointer(::EngineThreads, Me, &LEngineThread::Id); Thread)
    {
        return Thread->GetDisplayName();
    }

    checkNoEntry()
    return "NotRegistered";
}

LString Jafg::Tasks::GetCurrentThreadDisplayNameAsserted()
{
    const LThreadId Me = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();

    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to get current thread display name.")
        jassertNoEntry()
        return "NotRegistered";
    }

    if (const LEngineThread* Thread = algo::find_pointer(::EngineThreads, Me, &LEngineThread::Id); Thread)
    {
        return Thread->GetDisplayName();
    }

    jassertNoEntry()
    return "NotRegistered";
}

Jafg::ENamedThreads::Type Jafg::Tasks::GetCurrentThreadName()
{
    const LThreadId Me = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();

    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to get current thread name.")
        return ENamedThreads::Failure;
    }

    if (const LEngineThread* Thread = algo::find_pointer(::EngineThreads, Me, &LEngineThread::Id); Thread)
    {
        return Thread->ThreadName;
    }

    return ENamedThreads::Failure;
}

Jafg::LThreadId Jafg::Tasks::GetCurrentThreadId()
{
    return PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();
}

bool Jafg::Tasks::HasThread(const ENamedThreads::Type InThreadName)
{
    std::shared_lock Lock(::EngineThreadsMutex);

    if (::bTearingDown)
    {
        return false;
    }

    return algo::contains(::EngineThreads, InThreadName, &LEngineThread::ThreadName);
}

bool Jafg::Tasks::IsOnThread(const ENamedThreads::Type InThreadName)
{
    std::shared_lock Lock(::EngineThreadsMutex);

    if (::bTearingDown)
    {
        /*
         * This is not a 100% safe. But theoretically, only the master thread should be running when
         * ::bClosedJoinableThreadsVector is true. If this is not the case, this program is probably already burning and
         * in an undefined state. And when this would be the case, we are already pretty much fucked; this function
         * will not help either with the shitty state but also will not make it dramatically worse.
         */
        return ::EngineThreads.empty() && InThreadName == ENamedThreads::Master;
    }

    if (const LEngineThread* Thread = algo::find_pointer(::EngineThreads, InThreadName, &LEngineThread::ThreadName))
    {
        return Thread->Id == PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();
    }

    if (::EngineThreads.empty() && InThreadName == ENamedThreads::Master)
    {
        /* If no threads are registered, we are most likely on the master thread during ssi. */
        return true;
    }

    LOG_ERROR(LogTaskUtility, "Thread [{}] not found.", LexToString(InThreadName))
    return false;
}

void Jafg::Tasks::Make(ENamedThreads::Type InThreadName, ETaskTime::Type InPreferredTime, LTaskDelegate InDelegate)
{
    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskSystem, "Failed to make new task.")
        return;
    }

    if (LEngineThread* Thread = algo::find_pointer(::EngineThreads, InThreadName, &LEngineThread::ThreadName); Thread)
    {
        Thread->TaskQueue.EnqueueByEmplace(InPreferredTime, std::move(InDelegate));
    }
    else
    {
        LOG_ERROR(LogTaskSystem, "Thread {} not found.", LexToString(InThreadName))
        checkNoEntry()
    }

    return;
}

bool Jafg::Tasks::IsThreadRunning(const ENamedThreads::Type InThreadName)
{
    std::shared_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to check for thread status.")
        return false;
    }

    return algo::contains(::EngineThreads, InThreadName, &LEngineThread::ThreadName);
}

i32 Jafg::Tasks::TryRunTasks(const ENamedThreads::Type Which, const ETaskTime::Type Time, const i32 MaxTasks)
{
    STAT_CYCLE_FUNCTION_START(Trt)

    i32 RunTasks = 0;
    while (true)
    {
        if (RunTasks >= MaxTasks && MaxTasks != RunAllTasks)
        {
            break;
        }

        std::shared_lock Lock(::EngineThreadsMutex); // TODO Can we make this faster?
        LEngineThread* Thread { algo::find_pointer(::EngineThreads, Which, &LEngineThread::ThreadName) };
        ensure(Thread != nullptr);
        if (JAFG_UNLIKELY(Thread == nullptr))
        {
            LOG_ERROR(LogTaskSystem, "Thread {} not found.", LexToString(Which))
            break;
        }

        TMpmcQueue<LTask>& Queue = Thread->TaskQueue;
        LTask Task;
        if (Queue.DequeueByMoveWithPredicate(&Task, [Time](const LTask& InTask){ return InTask.Time & Time; }) == false)
        {
            break;
        }
        Lock.unlock();

        Task.Delegate();
        ++RunTasks;

        if (Queue.IsEmpty())
        {
            break;
        }

        continue;
    }

    if constexpr (IS_COMPILED_LOG(LogTasks, Trace))
    {
        if (RunTasks > 0)
        {
            LOG_TRACE(LogTasks, "Run {} tasks on thread [{}] during [{}].", RunTasks, LexToString(Which), LexToString(Time))
        }
    }

#if WITH_STATS
    if (RunTasks < 1)
    {
        STAT_DISCARD(Trt)
    }
#endif /* WITH_STATS */

    return RunTasks;
}

void Jafg::Tasks::StopThread(const ENamedThreads::Type ThreadName)
{
    STAT_CYCLE_FUNCTION()

    std::shared_lock Lock(::EngineThreadsMutex);
    LEngineThread* Thread = algo::find_pointer(::EngineThreads, ThreadName, &LEngineThread::ThreadName);
    if (Thread)
    {
        if (Thread->Runnable)
        {
            Thread->Runnable->Stop(ERunnableStopReason::RequestedStop);
        }
        else
        {
            LOG_WARNING(LogTaskSystem, "Invalid runnable for thread [{}].", LexToString(ThreadName))
        }
    }
    else
    {
        LOG_WARNING(LogTaskSystem, "No such thread [{}].", LexToString(ThreadName))
    }

    return;
}

void Jafg::Tasks::JoinThread(const ENamedThreads::Type ThreadName)
{
    STAT_CYCLE_FUNCTION()

    std::shared_lock Lock(::EngineThreadsMutex);
    LEngineThread* Thread = algo::find_pointer(::EngineThreads, ThreadName, &LEngineThread::ThreadName);
    if (Thread && Thread->Thread.has_value())
    {
        if (Thread->Thread->joinable())
        {
            Lock.unlock();
            Thread->Thread->join();
        }
        else
        {
            LOG_WARNING(LogTaskSystem, "Thread [{}] is not joinable.", LexToString(ThreadName))
        }
    }
    else
    {
        LOG_WARNING(LogTaskSystem, "Thread [{}] is not joinable.", LexToString(ThreadName))
    }

    return;
}

Jafg::ETaskExit::Type Jafg::Tasks::Private::LaunchNamedThread(const ENamedThreads::Type ThreadName, LRunnable* Runnable, const bool bKillRunnableWhenFinished /* = true */)
{
    STAT_CYCLE_FUNCTION()

    checkSlow( Runnable )

    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskUtility, "Failed to launch thread {}.", Runnable->GetHumanReadableName())
        if (bKillRunnableWhenFinished)
        {
            delete Runnable;
        }
        return ETaskExit::Illformed;
    }

    if (IsThreadRunning(ThreadName))
    {
        LOG_ERROR(LogTaskSystem, "Thread {}[{}] already running.", Runnable->GetHumanReadableName(), static_cast<i32>(ThreadName))
        if (bKillRunnableWhenFinished)
        {
            delete Runnable;
        }
        return ETaskExit::Illformed;
    }

    const ETaskExit::Type ErrorLevel = Runnable->Initialize();
    if (ErrorLevel != ETaskExit::Success)
    {
        if (ErrorLevel >= ETaskExit::Failure)
        {
            LOG_FATAL(
                LogTaskSystem,
                "Thread {}[{}] with aggregator {}[{}] failed to initialize with a non transient failure code: {}[{}].",
                Runnable->GetHumanReadableName(), LexToString(ThreadName),
                GetCurrentThreadDisplayName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                )
        }

        if (ErrorLevel >= ETaskExit::SanitizedFailure)
        {
            LString ErrorLevelStr = Jafg::SprintF(
                "Thread {}[{}] with aggregator {}[{}] failed to initialize with a sanitized failure code: {}[{}].",
                Runnable->GetHumanReadableName(), LexToString(ThreadName),
                GetCurrentThreadDisplayName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
            );

            if (bKillRunnableWhenFinished)
            {
                delete Runnable;
            }

            LOG_ERROR(LogTaskSystem, "{}", ErrorLevelStr)
            Application::RequestEngineExit(EPlatformExit::Fatal, ErrorLevelStr);

            return ErrorLevel;
        }

        if (ErrorLevel >= ETaskExit::TransientFailure)
        {
            LOG_ERROR(
                LogTaskSystem,
                "Thread {}[{}] with aggregator {}[{}] failed to initialize with a transient failure code: {}[{}].",
                Runnable->GetHumanReadableName(), LexToString(ThreadName),
                GetCurrentThreadDisplayName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                )
            if (bKillRunnableWhenFinished)
            {
                delete Runnable;
            }
            return ErrorLevel;
        }

        LOG_ERROR(
            LogTaskSystem,
            "Thread {}[{}] with aggregator {}[{}] failed to initialize with an unknown code: {}[{}].",
            Runnable->GetHumanReadableName(), LexToString(ThreadName),
            GetCurrentThreadDisplayName(), GetCurrentThreadId(),
            LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
            )
        if (bKillRunnableWhenFinished)
        {
            delete Runnable;
        }
        return ErrorLevel;
    }

    std::unique_lock Lock(::EngineThreadsMutex);
    if (::bTearingDown)
    {
        LOG_ERROR(LogTaskSystem, "Failed to launch thread {}[{}].", Runnable->GetHumanReadableName(), LexToString(ThreadName))
        if (bKillRunnableWhenFinished)
        {
            delete Runnable;
        }
        return ETaskExit::Illformed;
    }

    ::EngineThreads.emplace_back(ThreadName, Runnable, bKillRunnableWhenFinished);

    std::thread ThreadObj = std::thread(
        [ThreadName] (void) -> void
        {
            {
                std::unique_lock RefLock(::EngineThreadsMutex);
                LEngineThread* Ref{ algo::find_pointer(::EngineThreads, ThreadName, &LEngineThread::ThreadName) };
                check( Ref )
                Ref->Id = PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID();

                const LString DisplayName = Ref->GetDisplayName();
                ::RenameMe(DisplayName);
#if WITH_STATS
                if (Stats::Private::GTracer)
                {
                    Stats::Private::GTracer->AddNamedThread({
                        DisplayName,
                        Ref->Id,
                    });
                }
#endif /* WITH_STATS */
            }

            STAT_QUICK_CYCLE_START("Jafg::Tasks::Private::LaunchNamedThread::std::thread")

            LRunnable* MyRunnable{ nullptr };
            {
                std::shared_lock RefLock(::EngineThreadsMutex);
                LEngineThread* Ref{ algo::find_pointer(::EngineThreads, ThreadName, &LEngineThread::ThreadName) };
                check( Ref )
                MyRunnable = Ref->Runnable.get();
                check( MyRunnable )
            }

            const ETaskExit::Type LambdaErrorLevel{ MyRunnable->Run() };

            if (LambdaErrorLevel != ETaskExit::Success)
            {
                check( MyRunnable )

                if (LambdaErrorLevel >= ETaskExit::Failure)
                {
                    LOG_FATAL(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a non transient failure code: {}[{}].",
                        MyRunnable->GetHumanReadableName(), LexToString(ThreadName),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                        )
                    return;
                }
                if (LambdaErrorLevel >= ETaskExit::SanitizedFailure)
                {
                    LString ErrorLevelStr = Jafg::SprintF(
                        "Thread {}[{}] failed to initialize with a sanitized failure code: {}[{}].",
                        MyRunnable->GetHumanReadableName(), LexToString(ThreadName),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                        );

                    LOG_ERROR(LogTaskSystem, "{}", ErrorLevelStr)
                    Application::RequestEngineExit(EPlatformExit::Fatal, ErrorLevelStr);
                }
                else if (LambdaErrorLevel >= ETaskExit::TransientFailure)
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a transient failure code: {}[{}].",
                        MyRunnable->GetHumanReadableName(), LexToString(ThreadName),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                        )
                }
                else
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with an unknown code: {}[{}].",
                        MyRunnable->GetHumanReadableName(), LexToString(ThreadName),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                        )
                }
            }

            STAT_CYCLE_START(Re, "Runnable::Exit")
            MyRunnable->Exit();
            STAT_CYCLE_END(Re)

            if (::bTearingDown)
            {
                return;
            }

            Tasks::Make(ENamedThreads::Master, ETaskTime::Whenever,
                [ThreadName](void) -> void
                {
                    STAT_CYCLE_FUNCTION()

                    std::unique_lock RemoveLock(::EngineThreadsMutex);
                    if (::bTearingDown)
                    {
                        return;
                    }

                    auto It{ algo::find(::EngineThreads, ThreadName, &LEngineThread::ThreadName) };
                    if (It == ::EngineThreads.end())
                    {
                        LOG_ERROR(LogTaskSystem, "Failed to find thread {} for removal.", LexToString(ThreadName))
                        return;
                    }

                    check( It->Thread.has_value() )
                    if (It->Thread->joinable())
                    {
                        It->Thread->join();
                    }

                    ::EngineThreads.erase(It);

                    LOG_VERBOSE(LogTaskSystem, "Removed thread {}[{}].", LexToString(ThreadName), static_cast<i32>(ThreadName))

                    return;
                }
            );

            return;
        }
    );

    check( ::EngineThreads.back().ThreadName == ThreadName )
    check( ::EngineThreads.back().Thread.has_value() == false )
    ::EngineThreads.back().Thread = std::move(ThreadObj);

    return ErrorLevel;
}

void Jafg::Tasks::Private::StopAndJoinRemainingThreads(const bool bJoinTasks /* = true */)
{
    STAT_CYCLE_FUNCTION()

    check( IsOnMasterThread() )

    ::EngineThreadsMutex.lock();
    check( ::bTearingDown == false )
    ::bTearingDown = true;
    const TArray<LEngineThread>::size_type EngineThreadsSize{ ::EngineThreads.size() };
    ::EngineThreadsMutex.unlock();

    if (bJoinTasks)
    {
        TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, RunAllTasks);
    }

    auto TimeBeforeJoinedAllThreads{algo::now()};
    for (LEngineThread& Thread : ::EngineThreads)
    {
        check(::EngineThreads.size() == EngineThreadsSize)

        if (Thread.Runnable)
        {
            check( Thread.Thread.has_value())
            Thread.Runnable->Stop(ERunnableStopReason::EngineTermination);
            auto TimeBeforeJoin{std::chrono::high_resolution_clock::now()};
            if (Thread.Thread->joinable())
            {
                Thread.Thread->join();
            }
            LOG_VERBOSE(LogTaskSystem, "Joined thread {}[{}] after {} seconds."
                , Thread.Runnable->GetHumanReadableName(), LexToString(Thread.ThreadName)
                , algo::time_diff(TimeBeforeJoin, algo::now())
                )
        }

        checkCode
        (
            if (Thread.Thread.has_value())
            {
                check( Thread.Thread.value().joinable() == false )
            }
        )

        continue;
    }

    if (::EngineThreadsMutex.try_lock() == false)
    {
        LOG_ERROR(LogTaskSystem, "Failed to lock engine threads mutex. But in this state there should not be any other threads running.")
        ::Jafg::FlushOutStreams();
        ::EngineThreadsMutex.lock(); // Hang this. Probably a deadlock. Let it idle forever. Highly unlikely.
    }
    algo::orphan(&::EngineThreads);
    ::EngineThreadsMutex.unlock();

    LOG_VERBOSE(LogTaskSystem, "Joined {} threads after {} seconds."
        , EngineThreadsSize - /* Master */ 1, algo::time_diff(TimeBeforeJoinedAllThreads, algo::now())
        )

    return;
}

bool Jafg::Tasks::Private::AddThreadsToCurrentTracerSession()
{
    if (Stats::Private::GTracer)
    {
        std::shared_lock Lock(::EngineThreadsMutex);
        for (const LEngineThread& Thread : ::EngineThreads)
        {
            if (Thread.Id == 0)
            {
                continue;
            }

            Stats::Private::GTracer->AddNamedThread({
                LexToString(Thread.ThreadName),
                Thread.Id
            });
        }

        return true;
    }

    return false;
}

#undef PRIVATE_JAFG_GET_UNDERLYING_THREAD_ID
