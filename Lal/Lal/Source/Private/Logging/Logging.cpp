// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"


namespace Jafg::Private
{

LSimpleString LalLogPrivateColor_Trace   = LOG_COLOR_TRACE;
LSimpleString LalLogPrivateColor_Verbose = LOG_COLOR_VERBOSE;
LSimpleString LalLogPrivateColor_Info    = LOG_COLOR_INFO;
LSimpleString LalLogPrivateColor_Warning = LOG_COLOR_WARNING;
LSimpleString LalLogPrivateColor_Error   = LOG_COLOR_ERROR;
LSimpleString LalLogPrivateColor_Fatal   = LOG_COLOR_FATAL;
LSimpleString LalLogPrivateColor_End     = LOG_COLOR_END;

LStringLegacy GetColorForVerbosity(const ELogVerbosityType Verbosity)
{
    if (Verbosity == ELogVerbosity::Trace)   { return LalLogPrivateColor_Trace.ToC(); }
    if (Verbosity == ELogVerbosity::Verbose) { return LalLogPrivateColor_Verbose.ToC(); }
    if (Verbosity == ELogVerbosity::Info)    { return LalLogPrivateColor_Info.ToC(); }
    if (Verbosity == ELogVerbosity::Warning) { return LalLogPrivateColor_Warning.ToC(); }
    if (Verbosity == ELogVerbosity::Error)   { return LalLogPrivateColor_Error.ToC(); }
    if (Verbosity == ELogVerbosity::Fatal)   { return LalLogPrivateColor_Fatal.ToC(); }

    return "";
}

} /* ~Namespace Jafg::Private */

namespace Jafg
{

LSimpleString LexToString(const ELogVerbosityType Verbosity)
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
    default: { checkNoEntry() return "Unknown"; }
    }
}

} /* ~Namespace Jafg */
