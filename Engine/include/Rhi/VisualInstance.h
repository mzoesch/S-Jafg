// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Objects.h"
#include "Rhi/Bindless.h"

namespace Jafg
{

namespace UBO
{

struct VisualShared final: rhi::ubo_template<VisualShared, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F Proj;
    f32 Gamma;
};
static_assert(rhi::ubo<VisualShared>);

} /* ~Namespace UBO */

namespace SSBO
{

//# Device instance of a painted node.
struct VisualInstance final: rhi::ssbo_template<VisualInstance, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    static constexpr auto default_count{ 16'384uz };

    //# Required
    LRect2F Rect;
    LVec4F TexCoordRect{ 0.0f, 0.0f, 1.0f, 1.0f };
    LVec4F Radii{ maths::zero_vector<LVec4F> };

    //# Required
    LColor Tint;
    //# Only required if sampling to border is enabled.
    LColor BorderTint{ ESkipInit::Here };
    //# Only required if #OutlineThickness > 0.
    LColor OutlineTint{ ESkipInit::Here };
    f32 OutlineThickness{};

    u32 TextureIndex{ UBO::Bindless::IdentityMulIdx };
    u32 SamplerIndex{ UBO::Bindless::LinearClampToEdgeSamplerIdx };
    f32 MsdfPixelRange{};
    bool bIgnoreAlpha{};
    u8 _pad0;
    u8 _pad1;
    u8 _pad2;
};
static_assert(sizeof(VisualInstance) % 16 == 0);
static_assert(rhi::ssbo<VisualInstance>);

} /* ~Namespace SSBO */

} /* ~Namespace Jafg */
