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

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{}.{}.{}", this->Major, this->Minor, this->Patch);
    }
};

//# @return Build time of the engine.
NOINLINE ENGINE_API const LString& GetBuildTime();

//# @return Build date of the engine.
NOINLINE ENGINE_API const LString& GetBuildDate() ;

//# @return Engine version when the engine library was built.
NOINLINE ENGINE_API LEngineVersion GetEngineVersion();

} /* ~Namespace Jafg::BuildInfo */
