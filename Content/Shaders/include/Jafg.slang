// Copyright mzoesch. All rights reserved.

///
/// Optional Jafg header that defines utility functions/types that work directly with the CXX Jafg engine.
///
#pragma once

#include "Core.slang"

static const float PI = 3.14159265359;

#define JAFG_SAMPLER_LINEAR_REPEAT                                      0
#define JAFG_SAMPLER_LINEAR_MIRRORED_REPEAT                             1
#define JAFG_SAMPLER_LINEAR_CLAMP_TO_EDGE                               2
#define JAFG_SAMPLER_LINEAR_CLAMP_TO_BORDER                             3
// #define JAFG_SAMPLER_LINEAR_MIRROR_CLAMP_TO_EDGE
#define JAFG_SAMPLER_NEAREST_REPEAT                                     4
#define JAFG_SAMPLER_NEAREST_MIRRORED_REPEAT                            5
#define JAFG_SAMPLER_NEAREST_CLAMP_TO_EDGE                              6
#define JAFG_SAMPLER_NEAREST_CLAMP_TO_BORDER                            7
// #define JAFG_SAMPLER_NEAREST_MIRROR_CLAMP_TO_EDGE
#define JAFG_SAMPLER_COUNT                                              8

/// Equivalent to the LColor in C++. @see Maths/Color.h
typedef uint32_t LColor;

namespace maths
{

float3 srgb_to_linear(float3 srgb, float gamma)
{
    float3 low  = srgb / 12.92;
    float3 high = pow((srgb + 0.055) / 1.055, gamma);
    return select((srgb <= 0.04045), low, high);
}

float3 srgb_to_linear_fast(float3 srgb, float gamma)
{
    return pow(srgb, gamma);
}

float4 color_to_srgb(LColor color)
{
    return float4(
        ((color >>  0) & 0xFF) / 255.0,
        ((color >>  8) & 0xFF) / 255.0,
        ((color >> 16) & 0xFF) / 255.0,
        ((color >> 24) & 0xFF) / 255.0
        );
}

float4 color_to_linear(LColor color, float gamma)
{
    float4 srgb = color_to_srgb(color);
    return float4(
        srgb_to_linear(srgb.rgb, gamma),
        srgb.a 
        );
}

float4 color_to_linear_fast(LColor color, float gamma)
{
    float4 srgb = color_to_srgb(color);
    return float4(
        srgb_to_linear_fast(srgb.rgb, gamma),
        srgb.a
        );
}

float median_of_three(float a, float b, float c) 
{
    return max(min(a, b), min(max(a, b), c));
}

} /* ~Namespace maths */
