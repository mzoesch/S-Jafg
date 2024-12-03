// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if PLATFORM_WASM

#include "Platform/PlatformMisc.h"

Jafg::LSimpleString Jafg::PlatformMisc::GetEngineRootDirImpl()
{
    LOG_WARNING(LogSystem, "GetEngineRootDirImpl() is not implemented for the WASM platform.");
    return "";
}

Jafg::LSimpleString Jafg::PlatformMisc::GetRealEngineRootDirImpl()
{
    LOG_WARNING(LogSystem, "GetRealEngineRootDirImpl() is not implemented for the WASM platform.");
    return "";
}

int32 Jafg::PlatformMisc::GetNumberOfPhysicalViewports()
{
    LOG_WARNING(LogSystem, "GetNumberOfPhysicalViewports() is not implemented for the WASM platform.");
    return 0;
}

bool Jafg::PlatformMisc::SetPhysicalViewports()
{
    LOG_WARNING(LogSystem, "SetPhysicalViewports() is not implemented for the WASM platform.");
    return true;
}

#endif /* PLATFORM_WASM */
