// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace ELogVerbosity
{

enum Type : u8
{
    Trace       = 1,
    Verbose     = 2,
    Info        = 3,
    Warning     = 4,
    Error       = 5,
    Fatal       = 6,

    Num         = 6,

    /** Kill all logs that are below this level. */
    KillLog     = 255
};

} /* ~Namespace ELogVerbosity */

typedef ELogVerbosity::Type ELogVerbosityType;

LAL_API LString LexToString(const ELogVerbosityType Verbosity);

} /* ~Namespace Jafg */
