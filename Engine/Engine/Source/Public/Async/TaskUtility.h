// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Async/TaskForward.h"
#include "Async/Runnable.h"

namespace Jafg
{

#if PLATFORM_WINDOWS_WITH_MSVC
    typedef u32 LThreadId;
#elif PLATFORM_WINDOWS_WITH_GCC || LAL_WITH_CLANG
    typedef __gthread_t LThreadId;
#elif PLATFORM_WASM
    typedef unsigned long LThreadId;
#else /* PLATFORM_WASM */
    #error "Missing implementation for this platform."
#endif /* !PLATFORM_WASM */

typedef TFunction<void(void)> LTaskDelegate;

namespace ENamedThreads
{

enum Type : i32
{
    Master          = 0x0,
    Renderer        = Master, // Currently the same thread, but in the future this will change.

    //#
    //# A thread for medium-sized tasks that are too heavy for the master thread, but not heavy enough to be
    //# executed on a thread specifically designed for the said task.
    //#
    WorkerThread    = 0x3,

    ReservedMax     = 0x1FF,

    ///////////////////////////////////////////////////////////////////////////////
    // Runtime named threads.
    // These threads are generated at runtime and are not known at compile time.
    ///////////////////////////////////////////////////////////////////////////////
    Custom          = 0x200,
    //
    // RESERVED for custom threads. Resolved at runtime.
    //

    Failure         = 0x7FFFFFFF,
};

} /* ~Namespace NamedThreads */
ENGINE_API LString LexToString(const ENamedThreads::Type Thread);

//#
//# Allows one to specify when a task should be executed on a specific thread if said thread supports it.
//#
namespace ETaskTime
{

enum Type : u8
{
    Fail = 0,

    //# Run as early as possible on the thread.
    Early = 0x01 << 0,

    //# Run as late as possible on the thread.
    Late = 0x01 << 1,

    //# Run early or outside of threads' tick - e.g., during an undefined program state...
    NoTickDangerous = 0x01 << 2, // Only during undefined states. May never happen.
    NoTick = NoTickDangerous | Early, // <-- very safe.

    //# Run after the core package has been loaded.
    AfterCorePackageLoadDangerous = 0x01 << 3,
    AfterCorePackageLoad = AfterCorePackageLoadDangerous | Early,

    BeforeEngineInitButAfterAllocDangerous = 0x01 << 4,
    BeforeEngineInitButAfterAlloc = BeforeEngineInitButAfterAllocDangerous | Early,

    AfterEngineInitDangerous = 0x01 << 5,
    AfterEngineInit = AfterEngineInitDangerous | Early,

