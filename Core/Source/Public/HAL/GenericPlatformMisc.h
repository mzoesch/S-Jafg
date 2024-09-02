// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

struct GenericPlatformMisc
{

    static consteval auto GetMaxPathLength() -> int32 { return PLATFORM_MAX_PATH; }
    static           auto GetEngineRootDir() -> String { return { }; }

};
