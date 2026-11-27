// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/TaskForward.h"
#include "Framework/Runnable.h"

namespace Tasks
{

typedef std::thread::id LThreadId;
NODISCARD FORCEINLINE bool IsOnThread(LThreadId Id) noexcept { return std::this_thread::get_id() == Id; }

//#
//# Rename the current thread to a human-readable name. Might not be respected by all platforms.
//# The name should not exceed 16 bytes, else it will be truncated on some platforms.
//#
ENGINE_API void RenameMe(LString HumanReadableName);

//#
//# A task orchestrator for a single thread or a group of threads.
//#
//# TODO: When the time comes:
//#     1. Use lock-free mpmc queues
//#     2. Use pools with work stealing?
//#
struct LThreadTaskOrchestrator
{
    typedef TFunction2<void()> LTask;
    typedef std::deque<LTask> LTaskList;

    LThreadTaskOrchestrator() noexcept = delete;
    explicit LThreadTaskOrchestrator(LThreadId Id) noexcept
#if JAFG_DO_CHECKS
        : _check_ThreadId{[Id]() -> bool { return IsOnThread(Id); }}
#endif /* JAFG_DO_CHECKS */
    {
        check(Id != LThreadId{})
    }
    explicit LThreadTaskOrchestrator(TFunction2<bool()> _check_ThreadId) noexcept
#if JAFG_DO_CHECKS
        : _check_ThreadId{std::move(_check_ThreadId)}
#endif /* JAFG_DO_CHECKS */
    {
        check(!!this->_check_ThreadId)
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Producer

    //# Append to execute a task that will be executed in this thread group in the future.
    void operator+=(this auto&& Self, auto&& Task) noexcept { Self.Task(std::forward<decltype(Task)>(Task)); }
    void Task(auto&& Task) noexcept
        requires std::is_invocable_r_v<void, std::decay_t<decltype(Task)>&>
    {
        {
            std::scoped_lock _{this->Mutex};
            this->List.emplace_back(std::forward<decltype(Task)>(Task));
        }
        this->Cv.notify_one();
    }

    //# Append to execute a list of tasks that will be executed in this thread group in the future.
    void Tasks(LTaskList&& Tasks) noexcept
    {
        if (!Tasks.empty())
        {
            bool bMultiple{Tasks.size() > 1};
            {
                std::scoped_lock _{this->Mutex};
                this->List.insert(this->List.end()
                    , std::make_move_iterator(Tasks.begin())
                    , std::make_move_iterator(Tasks.end())
                    );
            }
            bMultiple
                ? this->Cv.notify_all()
                : this->Cv.notify_one()
                ;
        }
    }

    //#
    //# Append to execute a task that will be executed in this thread group in the future returning a future to
    //# easily get the result of the task.
    //#
    template<typename T>
    auto Future(T&& Task) -> std::future<std::invoke_result_t<T&>>
        requires std::is_invocable_v<T&>
    {
        typedef std::invoke_result_t<T&> inner;
        std::packaged_task<inner()> Package{std::forward<decltype(Task)>(Task)};
        auto Future{Package.get_future()};
        {
            std::scoped_lock _{this->Mutex};
            this->List.emplace_back(std::move(Package));
        }
        this->Cv.notify_one();
        return Future;
    }

    //#
    //# Append to execute tasks that will be executed in this thread group in the future returning futures to
    //# easily get the results of the tasks.
    //#
    template<typename T>
    auto Futures(T&& Tasks) -> TArray<std::future<std::invoke_result_t<typename std::remove_cvref_t<T>::value_type&>>>
        requires
               std::is_invocable_v<typename std::remove_cvref_t<T>::value_type&>
            && std::is_rvalue_reference_v<decltype(Tasks)>
    {
        typedef std::invoke_result_t<typename std::remove_cvref_t<T>::value_type&> inner;
        TArray<std::future<inner>> Out;
        if (!Tasks.empty())
        {
            bool bMultiple{Tasks.size() > 1};
            Out.reserve(Tasks.size());
            {
                std::scoped_lock _{this->Mutex};
                for (auto&& Task: Tasks)
                {
                    std::packaged_task<inner()> Package{std::move(Task)};
                    Out.emplace_back(Package.get_future());
                    this->List.emplace_back(std::move(Package));
                }
            }
            bMultiple
                ? this->Cv.notify_all()
                : this->Cv.notify_one()
                ;
        }
        return Out;
    }

    // ~Producer
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // Consumer

    //#
    //# Consumes a single task from the queue and executes it.
    //# @return True, if a task was consumed and executed, false if the queue was empty.
    //#
    bool Consume()
    {
        check(this->_check_ThreadId())
        return this->ConsumeUnchecked();
    }

    //# Consumes all tasks from the queue and executes them returning after the list is empty.
    void ConsumeAll()
    {
        while (this->Consume());
    }

    //# Consumes at least one if the list was not empty, even if stoken is requested to stop.
    void ConsumeAll(std::stop_token const& Stoken)
    {
        while (this->Consume() && !Stoken.stop_requested());
    }

    //# Consumes at least one if the list was not empty, even if the duration has passed.
    void ConsumeFor(f64 Duration)
    {
        const auto Start{algo::now()};
        while (this->Consume() && algo::time_diff(Start) < Duration);
    }

    //# Consumes at least one if the list was not empty, even if the duration has passed or stoken is requested to stop.
    void ConsumeFor(f64 Duration, std::stop_token const& Stoken)
    {
        const auto Start{algo::now()};
        while (this->Consume() && algo::time_diff(Start) < Duration && !Stoken.stop_requested());
    }

    //#
    //# Consumes at least one if the list was not empty, even if the stoken was requested.
    //# Then blocks the thread until the stoken is requested or a new task is submitted.
    //#
    void ConsumeAndWaitUntil(std::stop_token const& Stoken)
    {
        check(this->_check_ThreadId())

        {
            std::scoped_lock _{this->Mutex};
            ++this->BlockingConsumers;
        }

        /* Keep it consistent. Consume at least one even if the stoken is requested. */
        (void)this->Consume();

        std::unique_lock Lock{this->Mutex};
        while (true)
        {
            this->Cv.wait(Lock, Stoken, [this]{ return !this->List.empty(); });

            if (Stoken.stop_requested())
            {
                check(this->BlockingConsumers > 0)
                --this->BlockingConsumers;
                break;
            }

            {
                LTask Task{std::move(this->List.front())};
                this->List.pop_front();
                Lock.unlock();

                check(!!Task)
                Task();
            }

            Lock.lock();
        }
    }

    //#
    //# Consumes all tasks on the calling thread, regardless of the thread group, including the ones submitted while
    //# draining. Only for when no consumer of this group is left, e.g., after all workers have been joined at shutdown,
    //# so that nothing that was submitted is silently dropped.
    //#
    void Drain()
    {
        {
            std::scoped_lock _{this->Mutex};
            check(this->BlockingConsumers == 0 && "Draining while consumers of this thread group are still running.")
        }
        while (this->ConsumeUnchecked());
    }

    // ~Consumer
    ///////////////////////////////////////////////////////////////////////////////

private:

    bool ConsumeUnchecked()
    {
        LTask Task;
        {
            std::scoped_lock _{this->Mutex};
            if (this->List.empty())
            {
                return false;
            }
            Task = std::move(this->List.front());
            this->List.pop_front();
        }
        check(!!Task)
        Task();

        return true;
    }

#if JAFG_DO_CHECKS
    TFunction2<bool()> _check_ThreadId;
#endif /* JAFG_DO_CHECKS */

    std::mutex Mutex;
    std::condition_variable_any Cv;
    LTaskList List;
    //# The number of threads inside #ConsumeAndWaitUntil. Guarded by Mutex.
    std::size_t BlockingConsumers{0};
};

namespace Detail
{

ENGINE_API LThreadId GetMasterThreadId();
ENGINE_API void SetIsWorkerThread(bool bWorker) noexcept;

} /* ~Namespace Detail */

NODISCARD FORCEINLINE bool IsOnMasterThread() noexcept { return IsOnThread(Detail::GetMasterThreadId()); }
// TODO: ... Change this later to a proper renderer thread.
NODISCARD FORCEINLINE bool IsOnRendererThread() noexcept { return IsOnMasterThread(); }
NODISCARD ENGINE_API bool IsOnWorkerThread() noexcept;
//# The number of threads currently consuming the #Worker thread group. Only a hint, it may change at any time.
NODISCARD ENGINE_API std::size_t GetWorkerThreadCount() noexcept;

//# Submit master related work here.
ENGINE_API extern LThreadTaskOrchestrator Master;
//# Submit other work here. Might also be executed on the Master thread if frame time allows it.
ENGINE_API extern LThreadTaskOrchestrator Worker;

//#
//# Calls Fn(I) for every I in [0, Count) and returns once every call has returned. The calling thread takes part
//# and helpers are submitted to the #Worker thread group; no threads are created.
//# Meant for independent and heavy work, e.g., decoding files. Fn has to be safe to call concurrently for different
//# indices. Runs serially on the calling thread if there are no other worker threads.
//#
//# Never deadlocks, even when called from within a worker task or while the worker group is saturated: The calling
//# thread claims every index no helper got to, so it only ever waits for indices that are currently executing.
//#
template<typename TFn>
    requires std::invocable<std::remove_reference_t<TFn>&, std::size_t>
void ParallelFor(std::size_t Count, TFn&& Fn);

namespace Detail
{

template<typename TFn>
struct TParallelForState final
{
    TParallelForState(std::size_t InCount, TFn* InFn) noexcept : Count{InCount}, Fn{InFn} { }