    //# Run whenever possible on the thread (late or early).
    Whenever = Early | Late | NoTick,
};

} /* ~Namespace TaskTime */
ENUM_CLASS_FLAGS(ETaskTime::Type)
ENGINE_API LString LexToString(const ETaskTime::Type Time);

namespace Tasks
{

//#
//# Map a named thread to a thread id. To be called by the thread itself.
//#
ENGINE_API void RegisterThread(ENamedThreads::Type InThreadName);

ENGINE_API  auto GetCurrentThreadDisplayName() -> LString;
ENGINE_API  auto GetCurrentThreadDisplayNameChecked() -> LString;
ENGINE_API  auto GetCurrentThreadDisplayNameAsserted() -> LString;
ENGINE_API  auto GetCurrentThreadName() -> ENamedThreads::Type;
FORCEINLINE auto GetCurrentThreadNameChecked() -> ENamedThreads::Type;
FORCEINLINE auto GetCurrentThreadNameAsserted() -> ENamedThreads::Type;
ENGINE_API  auto GetCurrentThreadId() -> LThreadId;

ENGINE_API  bool IsOnThread(const ENamedThreads::Type InThreadName);
FORCEINLINE bool IsOnMasterThread() { return IsOnThread(ENamedThreads::Master); }
FORCEINLINE bool IsOnRendererThread() { return IsOnThread(ENamedThreads::Renderer); }

//#
//# Make a new task that is being executed on the specified thread in the future.
//# Is undefined when the task is being executed. It may be this tick or the tenth tick from now - depending on the
//# current system load.
//#
ENGINE_API void Make(const ENamedThreads::Type InThreadName, const ETaskTime::Type InPreferredTime, LTaskDelegate&& InDelegate);

//# Launch a named thread. This thread is globally accessible by its ENamedThreads::Type.
template <typename T, typename... Args>
ETaskExit::Type LaunchNamedThread(ENamedThreads::Type Thread, Args&&... InArgs);
//#
//# Launch a named thread where its ENamedThreads::Type is resolved at function call time.
//# @return The ENamedThreads::Type of the thread that was just launched.
//# @remark This function should generally be preferred over #LaunchNamedThread(ENameThreads::Type) as it avoids
//#         identifier clashing.
//#
template <typename T, typename... Args>
ENamedThreads::Type LaunchNamedThread(ETaskExit::Type* OutExit, Args&&... InArgs);

ENGINE_API bool IsThreadRunning(const ENamedThreads::Type InThreadName);

enum ERunAllTasks : i32 { RunAllTasks = 0, };
//# @return The number of tasks that were run.
ENGINE_API i32 TryRunTasks(const ENamedThreads::Type Which, const ETaskTime::Type Time, const i32 MaxTasks);

ENGINE_API void StopThread(const ENamedThreads::Type ThreadName);
ENGINE_API void JoinThread(const ENamedThreads::Type ThreadName);

namespace Private
{

ENGINE_API extern i32 CustomThreadCounter;

ENGINE_API auto LaunchNamedThread(ENamedThreads::Type ThreadName, LRunnable* Runnable, const bool bKillRunnableWhenFinished = true) -> ETaskExit::Type;
ENGINE_API void StopAndJoinRemainingThreads(const bool bJoinTasks = true);

//# @return True if added.
ENGINE_API bool AddThreadsToCurrentTracerSession();

FORCEINLINE ENamedThreads::Type MakeNewCustomNamedThreadId();

} /* ~Namespace Private */

FORCEINLINE ENamedThreads::Type GetCurrentThreadNameChecked()
{
    const ENamedThreads::Type Thread = GetCurrentThreadName();
    check( Thread != ENamedThreads::Failure )
    return Thread;
}

FORCEINLINE ENamedThreads::Type GetCurrentThreadNameAsserted()
{
    const ENamedThreads::Type Thread = GetCurrentThreadName();
    jassert( Thread != ENamedThreads::Failure )
    return Thread;
}

template <typename T, typename... Args>
FORCEINLINE ETaskExit::Type LaunchNamedThread(ENamedThreads::Type Thread, Args&&... InArgs)
{
    static_assert(std::is_base_of_v<LRunnable, T>, "T must be derived from LRunnable.");
    return Private::LaunchNamedThread(Thread, new T(std::forward<Args>(InArgs)...));
}

template <typename T, typename... Args>
FORCEINLINE ENamedThreads::Type LaunchNamedThread(ETaskExit::Type* OutExit, Args&&... InArgs)
{
    static_assert(std::is_base_of_v<LRunnable, T>, "T must be derived from LRunnable.");
    check( OutExit )

    i32 JafgThreadId = Private::MakeNewCustomNamedThreadId();
    *OutExit = LaunchNamedThread<T>(static_cast<ENamedThreads::Type>(JafgThreadId), std::forward<Args>(InArgs)...);
    return static_cast<ENamedThreads::Type>(JafgThreadId);
}

FORCEINLINE ENamedThreads::Type Private::MakeNewCustomNamedThreadId()
{
    check( IsOnMasterThread() )
    return static_cast<ENamedThreads::Type>(CustomThreadCounter++);
}

} /* ~Namespace Tasks */

} /* ~Namespace Jafg */
