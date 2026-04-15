// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/UniformBufferObjects.h"

namespace Jafg
{

//# Device instance of a rendered visual node.
// struct alignas(16) LVisualInstance2 final
// {
//     LVec4F Rect;
//     LVec4F Tint;
//     LVec4F BackgroundTint;
//     LVec4F Radii;
//     LVec4F OutlineTint;
//     LVec4F TexCoordRect;
//     f32 OutlineThickness;
//     u32 TextureIndex;
//     u32 SamplerIndex;
//     f32 MsdfPixelRange;
// };

struct alignas(16) LVisualInstance final
{
    LRect2F Rect;

    u32 Tint;
    u32 BackgroundTint;
    u32 _pad1;
    u32 _pad2;

    LVec4F Radii;

    u32 OutlineTint;
    u32 _pad3;
    u32 _pad4;
    u32 _pad5;

    LVec4F TexCoordRect;

    f32 OutlineThickness;
    u32 TextureIndex;
    u32 SamplerIndex;
    f32 MsdfPixelRange;
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