    //# Claims and executes indices until every index is claimed.
    void Work()
    {
        std::size_t Executed{0};
        for (std::size_t I; (I = this->Next.fetch_add(1, std::memory_order_relaxed)) < this->Count; ++Executed)
        {
            std::invoke(*this->Fn, I);
        }

        /* Releases this thread's side effects of Fn to the caller, which acquires the final count. */
        if (Executed > 0 && this->Done.fetch_add(Executed, std::memory_order_acq_rel) + Executed == this->Count)
        {
            this->Done.notify_all();
        }
    }

    std::size_t const Count;
    //#
    //# Points into the stack of the caller. Only dereferenced after claiming an index below Count, and the caller
    //# does not return before all of those have been executed. A helper starting late never touches it.
    //#
    TFn* const Fn;
    std::atomic<std::size_t> Next{0};
    //# The number of indices that have been executed.
    std::atomic<std::size_t> Done{0};
};

} /* ~Namespace Detail */

template<typename TFn>
    requires std::invocable<std::remove_reference_t<TFn>&, std::size_t>
void ParallelFor(std::size_t const Count, TFn&& Fn)
{
    /* The calling thread does not count as a helper if it is a worker itself. */
    std::size_t const Others{GetWorkerThreadCount() - (IsOnWorkerThread() ? 1uz : 0uz)};
    std::size_t const Helpers{Count > 0 ? maths::min(Count - 1, Others) : 0uz};
    if (Helpers == 0)
    {
        for (std::size_t I{0}; I < Count; ++I)
        {
            std::invoke(Fn, I);
        }
        return;
    }

    STAT_ZONE("Tasks::ParallelFor")

    typedef Detail::TParallelForState<std::remove_reference_t<TFn>> LState;
    /*
     * Shared with the helpers, as they may start after this function has returned (all indices were claimed by
     * others in the meantime), and the helper that finishes last still notifies after the caller might have woken up.
     */
    auto State{std::make_shared<LState>(Count, std::addressof(Fn))};

    for (std::size_t I{0}; I < Helpers; ++I)
    {
        Worker.Task([State]
        {
            STAT_ZONE("Tasks::ParallelFor::Helper")
            State->Work();
        });
    }

    State->Work();

    for (std::size_t Done; (Done = State->Done.load(std::memory_order_acquire)) < Count;)
    {
        State->Done.wait(Done, std::memory_order_acquire);
    }
}

//#
//# Without Delete, the caller owns the runnable and has to #Jafg::LRunnable::Join it before destroying it, even if
//# it exited on its own. With Delete, the runnable owns itself: It is joined and deleted on the master thread after
//# it exited, and nobody else may join or destroy it. You may use #DefaultRunnableDelete.
//#
EExit::Type LaunchRunnable(Jafg::LRunnable& Runnable, TFunction2<void(Jafg::LRunnable&)> Delete = {});
NODISCARD inline TFunction2<void(Jafg::LRunnable&)> DefaultRunnableDelete()
{
    return [](Jafg::LRunnable& Runnable)
    {
        check(!Runnable.Joinable())
        delete &Runnable;
    };
}

} /* ~Namespace Tasks */
