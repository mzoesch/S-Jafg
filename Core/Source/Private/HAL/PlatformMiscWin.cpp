// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "HAL/PlatformMisc.h"

String WinPlatformMisc::GetEngineRootDir()
{
    TCHAR Buffer[PlatformMisc::GetMaxPathLength()];

    GetModuleFileName(nullptr, Buffer, PlatformMisc::GetMaxPathLength());
    const WideString::size_type Pos = WideString(Buffer).find_last_of(LITERAL_WIDE("\\/"));
    if (Pos != WideString::npos)
    {
        Buffer[Pos] = 0;
    }

    return PlatformTypes::Ws2S(WideString(Buffer).substr(0, Pos));
}
