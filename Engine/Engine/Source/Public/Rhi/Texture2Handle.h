// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LTexture2;

//#
//# Represents a handle to a texture on the graphics processing unit.
//#
class LTexture2Handle final
{
public:

    FORCEINLINE LTexture2Handle() noexcept = default;

    //#
    //# Will call #Upload.
    //#
    ENGINE_API explicit LTexture2Handle(const LTexture2& InTexture);

    PROHIBIT_COPY(LTexture2Handle)
    DEFAULT_MOVE(LTexture2Handle)

    //#
    //# Will shred the underlying texture if it is loaded.
    //#
    FORCEINLINE ~LTexture2Handle() { this->Shred(); }

    //#
    //# Uploads the texture to the graphics processing unit.
    //#
    ENGINE_API void Upload(const LTexture2& InTexture);

    //#
    //# Shreds the texture from the graphics processing unit.
    //#
    ENGINE_API void Shred();

    FORCEINLINE bool IsValid() const noexcept { return this->Handle.IsSet(); }
    FORCEINLINE u32  GetHandle() const noexcept { return this->Handle.GetValue(); }

    FORCEINLINE operator bool() const noexcept { return this->IsValid(); }
    FORCEINLINE operator  u32() const noexcept { return this->GetHandle(); }

private:

    TOptional<u32> Handle;
};

} /* ~Namespace Jafg */
