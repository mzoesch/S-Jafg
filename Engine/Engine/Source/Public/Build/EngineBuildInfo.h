// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

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
        return Lal::SprintF("{}.{}.{}", this->Major, this->Minor, this->Patch);
    }
};

NOINLINE ENGINE_API LString const& GetBuildTime();
NOINLINE ENGINE_API LString const& GetBuildDate();
NOINLINE ENGINE_API LString const& GetVcsBranch();
NOINLINE ENGINE_API LString const& GetVcsRevision();

//# @return The engine version when the engine library was build.
NOINLINE ENGINE_API const LString& GetEngineVersionStr();
NOINLINE ENGINE_API LEngineVersion GetEngineVersion();

NOINLINE ENGINE_API LString const& GetCompilerVersion();
NOINLINE ENGINE_API LString const& GetCxxStandard();

} /* ~Namespace Jafg::BuildInfo */
