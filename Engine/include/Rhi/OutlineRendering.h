// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg::PC
{

struct Outline final: rhi::pc_template<Outline, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LVec2F InverseViewportExtent;
    u32 Thickness;
    LColor Tint;
};
static_assert(rhi::pc<Outline>);

} /* ~Namespace Jafg::PC */
