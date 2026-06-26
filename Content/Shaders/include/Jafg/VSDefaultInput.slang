// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"

[[CxxName("Jafg::LStaticMesh::Vertex")]]
struct VSInput
{
    float3 Position: POSITION;
    float3 Normal: NORMAL;
    float2 UV: TEXCOORD0;
    float4 Tangent: TANGENT;
};
