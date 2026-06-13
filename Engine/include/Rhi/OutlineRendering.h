// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RenderInfo.h"
#include "Rhi/PushConstants.h"

namespace Jafg::PC
{

struct Outline final : public TPushConstant<Outline, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LVec2F InverseViewportExtent;
    u32 Thickness;
    LColor Tint;
};
static_assert(CPushConstant<Outline>);

} /* ~Namespace Jafg::PC */
