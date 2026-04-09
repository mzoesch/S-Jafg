// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LGlyphInfo final
{
    LVec4F Rect;
    LVec4F TexCoordRect;
    u32 BindlessTextureIndex;
    u32 SamplerIndex;
    f32 MsdfPixelRange;
};

struct LGetGlyphInfosResult
{
    TArray<LGlyphInfo> GlyphInfos;
    f32 Ascender;
    f32 Descender;
    f32 LineHeight;
};

} /* ~Namespace Jafg */
