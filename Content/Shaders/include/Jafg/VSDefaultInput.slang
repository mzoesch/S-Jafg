// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::LStaticMesh::Vertex")]]
struct VSInput
{
    float3 pos: POSITION;
    float3 normal: NORMAL;
    float2 uv: TEXCOORD0;
    float4 tangent: TANGENT;
};
