// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Async/TaskForward.h"

namespace Jafg
{

namespace ERunnableStopReason
{

enum Type : u8
{
    EngineTermination = 0,
    RequestedStop     = 1,
    Custom            = 2,
};

} /* ~Namespace RunnableStopReason */

//#
//# Generates a unique thread for itself and runs the #Run method on it. For long-running tasks only.
//# For short running tasks, use #Tasks::Make.
//#
class LRunnable
{
public:

    ENGINE_API LRunnable();
    ENGINE_API LRunnable(const LString& InHumanReadableName);

    PROHIBIT_ANY_REALLOC_OTHER_THAN_CDR(LRunnable)
    {
        check( CDR.IsStopped() == false )
        check( CDR.HumanReadableName.empty() )

        return;
    }

    virtual ~LRunnable() = default;

    //#
    //# Called on aggregating thread to initialize the runnable object.
    //# @return True if initialization was successful, false otherwise. False will cause the thread to #Exit and not
    //#         call #Run.
    //#
    virtual ETaskExit::Type Initialize() { return ETaskExit::Success; }

    //#
    //# Runs the runnable object on a separate thread.
    //# When this method terminates, #Exit will be called.
    //#
    virtual ETaskExit::Type Run() = 0;

    //#
    //# Call from any other thread to early stop this thread's execution.
    //#
    void Stop(const ERunnableStopReason::Type InType)
    {
        if (this->bStopped == false)
        {
            this->bStopped = true;
            this->OnStop(InType);
        }
    }

    //#
    //# Called in the context of the thread that wishes to exit.
    //#
    virtual void Exit() { this->bStopped = true; }

    //#
    //# Joins the thread. Call from any other thread.
    //#
    ENGINE_API void Join();

    FORCEINLINE bool IsStopped() const noexcept { return this->bStopped; }
    FORCEINLINE const std::atomic_bool* GetStoppedPointer() const noexcept { return &this->bStopped; }
    FORCEINLINE LString const& GetHumanReadableName() const { return this->HumanReadableName; }

protected:

    virtual void OnStop(const ERunnableStopReason::Type InType) { }

    std::atomic_bool bStopped{ false };
    LString HumanReadableName;
};

} /* ~Namespace Jafg */
