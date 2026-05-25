// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/UniformBufferObjects.h"

namespace Jafg
{

namespace UBO
{

struct WorldData final : public TUbo<WorldData, 0, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    // LMat4F model;                     // Model-to-world transformation matrix
    LMat4F view;                      // World-to-camera transformation matrix
    LMat4F proj;                      // Camera-to-clip space projection matrix
    LVec4F lightPositions[4];           // Light positions in world space
    LVec4F lightColors[4];              // Light intensities and colors
    LVec4F camPos;                      // Camera position for view-dependent effects
    float exposure;                     // HDR exposure control
    float gamma;                        // Gamma correction value (typically 2.2)
    float prefilteredCubeMipLevels;     // IBL prefiltered environment map mip levels
    float scaleIBLAmbient;              // IBL ambient contribution scale
};
static_assert(CUniformBufferObject<WorldData>);

} /* ~Namespace UBO */

} /* ~Namespace Jafg */
