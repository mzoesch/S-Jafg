// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Async/TaskForward.h"
#include "Async/Runnable.h"

namespace Jafg
{

typedef uint32 LThreadId;

namespace ENamedThreads
{

enum Type : int32
{
    Master          = 0x0,

    ReservedMax     = 0x1FF,
    /*----------------------------------------------------------------------------
        Runtime named threads.
        These threads are generated at runtime and are not known at compile time.
    ----------------------------------------------------------------------------*/
    /*----------------------------------------------------------------------------
    ----------------------------------------------------------------------------*/
    Custom          = 0x200,
    /*----------------------------------------------------------------------------
    ----------------------------------------------------------------------------*/

    /*
     * RESERVED for custom threads. Resolved at runtime.
     */
};

} /* ~Namespace NamedThreads. */
ENGINE_API LSimpleString LexToString(const ENamedThreads::Type Thread);

/**
 * Allows one to specify when a task should be executed on a specific thread if said thread supports it.
 */
namespace ETaskTime
{

enum Type : uint8
{
    Fail = 0,

    /** Run as early as possible on the thread. */
    Early    = 0x01 << 0,
    /** Run as late as possible on the thread. */
    Late     = 0x01 << 1,
    /** Run whenever possible on the thread (late or early). */
    Whenever = Early | Late,
};

} /* ~Namespace TaskTime */
ENUM_CLASS_FLAGS(ETaskTime::Type)
ENGINE_API LSimpleString LexToString(const ETaskTime::Type Time);

namespace Tasks
{

/**
 * Map a named thread to a thread id. To be called by the thread itself.
 */
ENGINE_API void RegisterThread(ENamedThreads::Type Thread);

ENGINE_API LSimpleString GetCurrentThreadName();
ENGINE_API LThreadId GetCurrentThreadId();

ENGINE_API bool IsOnThread(ENamedThreads::Type InThread);
ENGINE_API bool IsOnMasterThread();

/**
 * Make a new task that is being executed on the specified thread in the future.
 * Is undefined when the task is being executed. It may be this tick or the tenth tick from now - depending on the
 * current system load.
 */
ENGINE_API void Make(ENamedThreads::Type Thread, ETaskTime::Type Time, TFunction<void()>&& InDelegate);

/** Launch a named thread. This thread is globally accessible by its ENamedThreads::Type. */
template <typename T> ETaskExit::Type LaunchNamedThread(ENamedThreads::Type Thread);
/**
 * Launch a named thread where its ENamedThreads::Type is resolved at function call time.
 * @return The ENamedThreads::Type of the thread that was just launched.
 * @remark This function should generally be preferred over #LaunchNamedThread(ENameThreads::Type) as it avoids
 *         identifier clashing.
 */
template <typename T> ENamedThreads::Type LaunchNamedThread(ETaskExit::Type& OutExit);

namespace Private
{

ENGINE_API extern int32 CustomThreadCounter;

ENGINE_API bool IsThreadRunning(const ENamedThreads::Type Thread);

ENGINE_API auto TryRunTasks(const ENamedThreads::Type Which, const ETaskTime::Type Time, const int32 MaxTasks) -> void;
ENGINE_API auto LaunchNamedThread(ENamedThreads::Type Thread, LRunnable* Runnable, const bool bKillRunnableWhenFinished = true) -> ETaskExit::Type;

ENGINE_API void JoinThread(const ENamedThreads::Type Thread);
ENGINE_API void StopAndJoinRemainingThreads(const bool bJoinTasks = true);

FORCEINLINE ENamedThreads::Type MakeNewCustomNamedThreadId()
{
    check( IsOnMasterThread() )
    return static_cast<ENamedThreads::Type>(CustomThreadCounter++);
}

} /* ~Namespace Private */

template <typename T>
ETaskExit::Type LaunchNamedThread(ENamedThreads::Type Thread)
{
    static_assert(std::is_base_of_v<LRunnable, T>, "T must be derived from LRunnable.");
    return Private::LaunchNamedThread(Thread, new T());
}

template <typename T>
ENamedThreads::Type LaunchNamedThread(ETaskExit::Type& OutExit)
{
    static_assert(std::is_base_of_v<LRunnable, T>, "T must be derived from LRunnable.");

    int32 JafgThreadId = Private::MakeNewCustomNamedThreadId();
    OutExit = LaunchNamedThread<T>(static_cast<ENamedThreads::Type>(JafgThreadId));
    return static_cast<ENamedThreads::Type>(JafgThreadId);
}

} /* ~Namespace Tasks */

} /* ~Namespace Jafg */
