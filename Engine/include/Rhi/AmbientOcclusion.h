// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg
{

struct LAmbientOcclusionSettings final
{
    //# World space radius in meters in which geometry occludes.
    f32 Radius{ 0.5f };
    //#
    //# Occluders have to rise above the surface by this sine of an angle. Raise it if flat or low poly surfaces
    //# occlude themselves.
    //#
    f32 Bias{ 0.1f };
    //# How dark the occlusion gets.
    f32 Intensity{ 1.5f };
    //# Contrast of the occlusion.
    f32 Power{ 1.5f };
    //# Relative depth difference at which the blur does not cross an edge anymore.
    f32 BlurDepthThreshold{ 0.05f };
};

namespace PC
{

//# @see Content/Shaders/src/Ssao.slang
struct Ssao final: rhi::pc_template<Ssao, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LVec2F ViewportExtent;
    LVec2F TanHalfFov;
    f32 Radius;
    f32 Bias;
    f32 Intensity;
    f32 Power;
};
static_assert(rhi::pc<Ssao>);
static_assert(sizeof(Ssao) == 32, "Is the shader also modified?");

//# @see Content/Shaders/src/SsaoBlur.slang
struct SsaoBlur final: rhi::pc_template<SsaoBlur, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LVec2F ViewportExtent;
    f32 DepthThreshold;
    f32 _pad0;
};
static_assert(rhi::pc<SsaoBlur>);
static_assert(sizeof(SsaoBlur) == 16, "Is the shader also modified?");

} /* ~Namespace PC */

} /* ~Namespace Jafg */
