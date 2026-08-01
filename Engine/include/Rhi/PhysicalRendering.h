// Copyright mzoesch. All rights reserved.

#pragma once

#include "Material.h"
#include "Framework/ActorComponentForward.h"
#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg::PC
{

struct Pbr final: rhi::pc_template<Pbr, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
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

    static void PushForActor(LActorRenderInfo const& Info, LMaterial const& Material, LPushConstantProvider::LActorInfo const& ActorInfo) noexcept
    {
        Pbr{
            .Model = maths::model(ActorInfo.Transform),
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
        }.upload(Info.CommandBuffer, Material.Pipeline);
    }
};
static_assert(rhi::pc<Pbr>);

} /* ~Namespace Jafg::PC */
