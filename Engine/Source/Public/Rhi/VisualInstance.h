// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/UniformBufferObjects.h"
#include "Rhi/Bindless.h"

namespace Jafg
{

//# Device instance of a painted node.
struct alignas(16) LVisualInstance final
{
    //# Required
    LRect2F Rect;
    LVec4F TexCoordRect{ 0.0f, 0.0f, 1.0f, 1.0f };
    LVec4F Radii{ maths::zero_vector<LVec4F> };

    //# Required
    LColor Tint;
    //# Only required if sampling to border is enabled.
    LColor BorderTint{ SkipInit };
    //# Only required if #OutlineThickness > 0.
    LColor OutlineTint{ SkipInit };
    f32 OutlineThickness{};

    u32 TextureIndex{ UBO::Bindless::IdentityMulIdx };
    u32 SamplerIndex{ UBO::Bindless::LinearClampToEdgeSamplerIdx };
    f32 MsdfPixelRange{};
    u32 _pad1;
};
static_assert(sizeof(LVisualInstance) % 16 == 0);

namespace UBO
{

struct VisualShared final : public TUbo<VisualShared, 0, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F Proj;
    f32 Gamma;
};
static_assert(CUniformBufferObject<VisualShared>);

} /* ~Namespace UBO */

} /* ~Namespace Jafg */
