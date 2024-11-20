// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Async/TaskUtility.h"
#include "../Engine/CoreGlobals.h"
#include "Async/Runnable.h"
#include "Core/Application.h"

namespace
{

static_assert(
    sizeof(::Jafg::LThreadId) == sizeof(std::thread::id),
    "The size of std::thread::id and uint32 must be the same."
);
static_assert(
    std::is_same_v<::Jafg::LThreadId, _Thrd_id_t>,
    "The maximum value of std::thread::id and uint32 must be the same."
);

/**
 * When accessing and chaining #JoinableThreads and #RegisteredThreadIds, use this mutex.
 */
std::mutex JoinableThreadsMutex;

/**
 * Whether the engine closed #JoinableThreads and #RegisteredThreadIds, new threads cannot be added and existing
 * threads cannot remove themselves from those vectors. They should just exit without cleaning up - the engine
 * will handle that.
 */
bool bClosedJoinableThreadsVector = false;

/** Threads that are being joined when the engine is existing. */
struct LJoinableThread final
{
    ::Jafg::ENamedThreads::Type ThreadTy;
    std::thread                 Thread;
    Jafg::LRunnable*            Runnable;
    bool                        bKillRunnableWhenFinished;
};

/** Threads that will be joined by the engine. */
std::vector<LJoinableThread> JoinableThreads;

struct LRegisteredThread final
{
    ::Jafg::LThreadId         Id;
    Jafg::ENamedThreads::Type ThreadTy;
};
/** Current registered threads that may be accessed by ENamedThreads::Type. */
Jafg::TdhArray<LRegisteredThread> RegisteredThreadIds;

/** Global queue for tasks that are being executed on the specified thread in the future. */
struct LTaskQueue final
{
    FORCEINLINE LTaskQueue(const Jafg::ETaskTime::Type InTime) : Time(InTime), Delegate(new Jafg::TFunction<void(void)>()) {}
    FORCEINLINE LTaskQueue(const Jafg::ETaskTime::Type InTime, Jafg::TFunction<void()>&& InDelegate)
        : Time(InTime)
    {
        this->Delegate = new Jafg::TFunction<void(void)>();
        *this->Delegate = std::move(InDelegate);
        checkSlow( InDelegate.IsSet() == false )
    }

    Jafg::ETaskTime::Type        Time;
    Jafg::TFunction<void(void)>* Delegate = nullptr;
};
std::map<Jafg::ENamedThreads::Type, std::vector<LTaskQueue>> TaskQueue;

LJoinableThread* FindJoinableThread(const Jafg::ENamedThreads::Type InName)
{
    for (LJoinableThread& T : JoinableThreads)
    {
        if (T.ThreadTy == InName)
        {
            return &T;
        }
    }

    return nullptr;
}

} /* ~Namespace <Anonymous> */

namespace Jafg::Tasks::Private
{

ENGINE_API int32 CustomThreadCounter = ENamedThreads::Custom + 1;

} /* ~Namespace Jafg::Tasks::Private */

Jafg::LSimpleString Jafg::LexToString(const ENamedThreads::Type Thread)
{
    switch (Thread)
    {
    case ENamedThreads::Master: { return "Master"; }
    default: { return "Custom"; }
    }
}

Jafg::LSimpleString Jafg::LexToString(const ETaskTime::Type Time)
{
    switch (Time)
    {
    case ETaskTime::Fail: { panicMsgf( "Encountered invalid task time." ) return ""; }
    case ETaskTime::Early: { return "Early"; }
    case ETaskTime::Late: { return "Late"; }
    case ETaskTime::Whenever: { return "Whenever"; }
    default: { panicMsgf( "Encountered invalid task time." ) return ""; }
    }
}

void Jafg::Tasks::RegisterThread(ENamedThreads::Type Thread)
{
    if (::RegisteredThreadIds.ContainsByPredicate(
        [Thread] (const LRegisteredThread& i)
        {
            return i.ThreadTy == Thread;
        }
    ))
    {
        LOG_ERROR(LogTaskUtility, "Thread {} already registered.", LexToString(Thread))
        return;
    }

    ::RegisteredThreadIds.Emplace(std::this_thread::get_id()._Get_underlying_id(), Thread);

    return;
}

Jafg::LSimpleString Jafg::Tasks::GetCurrentThreadName()
{
    const LThreadId CurThreadId = std::this_thread::get_id()._Get_underlying_id();
    for (const LRegisteredThread& Thread : ::RegisteredThreadIds)
    {
        if (Thread.Id == CurThreadId)
        {
            return LexToString(Thread.ThreadTy);
        }

        continue;
    }

    return "NotRegistered";
}

Jafg::LThreadId Jafg::Tasks::GetCurrentThreadId()
{
    return std::this_thread::get_id()._Get_underlying_id();
}

