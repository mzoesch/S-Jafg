// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::SSBO::Ray")]]
struct Ray
{
    /* Expects w to be 1.0. */
    float4 start;
    /* Actually a float3+LColor. */
    float4 end; 
};
