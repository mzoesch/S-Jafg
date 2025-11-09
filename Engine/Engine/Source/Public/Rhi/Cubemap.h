// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/EnginePath.h"

namespace Jafg
{

class LShader;
class LEye;
class LViewport;

namespace ECubemap
{

enum Type
{
    Front  = 0,
    Back   = 1,
    Right  = 2,
    Left   = 3,
    Top    = 4,
    Bottom = 5,

    Size   = 6,
};

} /* ~Namespace ECubemap */

//#
//# A cubemap. Provide the paths to all six sides of each cube face in the order
//# as stated above in the #ECubemap enum.
//#
struct LCubemap final
{

    LCubemap() = default;
    /*ENGINE_API*/ explicit LCubemap(const TArray<LEnginePath>& InPaths) {}
    PROHIBIT_COPY(LCubemap)
    DEFAULT_MOVE(LCubemap)
    inline ~LCubemap() { this->Free(); }

    /*ENGINE_API*/ void Free() {}

    /*ENGINE_API*/ void Reload() {}
    /*ENGINE_API*/ void Cache(const TArray<LEnginePath>& InPaths) {}
    /*ENGINE_API*/ void Load(const TArray<LEnginePath>& InPaths) {}

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Handle.has_value(); }

    FORCEINLINE constexpr u32 GetHandle() const noexcept { return this->Handle.value(); }
    FORCEINLINE constexpr operator  u32() const noexcept { return this->GetHandle(); }

private:

    void LoadImpl();

    TArray<LEnginePath> Paths;
    TOptional<u32> Handle;
};

} /* ~Namespace Jafg */
