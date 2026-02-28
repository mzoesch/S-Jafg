// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponentForward.h"
#include "Rhi/GraphicsPipeline.h"

namespace Jafg::PC
{

struct Pbr final : public TPushConstant<Pbr, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F Model;
    LVec4F baseColorFactor;
    f32 metallicFactor;
    f32 roughnessFactor;
    i32 baseColorTextureSet;
    i32 physicalDescriptorTextureSet;
    i32 normalTextureSet;
    i32 occlusionTextureSet;
    i32 emissiveTextureSet;
    f32 alphaMask;
    f32 alphaMaskCutoff;

    static void AutoActorPush(LActorRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline, LActorDrawInfo const& DrawInfo) noexcept
    {
        Pbr{
            .Model = maths::model(DrawInfo.Transform),
            .baseColorFactor = LVec4F{1.0f, 1.0f, 1.0f, 1.0f},
            .metallicFactor = 0,
            .roughnessFactor = 1,
            .baseColorTextureSet = 0,
            .physicalDescriptorTextureSet = 0,
            .normalTextureSet = 0,
            .occlusionTextureSet = 0,
            .emissiveTextureSet = 0,
            .alphaMask = 0,
            .alphaMaskCutoff = 0,
        }.Push(Info, Pipeline);
    }
};
static_assert(CPushConstant<Pbr>);

} /* ~Namespace Jafg::PC */
