// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Texture2.h"

namespace Jafg
{

//#
//# An image is a brush component that wraps a loaded two-dimensional texture.
//# An image does not own the texture it wraps - therefore, many images may wrap the same texture to save a lot
//# on memory usage.
//# An image allows for further customization of the texture during the pipeline process such as tinting, scaling,
//# translating, rotating, etc.
//#
class LImage final
{
public:

    inline LImage() = default;
    DEFAULT_REALLOC_OF_ANY_FORM(LImage)
    inline ~LImage() = default;

    FORCEINLINE bool    IsTextureValid() const noexcept { return this->TextureRef != nullptr; }
    FORCEINLINE LImage& SetTexture(const LTexture2* InTexture) noexcept { this->TextureRef = InTexture; return *this; }
    FORCEINLINE const LTexture2* GetTexture() const noexcept { check( this->TextureRef ) return this->TextureRef; }

    FORCEINLINE bool  IsHandleValid() const noexcept { return this->TextureRef && this->TextureRef->IsUploaded(); }
    FORCEINLINE const LTexture2Handle& GetTextureHandle() const noexcept { check( this->TextureRef ) return this->TextureRef->GetHandle(); }

private:

    const LTexture2* TextureRef { nullptr };
};

} /* ~Namespace Jafg */
