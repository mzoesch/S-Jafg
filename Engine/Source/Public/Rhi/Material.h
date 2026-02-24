// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LMaterial final
{
    struct TextureView
    {
        LString Identifier;
    };

    LPath Path;
    LPath Shader;
    TArray<TextureView> TextureViews;
};

} /* ~Namespace Jafg */
