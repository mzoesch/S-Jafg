// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

namespace Jafg
{

#if JAFG_WITH_EDITOR
    static const uint WORLD_FLAG_SHOW_DIRECTIONAL_LIGHTS = 0x1 <<  0;
    static const uint WORLD_FLAG_SHOW_POINT_LIGHTS       = 0x1 <<  1;
    static const uint WORLD_FLAG_SHOW_SPOT_LIGHTS        = 0x1 <<  2;
    static const uint WORLD_FLAG_SHOW_RECT_LIGHTS        = 0x1 <<  3;
    static const uint WORLD_FLAG_SHOW_IBL                = 0x1 <<  4;
    static const uint WORLD_FLAG_SHOW_ALBEDO             = 0x1 <<  5;
    static const uint WORLD_FLAG_SHOW_NORMAL             = 0x1 <<  6;
    static const uint WORLD_FLAG_SHOW_OCCLUSION          = 0x1 <<  7;
    static const uint WORLD_FLAG_SHOW_ROUGHNESS          = 0x1 <<  8;
    static const uint WORLD_FLAG_SHOW_METALLIC           = 0x1 <<  9;
    static const uint WORLD_FLAG_SHOW_EMISSIVE           = 0x1 << 10;
    static const uint WORLD_FLAG_SHOW_SSAO               = 0x1 << 11;
#endif /* JAFG_WITH_EDITOR */

[[CxxName("Jafg::UBO::WorldData")]]
struct WorldData
{
    float4x4 view;
    float4x4 proj;

    /// rgb: The ambient light.
    /// a:   Exposure.
    float4 ambient;

    /// Gamma correction. We use sRGB only for now. So this is currently unused.
    float gamma;

    /// IBL parameters
    float    ibl_specular_max_lod;
    float    ibl_diffuse_intensity;
    float    ibl_specular_intensity;

    uint _pad0; // float ibl_rotation;
    /// Bindless index of the screen space ambient occlusion for this view or INVALID_INDEX.
    uint ssao_texture;

#if JAFG_WITH_EDITOR
    uint Flags;
#else /* JAFG_WITH_EDITOR */
    float _pad2;
#endif /* !JAFG_WITH_EDITOR */

    float cam_pos_x;
    float cam_pos_y;
    float cam_pos_z;

    float viewport_height;
    float vertical_fov;
};

} /* ~Namespace Jafg */
