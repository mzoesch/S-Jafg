// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

typedef u8 LTaskExit;
namespace ETaskExit
{

enum Type : LTaskExit
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

inline LString LexToString(const ETaskExit::Type Exit)
{
    if (Exit == ETaskExit::Success) { return "Success"; }
    if (Exit == ETaskExit::Illformed) { return "Illformed"; }
    if (Exit == ETaskExit::TransientFailure) { return "TransientFailure"; }
    if (Exit  > ETaskExit::TransientFailure && Exit < ETaskExit::SanitizedFailure) { return "CustomTransientFailure"; }
    if (Exit == ETaskExit::SanitizedFailure) { return "SanitizedFailure"; }
    if (Exit  > ETaskExit::SanitizedFailure && Exit < ETaskExit::Failure) { return "CustomSanitizedFailure"; }
    if (Exit == ETaskExit::Failure) { return "Failure"; }
    if (Exit  > ETaskExit::Failure) { return "CustomFailure"; }

    checkNoEntry()

    return "Unknown";
}

} /* ~Namespace Jafg */
