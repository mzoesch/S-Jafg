// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::BuildInfo
{

struct LEngineVersion final
{
    u8 Major { 0 };
    u8 Minor { 0 };
    u8 Patch { 0 };

    FORCEINLINE constexpr std::strong_ordering operator<=>(const LEngineVersion& Other) const = default;

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{}.{}.{}", this->Major, this->Minor, this->Patch);
    }
};

//# @return Build time of the engine.
NOINLINE ENGINE_API const LString& GetBuildTime();

//# @return Build date of the engine.
NOINLINE ENGINE_API const LString& GetBuildDate() ;

NOINLINE ENGINE_API const LString& GetVcsBranch() ;
NOINLINE ENGINE_API const LString& GetVcsRevision() ;

//# @return The engine version when the engine library was build.
NOINLINE ENGINE_API LString        GetEngineVersionStr();
NOINLINE ENGINE_API LEngineVersion GetEngineVersion();

} /* ~Namespace Jafg::BuildInfo */
