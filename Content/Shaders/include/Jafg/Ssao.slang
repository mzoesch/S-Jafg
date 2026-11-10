// Copyright mzoesch. All rights reserved.

///
/// Shared between the passes of the screen space ambient occlusion.
///
#pragma once

#include "Jafg.slang"

namespace Jafg
{

/// The depth prepass is cleared to a depth beyond this. Such pixels show the background and are never occluded.
static const float SSAO_BACKGROUND_DEPTH = 1e20;

struct SsaoFRInput
{
    float4 pos: SV_POSITION;
};

/// A single triangle that covers the whole viewport.
SsaoFRInput ssao_fullscreen_triangle(uint vertex_id)
{
    float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0,  3.0),
        float2( 3.0, -1.0)
    };

    SsaoFRInput result;
    result.pos = float4(positions[vertex_id], 0.0, 1.0);
    return result;
}

/// Loads a texel with the coordinates clamped to the viewport.
float ssao_load(Texture2D<float> texture, int2 pixel, float2 viewport_size)
{
    return texture.Load(int3(clamp(pixel, int2(0, 0), int2(viewport_size) - 1), 0));
}

} /* ~Namespace Jafg */