bool Jafg::Tasks::IsOnThread(ENamedThreads::Type InThread)
{
    if (const LRegisteredThread* ThreadTy = ::RegisteredThreadIds.FindByPredicate(
    [InThread] (const LRegisteredThread& Thread)
    {
        return Thread.ThreadTy == InThread;
    }); ThreadTy)
    {
        return ThreadTy->Id == std::this_thread::get_id()._Get_underlying_id();
    }

    return false;
}

bool Jafg::Tasks::IsOnMasterThread()
{
    return IsOnThread(ENamedThreads::Master);
}

void Jafg::Tasks::Make(const ENamedThreads::Type Thread, const ETaskTime::Type Time, TFunction<void()>&& InDelegate)
{
    check( InDelegate.IsSet() )

    std::vector<LTaskQueue>& Queue = ::TaskQueue[Thread];
    Queue.emplace_back(Time);

    checkSlow( Queue.back().Delegate->IsSet() == false )

    *Queue.back().Delegate = std::move(InDelegate);
#if DO_CHECKS
    Queue.back().Delegate->CheckForValidCall();
#endif /* DO_CHECKS */

    return;
}

bool Jafg::Tasks::Private::IsThreadRunning(ENamedThreads::Type Thread)
{
    return ::RegisteredThreadIds.ContainsByPredicate(
        [Thread] (const LRegisteredThread& i)
        {
            return i.ThreadTy == Thread;
        }
    );
}

void Jafg::Tasks::Private::TryRunTasks(const ENamedThreads::Type Which, const ETaskTime::Type Time, const int32 MaxTasks)
{
    std::vector<LTaskQueue>& Queue = ::TaskQueue[Which];

    if constexpr (IS_COMPILED_LOG(LogTasks, Verbose))
    {
        if (Queue.empty() == false)
        {
            LOG_VERBOSE(LogTasks, "Queued {} tasks on thread [{}].", Queue.size(), LexToString(Which))
        }
    }

    int32 RunTasks = 0;
    for (int32 Idx = 0; Queue.empty() == false;)
    {
        if ((RunTasks >= MaxTasks && MaxTasks != 0) || Idx >= static_cast<int32>(Queue.size()))
        {
            break;
        }

        LTaskQueue& Task = Queue[Idx];
        if (Task.Time & Time)
        {
            check( Task.Delegate->IsSet() )
            Task.Delegate->Call();
            Queue.erase(Queue.begin() + Idx);
            ++RunTasks;
            continue;
        }

        ++Idx;

        continue;
    }

    return;
}

Jafg::ETaskExit::Type Jafg::Tasks::Private::LaunchNamedThread(const ENamedThreads::Type Thread, LRunnable* Runnable, const bool bKillRunnableWhenFinished /* = true */)
{
    checkSlow( Runnable )

    if (IsThreadRunning(Thread))
    {
        LOG_ERROR(LogTaskSystem, "Thread {}[{}] already running.", LexToString(Thread), static_cast<int32>(Thread))
        delete Runnable;
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
                Runnable->GetHumanReadableName(), LexToString(Thread),
                GetCurrentThreadName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
            )
            return ErrorLevel;
        }

        if (ErrorLevel >= ETaskExit::SanitizedFailure)
        {
            LSimpleString ErrorLevelStr = LSimpleString::SprintF(
                "Thread {}[{}] with aggregator {}[{}] failed to initialize with a sanitized failure code: {}[{}].",
                Runnable->GetHumanReadableName(), LexToString(Thread),
                GetCurrentThreadName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
            );

            LOG_ERROR(LogTaskSystem, "{}", ErrorLevelStr)
            ::Jafg::RequestEngineExit(EPlatformExit::Fatal, ErrorLevelStr);

            return ErrorLevel;
        }

        if (ErrorLevel >= ETaskExit::TransientFailure)
        {
            LOG_ERROR(
                LogTaskSystem,
                "Thread {}[{}] with aggregator {}[{}] failed to initialize with a transient failure code: {}[{}].",
                Runnable->GetHumanReadableName(), LexToString(Thread),
                GetCurrentThreadName(), GetCurrentThreadId(),
                LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
            )
            return ErrorLevel;
        }

        LOG_ERROR(
            LogTaskSystem,
            "Thread {}[{}] with aggregator {}[{}] failed to initialize with an unknown code: {}[{}].",
            Runnable->GetHumanReadableName(), LexToString(Thread),
            GetCurrentThreadName(), GetCurrentThreadId(),
            LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
        )
        return ErrorLevel;
    }

    ::JoinableThreadsMutex.lock();
    std::thread ThreadObj = std::thread(
        [Thread, Runnable] (void) -> void
        {
            Tasks::RegisterThread(Thread);
            const ETaskExit::Type LambdaErrorLevel = Runnable->Run();
            if (LambdaErrorLevel != ETaskExit::Success)
            {
                if (LambdaErrorLevel >= ETaskExit::Failure)
                {
                    LOG_FATAL(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a non transient failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                    )
                    return;
                }
                if (LambdaErrorLevel >= ETaskExit::SanitizedFailure)
                {
                    LSimpleString ErrorLevelStr = LSimpleString::SprintF(
                        "Thread {}[{}] failed to initialize with a sanitized failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                    );

                    LOG_ERROR(LogTaskSystem, "{}", ErrorLevelStr)
                    ::Jafg::RequestEngineExit(EPlatformExit::Fatal, ErrorLevelStr);
                }
                else if (LambdaErrorLevel >= ETaskExit::TransientFailure)
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a transient failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                    )
                }
                else
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with an unknown code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(LambdaErrorLevel), static_cast<LTaskExit>(LambdaErrorLevel)
                    )
                }
            }

            Runnable->Exit();

            if (::bClosedJoinableThreadsVector)
            {
                return;
            }

            ::JoinableThreadsMutex.lock();
            Tasks::Make(ENamedThreads::Master, ETaskTime::Whenever,
                [Thread, Runnable] (void) -> void
                {
                    if (::bClosedJoinableThreadsVector)
                    {
                        return;
                    }

                    ::JoinableThreadsMutex.lock();

                    ::RegisteredThreadIds.RemoveOnceByPredicateChecked(
                        [Thread] (const LRegisteredThread& i)
                        {
                            return i.ThreadTy == Thread;
                        }
                    );

#if DO_CHECKS
                    bool bRemoved = false;
#endif /* DO_CHECKS */
                    for (uint64 i = 0; i < ::JoinableThreads.size(); ++i)
                    {
                        LJoinableThread& Joinable = ::JoinableThreads[i];
                        checkSlow( Joinable.Runnable )

                        if (Joinable.Runnable == Runnable)
                        {
                            checkSlow( Joinable.Runnable == Runnable )

                            if (Joinable.bKillRunnableWhenFinished)
                            {
                                delete Joinable.Runnable;
                            }
                            Joinable.Runnable = nullptr;
                            if (Joinable.Thread.joinable())
                            {
                                Joinable.Thread.detach(); /* Detach ourselves from master. */
                            }
                            JoinableThreads.erase(JoinableThreads.begin() + static_cast<uint64>(i));
#if DO_CHECKS
                            bRemoved = true;
#endif /* DO_CHECKS */
                            break;
                        }

                        continue;
                    }

#if DO_CHECKS
                    check( bRemoved )
#endif /* DO_CHECKS */

                    ::JoinableThreadsMutex.unlock();

                    LOG_VERBOSE(LogTaskSystem, "Removed thread {}[{}].", LexToString(Thread), static_cast<int32>(Thread))

                    return;
                }
            );
            ::JoinableThreadsMutex.unlock();

            return;
        }
    );
    JoinableThreads.emplace_back(Thread, std::move(ThreadObj), Runnable, bKillRunnableWhenFinished);
    ::JoinableThreadsMutex.unlock();

    return ErrorLevel;
}

