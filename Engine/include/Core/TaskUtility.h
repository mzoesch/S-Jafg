// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/TaskForward.h"
#include "Framework/Runnable.h"
#if !JAFG_PLATFORM_WASM
    #include <atomic>
    #include <thread>
#endif /* !JAFG_PLATFORM_WASM */

namespace Jafg
{

#if JAFG_WITH_MSVC
    typedef u32 LThreadId;
#elif JAFG_WITH_GCC || JAFG_WITH_CLANG
    typedef __gthread_t LThreadId;
#elif JAFG_PLATFORM_WASM
    typedef unsigned long LThreadId;
#else /* JAFG_PLATFORM_WASM */
    #error "Missing implementation for this platform."
#endif /* !JAFG_PLATFORM_WASM */

typedef std::move_only_function<void(void)> LTaskDelegate;

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

#if JAFG_WITH_REST_CLS
    ReSTCli         = 0x10,
#endif /* JAFG_WITH_REST_CLS */

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
ENGINE_API LString LexToString(ENamedThreads::Type Thread);

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

    //# Flush. For internal use only.
    _Flush = std::numeric_limits<u8>::max(),
};

} /* ~Namespace TaskTime */
ENUM_CLASS_FLAGS(ETaskTime::Type)
ENGINE_API LString LexToString(ETaskTime::Type Time);

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

ENGINE_API  bool HasThread(ENamedThreads::Type InThreadName);
FORCEINLINE bool HasMasterThread() { return HasThread(ENamedThreads::Master); }
FORCEINLINE bool HasRendererThread() { return HasThread(ENamedThreads::Renderer); }
FORCEINLINE bool HasWorkerThread() { return HasThread(ENamedThreads::WorkerThread); }
FORCEINLINE bool HasReSTCliThread();

ENGINE_API  bool IsOnThread(ENamedThreads::Type InThreadName);
FORCEINLINE bool IsOnMasterThread() { return IsOnThread(ENamedThreads::Master); }
FORCEINLINE bool IsOnRendererThread() { return IsOnThread(ENamedThreads::Renderer); }
FORCEINLINE bool IsOnWorkerThread() { return IsOnThread(ENamedThreads::WorkerThread); }
FORCEINLINE bool IsOnReSTCliThread();

//#
//# Make a new task that is being executed on the specified thread in the future.
//# Is undefined when the task is being executed. It may be this tick or the tenth tick from now - depending on the
//# current system load.
//#
ENGINE_API void Make(ENamedThreads::Type InThreadName, ETaskTime::Type InPreferredTime, LTaskDelegate InDelegate);

//# Launch a named thread. This thread is globally accessible by its ENamedThreads::Type.
template <typename T, typename... Args> requires std::is_base_of_v<LRunnable, T>
ETaskExit::Type LaunchNamedThread(ENamedThreads::Type Thread, Args&&... InArgs);
//#
//# Launch a named thread where its ENamedThreads::Type is resolved at function call time.
//# @return The ENamedThreads::Type of the thread that was just launched.
//# @remark This function should generally be preferred over #LaunchNamedThread(ENameThreads::Type) as it avoids
//#         identifier clashing.
//#
template <typename T, typename... Args>
ENamedThreads::Type LaunchNamedThread(ETaskExit::Type* OutExit, Args&&... InArgs);

ENGINE_API bool IsThreadRunning(ENamedThreads::Type InThreadName);

enum ERunAllTasks : i32 { RunAllTasks = 0, };
//# @return The number of tasks that were run.
ENGINE_API i32 TryRunTasks(ENamedThreads::Type Which, ETaskTime::Type Time, i32 MaxTasks);

//#
//# Calls Fn(I) for every I in [0, Count) on up to hardware-concurrency many threads (the calling thread included)
//# and returns once every call has returned. Meant for short-lived, independent and heavy work, e.g., decoding files.
//# Fn has to be safe to call concurrently for different indices. Runs serially on platforms without threads.
//#
template<typename TFn>
void ParallelFor(std::size_t Count, TFn&& Fn);

ENGINE_API void StopThread(ENamedThreads::Type ThreadName);
ENGINE_API void JoinThread(ENamedThreads::Type ThreadName);

namespace Private
{

ENGINE_API extern i32 CustomThreadCounter;

ENGINE_API auto LaunchNamedThread(ENamedThreads::Type ThreadName, LRunnable* Runnable, bool bKillRunnableWhenFinished = true) -> ETaskExit::Type;
ENGINE_API void StopAndJoinRemainingThreads(bool bJoinTasks = true);

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

FORCEINLINE bool HasReSTCliThread()
{
#if JAFG_WITH_REST_CLS
    return HasThread(ENamedThreads::ReSTCli);
#else /* JAFG_WITH_REST_CLS */
    return false;
#endif /* !JAFG_WITH_REST_CLS */
}

FORCEINLINE bool IsOnReSTCliThread()
{
#if JAFG_WITH_REST_CLS
    return IsOnThread(ENamedThreads::ReSTCli);
#else /* JAFG_WITH_REST_CLS */
    return false;
#endif /* JAFG_WITH_REST_CLS */
}

template <typename T, typename... Args> requires std::is_base_of_v<LRunnable, T>
FORCEINLINE ETaskExit::Type LaunchNamedThread(const ENamedThreads::Type Thread, Args&&... InArgs)
{
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

template<typename TFn>
void ParallelFor(std::size_t const Count, TFn&& Fn)
{
#if JAFG_PLATFORM_WASM
    for (std::size_t I{0}; I < Count; ++I)
    {
        Fn(I);
    }
#else /* JAFG_PLATFORM_WASM */
    if (Count <= 1)
    {
        if (Count == 1)
        {
            Fn(0uz);
        }
        return;
    }

    std::size_t const Workers{std::min<std::size_t>(Count, std::max(1u, std::thread::hardware_concurrency()))};
    std::atomic<std::size_t> Next{0};
    auto Work{[&Next, &Fn, Count]
    {
        for (std::size_t I; (I = Next.fetch_add(1, std::memory_order_relaxed)) < Count;)
        {
            Fn(I);
        }
    }};

    std::vector<std::jthread> Threads;
    Threads.reserve(Workers - 1);
    for (std::size_t I{1}; I < Workers; ++I)
    {
        Threads.emplace_back(Work);
    }
    Work();
    /* Joined by the destructors of the threads. */
#endif /* !JAFG_PLATFORM_WASM */
}

FORCEINLINE ENamedThreads::Type Private::MakeNewCustomNamedThreadId()
{
    check( IsOnMasterThread() )
    return static_cast<ENamedThreads::Type>(CustomThreadCounter++);
}

} /* ~Namespace Tasks */

} /* ~Namespace Jafg */
