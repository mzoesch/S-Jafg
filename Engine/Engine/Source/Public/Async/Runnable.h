// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Async/TaskForward.h"

namespace Jafg
{

namespace ERunnableStopReason
{

enum Type : uint8
{
    EngineTermination = 0,
    RequestedStop     = 1,
    Custom            = 2,
};

} /* ~Namespace RunnableStopReason */

/**
 * Generates a unique thread for itself and runs the #Run method on it. For long-running tasks only.
 * For short running tasks, use #Tasks::Make.
 */
class ENGINE_API LRunnable
{
public:

    LRunnable() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LRunnable)
    virtual ~LRunnable() = default;

    /**
     * Called on aggregating thread to initialize the runnable object.
     * @return True if initialization was successful, false otherwise. False will cause the thread to #Exit and not
     *         call #Run.
     */
    virtual ETaskExit::Type Initialize() { return ETaskExit::Success; }

    /**
     * Runs the runnable object on a separate thread.
     * When this method terminates, #Exit will be called.
     */
    virtual ETaskExit::Type Run() = 0;

    /**
     * Call from any other thread to early stop this thread's execution.
     */
    virtual void Stop(const ERunnableStopReason::Type InType) { }

    /**
     * Called in the context of the thread that wishes to exit.
     */
    virtual void Exit() { }

    FORCEINLINE LSimpleString GetHumanReadableName() const { return this->HumanReadableName; }

protected:

    LSimpleString HumanReadableName = "Unnamed Thread";
};

} /* ~Namespace Jafg */
