// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::UBO::WorldData")]]
// Uniform buffer - Global data shared across all vertices/fragments
struct WorldData
{
    float4x4 view;
    float4x4 proj;
    float4 lightPositions[4];           // Light positions in world space
    float4 lightColors[4];              // Light intensities and colors

    float exposure;                     // HDR exposure control
    float gamma;                        // Gamma correction value (typically 2.2)
    float prefilteredCubeMipLevels;     // IBL prefiltered environment map mip levels
    float scaleIBLAmbient;              // IBL ambient contribution scale

    float _pad0;
    float _pad1;
    float _pad2;

    float cam_pos_x;
    float cam_pos_y;
    float cam_pos_z;

    float viewport_height;
    float vertical_fov;
};