void Jafg::Tasks::Private::JoinThread(const ENamedThreads::Type Thread)
{
    ::JoinableThreadsMutex.lock();
    LJoinableThread* Joinable = ::FindJoinableThread(Thread);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    if (Joinable)
    {
        if (Joinable->Thread.joinable())
        {
            ::JoinableThreadsMutex.unlock();
            Joinable->Thread.join();
        }
        else
        {
            LOG_WARNING(LogTaskSystem, "Thread {}[{}] not joinable.", LexToString(Thread), static_cast<int32>(Thread))
            ::JoinableThreadsMutex.unlock();
        }
    }
    else
    {
        LOG_WARNING(LogTaskSystem, "Thread {}[{}] not found.", LexToString(Thread), static_cast<int32>(Thread))
        ::JoinableThreadsMutex.unlock();
    }

    return;
}

void Jafg::Tasks::Private::StopAndJoinRemainingThreads(const bool bJoinTasks /* = true */)
{
    check( IsOnMasterThread() )

    if (bJoinTasks)
    {
        Jafg::Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, 0);
    }

    ::JoinableThreadsMutex.lock();
    ::bClosedJoinableThreadsVector = true;

    if (JoinableThreads.empty() == false)
    {
        LOG_VERBOSE(LogTasks, "Joining {} remaining threads.", ::JoinableThreads.size())

        for (LJoinableThread& Joinable : ::JoinableThreads)
        {
            checkSlow( Joinable.Runnable )
            Joinable.Runnable->Stop(ERunnableStopReason::EngineTermination);
        }

        const Application::Private::LHrcTimePoint TimeBeforeJoin = Application::GetHighestNow();
        for (LJoinableThread& Joinable : ::JoinableThreads)
        {
            checkSlow( Joinable.Runnable )
            if (Joinable.Thread.joinable())
            {
                Joinable.Thread.join();
            }
            delete Joinable.Runnable;
        }
        LOG_INFO(LogTasks, "Finished joining remaining threads after {} seconds.",
            Application::GetTimeDiff(TimeBeforeJoin, Application::GetHighestNow()))
    }
    else
    {
        LOG_VERBOSE(LogTasks, "No remaining threads to join.")
    }

    ::RegisteredThreadIds.Empty();
    ::JoinableThreads.clear();

    ::JoinableThreadsMutex.unlock();

    return;
}
