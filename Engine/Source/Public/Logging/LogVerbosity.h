// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
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
static inline LString LexToString(ELogVerbosity::Type Verbosity) noexcept
{
    switch (Verbosity)
    {
    case ELogVerbosity::Trace: { return "Trace"; }
    case ELogVerbosity::Verbose: { return "Verbose"; }
    case ELogVerbosity::Info: { return "Info"; }
    case ELogVerbosity::Warning: { return "Warning"; }
    case ELogVerbosity::Error: { return "Error"; }
    case ELogVerbosity::Fatal: { return "Fatal"; }
    case ELogVerbosity::KillLog: { return "KillLog"; }
    default: { return "Unknown"; }
    }
}

} /* ~Namespace Jafg */
