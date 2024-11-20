// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Async/TaskUtility.h"
#include "../Engine/CoreGlobals.h"
#include "Async/Runnable.h"

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
    std::thread      Thread;
    Jafg::LRunnable* Runnable;
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
    FORCEINLINE LTaskQueue(const Jafg::ETaskTime::Type InTime, Jafg::TFunction<void()>&& InDelegate)
        : Time(InTime)
    {
        Delegate = new Jafg::TFunction<void(void)>();
        Delegate->GetRaw() = InDelegate.GetRaw();
        InDelegate.Reset();
    }

    Jafg::ETaskTime::Type        Time;
    Jafg::TFunction<void(void)>* Delegate = nullptr;
};
std::map<Jafg::ENamedThreads::Type, std::vector<LTaskQueue>> TaskQueue;

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
    std::vector<LTaskQueue>& Queue = ::TaskQueue[Thread];
    Queue.emplace_back(Time, std::move(InDelegate));
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

    int32 RunTasks = 0;
    for (int32 Idx = 0; Queue.empty() == false;)
    {
        if (RunTasks >= MaxTasks || Idx >= static_cast<int32>(Queue.size()))
        {
            break;
        }

        LTaskQueue& Task = Queue[Idx];
        if (Task.Time & Time)
        {
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

Jafg::ETaskExit::Type Jafg::Tasks::Private::LaunchNamedThread(const ENamedThreads::Type Thread, LRunnable* Runnable)
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

    std::thread ThreadObj = std::thread(
        [Thread, Runnable] (void) -> void
        {
            Tasks::RegisterThread(Thread);
            const ETaskExit::Type ErrorLevel = Runnable->Run();
            if (ErrorLevel != ETaskExit::Success)
            {
                if (ErrorLevel >= ETaskExit::Failure)
                {
                    LOG_FATAL(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a non transient failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                    )
                    return;
                }
                if (ErrorLevel >= ETaskExit::SanitizedFailure)
                {
                    LSimpleString ErrorLevelStr = LSimpleString::SprintF(
                        "Thread {}[{}] failed to initialize with a sanitized failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                    );

                    LOG_ERROR(LogTaskSystem, "{}", ErrorLevelStr)
                    ::Jafg::RequestEngineExit(EPlatformExit::Fatal, ErrorLevelStr);
                }
                else if (ErrorLevel >= ETaskExit::TransientFailure)
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with a transient failure code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                    )
                }
                else
                {
                    LOG_ERROR(
                        LogTaskSystem,
                        "Thread {}[{}] failed to initialize with an unknown code: {}[{}].",
                        Runnable->GetHumanReadableName(), LexToString(Thread),
                        LexToString(ErrorLevel), static_cast<LTaskExit>(ErrorLevel)
                    )
                }
            }

            Runnable->Exit();

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
                    delete Joinable.Runnable;
                    Joinable.Runnable = nullptr;
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

            return;
        }
    );
    ThreadObj.detach();

    ::JoinableThreadsMutex.lock();
    JoinableThreads.emplace_back(std::move(ThreadObj), Runnable);
    ::JoinableThreadsMutex.unlock();

    return ErrorLevel;
}

void Jafg::Tasks::Private::StopAndJoinRemainingThreads()
{
    ::JoinableThreadsMutex.lock();
    ::bClosedJoinableThreadsVector = true;

    for (LJoinableThread& Joinable : ::JoinableThreads)
    {
        checkSlow( Joinable.Runnable )
        Joinable.Runnable->Stop(ERunnableStopReason::EngineTermination);
    }

    for (LJoinableThread& Joinable : ::JoinableThreads)
    {
        checkSlow( Joinable.Runnable )
        Joinable.Thread.join();
        delete Joinable.Runnable;
    }

    ::RegisteredThreadIds.Empty();
    ::JoinableThreads.clear();

    ::JoinableThreadsMutex.unlock();

    return;
}
