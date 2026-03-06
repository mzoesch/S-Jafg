// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/UniformBufferObjects.h"

namespace Jafg
{

struct alignas(16) LVisualInstance final
{
    LVec4F Rect;
    LVec4F Tint;
    LVec4F Radii;
    LVec4F OutlineTint;
    LVec4F TextureTint;
    LVec4F TexCoordRect;
    f32 OutlineThickness;
};

namespace UBO
{

//# Device instance of a rendered visual node.
struct VisualShared final : public TUbo<VisualShared, 0, vk::ShaderStageFlagBits::eVertex>
{
    LMat4F Proj;
};
static_assert(CUniformBufferObject<VisualShared>);

} /* ~Namespace UBO */

} /* ~Namespace Jafg */
