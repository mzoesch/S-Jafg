// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"

#if JAFG_PLATFORM_WASM

#include "Platform/PlatformMisc.h"

Jafg::LString Jafg::PlatformMisc::GetEngineRootDirImpl()
{
    LOG_WARNING(LogSystem, "GetEngineRootDirImpl() is not implemented for the WASM platform.");
    return "";
}

Jafg::LString Jafg::PlatformMisc::GetRealEngineRootDirImpl()
{
    LOG_WARNING(LogSystem, "GetRealEngineRootDirImpl() is not implemented for the WASM platform.");
    return "";
}

i32 Jafg::PlatformMisc::GetNumberOfPhysicalViewports()
{
    LOG_WARNING(LogSystem, "GetNumberOfPhysicalViewports() is not implemented for the WASM platform.");
    return 0;
}

bool Jafg::PlatformMisc::SetPhysicalViewports()
{
    LOG_WARNING(LogSystem, "SetPhysicalViewports() is not implemented for the WASM platform.");
    return true;
}

#endif /* JAFG_PLATFORM_WASM */
