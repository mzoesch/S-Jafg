// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Objects.h"

namespace Jafg::UBO
{

struct WorldData final : rhi::ubo_template<WorldData, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    // LMat4F model;                     // Model-to-world transformation matrix
    LMat4F view;                      // World-to-camera transformation matrix
    LMat4F proj;                      // Camera-to-clip space projection matrix

    std::array<LVec4F,4> lightPositions;           // Light positions in world space
    std::array<LVec4F,4> lightColors;              // Light intensities and colors

    f32 exposure;                     // HDR exposure control
    f32 gamma;                        // Gamma correction value (typically 2.2)
    f32 prefilteredCubeMipLevels;     // IBL prefiltered environment map mip levels
    f32 scaleIBLAmbient;              // IBL ambient contribution scale

    f32 _pad0;
    f32 _pad1;
    f32 _pad2;

    LVec3F CameraPosition;

    f32 ViewportHeight;
    f32 VerticalFov;
};
static_assert(rhi::ubo<WorldData>);

} /* ~Namespace Jafg::UBO */
