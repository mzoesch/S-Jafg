// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::BuildInfo
{

struct LEngineVersion
{
    u8 Major = 0;
    u8 Minor = 0;
    u8 Patch = 0;

    FORCEINLINE auto ToString() const -> LSimpleString
    {
        return LSimpleString::SprintF("{}.{}.{}", this->Major, this->Minor, this->Patch);
    }
};

//# @return Build time of the engine.
NOINLINE ENGINE_API auto GetBuildTime() -> const LSimpleString&;

//# @return Build date of the engine.
NOINLINE ENGINE_API auto GetBuildDate() -> const LSimpleString&;

//# @return Engine version when the engine library was built.
NOINLINE ENGINE_API auto GetEngineVersion() -> LEngineVersion;

} /* ~Namespace Jafg::BuildInfo */
