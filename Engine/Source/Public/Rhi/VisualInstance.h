// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/UniformBufferObjects.h"

namespace Jafg
{

//# Device instance of a rendered visual node.
struct alignas(16) LVisualInstance final
{
    LVec4F Rect;
    LVec4F Tint;
    LVec4F BackgroundTint;
    LVec4F Radii;
    LVec4F OutlineTint;
    LVec4F TexCoordRect;
    f32 OutlineThickness;
    u32 TextureIndex;
    u32 SamplerIndex;
    f32 MsdfPixelRange;
};

namespace UBO
{

struct VisualShared final : public TUbo<VisualShared, 0, vk::ShaderStageFlagBits::eVertex>
{
    LMat4F Proj;
};
static_assert(CUniformBufferObject<VisualShared>);

} /* ~Namespace UBO */

} /* ~Namespace Jafg */
