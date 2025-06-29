// Copyright mzoesch. All rights reserved.

#include "Engine/EngineCompileTimeConstants.h"

#if JAFG_WITH_FOREIGN_SUPPORT

#include "Foreign/Plugin.h"

namespace Jafg
{

LString LexToString(const EPluginLoadReturnCode::Type InValue)
{
    switch (InValue)
    {
    case EPluginLoadReturnCode::Success: { return "Success"; }
    case EPluginLoadReturnCode::AlreadyLoaded: { return "AlreadyLoaded"; }
    case EPluginLoadReturnCode::NotLoaded: { return "NotLoaded"; }
    case EPluginLoadReturnCode::Failure: { return "Failure"; }
    case EPluginLoadReturnCode::NotFound: { return "NotFound"; }
    case EPluginLoadReturnCode::NoBin: { return "NoBin"; }
    case EPluginLoadReturnCode::PlatformError: { return "PlatformError"; }
    case EPluginLoadReturnCode::NoLifetimeHandle: { return "NoLifetimeHandle"; }
    case EPluginLoadReturnCode::NoLifetime: { return "NoLifetime"; }
    }

    checkNoEntry()
    return { };
}

} /* ~Namespace Jafg */

#endif /* JAFG_WITH_FOREIGN_SUPPORT */
