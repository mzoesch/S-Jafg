// Copyright mzoesch. All rights reserved.

#include "Lal.afx"

#if LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES && PLATFORM_WINDOWS

#include "Foreign/Plugin.h"

namespace Jafg
{

EPluginLoadReturnCode::Type LLoadedPlugin::OpenLibrary()
{
    check( this->IsValid() )
    checkNoEntry()
    return EPluginLoadReturnCode::Success;
}

EPluginLoadReturnCode::Type LLoadedPlugin::CloseLibrary()
{
    check( this->IsValid() )
    checkNoEntry()
    return EPluginLoadReturnCode::Success;
}

} /* ~Namespace Jafg */

#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES && PLATFORM_WINDOWS */
