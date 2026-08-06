// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

namespace Jafg
{

[[CxxName("Jafg::PC::Pbr")]]
struct PbrMaterial 
{
    float4x4 model;
    float4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    int baseColorTextureSet;
    int physicalDescriptorTextureSet;
    int normalTextureSet;
    int occlusionTextureSet;
    int emissiveTextureSet;
    float alphaMask;
    float alphaMaskCutoff;
};

} /* ~Namespace Jafg */
