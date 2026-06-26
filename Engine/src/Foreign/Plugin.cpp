// Copyright mzoesch. All rights reserved.

#include "Engine/EngineCompileTimeConstants.h"

#if JAFG_WITH_FOREIGN_SUPPORT

#include "Foreign/Plugin.h"
#include "Async/TaskUtility.h"

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
    std::unreachable();
}

LLoadedPlugin::~LLoadedPlugin()
{
    if (this->IsLoaded())
    {
        //#
        //# If this ever triggers - we are fucked. The plugin probably has handles all over the place.
        //#
        LOG_WARNING(LogForeign, "Plugin [{}] was not closed before destruction.", this->GetIdentifier())
        this->CloseLibrary(EPluginShutdownReason::Unspecified);
    }

    return;
}

void LLoadedPlugin::PrepareLibraryClose(const EPluginShutdownReason::Type InReason)
{
    check( Tasks::IsOnMasterThread() )

    if (this->IsLoaded() == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not loaded.", this->GetAbsolutePath())
        return;
    }

    if (this->Fetched.bDynUnloadable == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not marked as unloadable.", this->GetAbsolutePath())
        return;
    }

    if (this->Lifetime.get() != nullptr)
    {
        this->Lifetime->OnPrepareShutdown(InReason);
    }
    else
    {
        LOG_ERROR(LogForeign, "Lifetime [{}] is invalid.", this->GetAbsolutePath())
    }

    return;
}

} /* ~Namespace Jafg */

#endif /* JAFG_WITH_FOREIGN_SUPPORT */
