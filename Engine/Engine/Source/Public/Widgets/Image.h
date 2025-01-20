// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
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

    LImage() = default;
    DEFAULT_REALLOC_OF_ANY_FORM(LImage)
    ~LImage() = default;

    FORCEINLINE auto SetTexture(const LTexture2* InTexture) -> LImage& { this->TextureRef = InTexture; return *this; }
    FORCEINLINE bool HasTexture() const { return this->TextureRef; }
    FORCEINLINE auto GetTexture() const -> const LTexture2* { return this->TextureRef; }

private:

    const LTexture2* TextureRef = nullptr;
};

} /* ~Namespace Jafg */
