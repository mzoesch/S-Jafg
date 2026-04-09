// Copyright mzoesch. All rights reserved.

#pragma once

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

//# Equivalent to the LColor in C++. @see Maths/Color.h
typedef uint32_t LColor;

float3 jafg_srgb_to_linear(float3 srgb)
{
    return pow(srgb, 2.2);
}

float4 jafg_color_to_srgb(LColor color)
{
    return float4(
        ((color >>  0) & 0xFF) / 255.0,
        ((color >>  8) & 0xFF) / 255.0,
        ((color >> 16) & 0xFF) / 255.0,
        ((color >> 24) & 0xFF) / 255.0
        );
}

float4 jafg_color_to_linear(LColor color)
{
    float4 srgb = jafg_color_to_srgb(color);
    return float4(
        jafg_srgb_to_linear(srgb.rgb),
        srgb.a 
        );
}

float jafg_median_of_three(float a, float b, float c) 
{
    return max(min(a, b), min(max(a, b), c));
}
