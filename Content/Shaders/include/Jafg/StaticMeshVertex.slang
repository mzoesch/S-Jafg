// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

namespace Jafg
{

[[CxxName("Jafg::LStaticMesh::Vertex")]]
struct StaticMeshVertex
{
    float3 pos: POSITION;
    float3 normal: NORMAL;
    float2 uv: TEXCOORD0;
    float4 tangent: TANGENT;
};

} /* ~Namespace Jafg */
