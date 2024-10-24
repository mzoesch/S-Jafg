// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace ELogVerbosity
{

enum Type : uint8
{
    Trace       = 1,
    Debug       = 2,
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

} /* ~Namespace Jafg */
