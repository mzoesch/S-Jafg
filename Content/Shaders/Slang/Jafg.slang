// Copyright mzoesch. All rights reserved.

#pragma once

static const float PI = 3.14159265359;

#define JAFG_SAMPLER_REPEAT                                             0
#define JAFG_SAMPLER_MIRRORED_REPEAT                                    1
#define JAFG_SAMPLER_CLAMP_TO_EDGE                                      2
#define JAFG_SAMPLER_CLAMP_TO_BORDER                                    3
// #define JAFG_SAMPLER_MIRROR_CLAMP_TO_EDGE                            4
#define JAFG_SAMPLER_COUNT                                              4

float jafg_median_of_three(float a, float b, float c) 
{
    return max(min(a, b), min(max(a, b), c));
}
