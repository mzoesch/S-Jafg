// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

namespace ELogVerbosity
{

enum Type : u8
{
    Unknown     = 0,

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

LAL_API Jafg::LString LexToString(const ELogVerbosity::Type Verbosity);

} /* ~Namespace Lal */
