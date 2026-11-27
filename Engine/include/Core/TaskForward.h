// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Tasks
{

typedef u8 LTaskExit;
namespace EExit
{

enum Type: LTaskExit
{
    //# Ok. Nothing to do.
    Success             = 0,

    //#
    //# The engine has detected that the task is illformed.
    //# The engine will return the status to the illformed caller and will not execute said task - no further action
    //# is taken.
    //#
    Illformed           = 1,

    //#
    //# The task was initialized or executed and failed. The engine will return the status to the caller but will not
    //# take further action.
    //#
    TransientFailure    = 2,
    //
    // RESERVED for custom transient failures.
    //

    //#
    //# The task was initialized or executed and failed. The engine will return the status to the caller and will
    //# automatically request an engine exit. All systems will be shut-downed controlled.
    //#
    SanitizedFailure    = 0x40,
    //
    // RESERVED for custom sanitized failures.
    //

    //#
    //# The task was initialized or executed and failed. The engine will immediately request an engine exit. Only
    //# the absolute core systems will be shut-downed controlled - others will be left in an undefined state at process
    //# termination. The engine will not return the status to the caller.
    //#
    Failure             = 0x80
    //
    // RESERVED for custom failures.
    //
};

} /* ~Namespace TaskExit */

NODISCARD inline LString LexToString(EExit::Type Exit)
{
    if (Exit == EExit::Success) { return "Success"; }
    if (Exit == EExit::Illformed) { return "Illformed"; }
    if (Exit == EExit::TransientFailure) { return "TransientFailure"; }
    if (Exit  > EExit::TransientFailure && Exit < EExit::SanitizedFailure) { return "CustomTransientFailure"; }
    if (Exit == EExit::SanitizedFailure) { return "SanitizedFailure"; }
    if (Exit  > EExit::SanitizedFailure && Exit < EExit::Failure) { return "CustomSanitizedFailure"; }
    if (Exit == EExit::Failure) { return "Failure"; }
    if (Exit  > EExit::Failure) { return "CustomFailure"; }

    checkNoEntry()
    return "Unknown";
}

namespace Detail
{

struct LRunnableLauncher;

} /* ~Namespace Detail */

} /* ~Namespace Tasks */
