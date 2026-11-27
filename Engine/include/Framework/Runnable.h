// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/TaskForward.h"

namespace Jafg
{

SERDE_DECLARE_TRIVIAL_ENUM(ERunnableStopReason, u8,
    EngineTermination,
    RequestedStop,
    Custom
    )

//#
//# Generates a unique thread for itself and runs the #Run method on it. For long-running tasks only.
//# For short running tasks, use #Tasks::Make.
//#
class ENGINE_API LRunnable
{
    friend Tasks::Detail::LRunnableLauncher;

public:

    LRunnable() = default;
    explicit LRunnable(LString HumanReadableName): HumanReadableName{std::move(HumanReadableName)} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LRunnable)
    //#
    //# The thread must be stopped and joined before destruction. The derived parts are destroyed before the
    //# thread would otherwise be joined here, while #Run and #Exit might still be executing on them.
    //#
    virtual ~LRunnable()
    {
        check(!this->Thread.joinable() && "Runnable destroyed with a running or unjoined thread.")
    }

    NODISCARD FORCEINLINE LString const& GetHumanReadableName() const noexcept { return this->HumanReadableName; }

    //# Called on aggregating thread to initialize the runnable object.
    virtual Tasks::EExit::Type Initialize() { return Tasks::EExit::Success; }

    //#
    //# Runs the runnable object on a separate thread.
    //# When this method terminates, #Exit will be called.
    //#
    virtual Tasks::EExit::Type Run(std::stop_token const& Stoken) = 0;

    //#
    //# Call from the master thread to early stop this thread's execution. Master only, as the master thread is the
    //# one that launches and joins runnables, so no other thread may access the thread object concurrently.
    //# @return True, if this call requested the stop, false if the thread is not joinable or was already requested.
    //#
    bool Stop(ERunnableStopReason Reason);

    //# Called in the context of the thread that wishes to exit.
    virtual void Exit() {}

    //# Joins the thread. Call from any other thread.
    void Join()
    {
        if (this->Thread.joinable())
        {
            this->Thread.join();
        }
    }

    NODISCARD FORCEINLINE bool Joinable() const
    {
        return this->Thread.joinable();
    }

protected:

    void SetThreadName(LString Name) noexcept
    {
        check(!this->Thread.joinable())
        this->HumanReadableName = std::move(Name);
    }

    //#
    //# Called on the master thread by #Stop.
    //# Might actually never be called if the thread exists on their own before any stop request was made.
    //#
    virtual void OnStop(ERunnableStopReason Reason) {}

private:

    LString HumanReadableName;
    std::jthread Thread;
};

} /* ~Namespace Jafg */
