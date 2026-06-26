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
    LVec4F camPos;                      // Camera position for view-dependent effects
    float exposure;                     // HDR exposure control
    float gamma;                        // Gamma correction value (typically 2.2)
    float prefilteredCubeMipLevels;     // IBL prefiltered environment map mip levels
    float scaleIBLAmbient;              // IBL ambient contribution scale
};
static_assert(rhi::ubo<WorldData>);

} /* ~Namespace Jafg::UBO */
