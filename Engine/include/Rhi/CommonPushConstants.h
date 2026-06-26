// Copyright mzoesch. All rights reserved.

#pragma once

#include "Material.h"
#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"
#include "Components/ActorComponentForward.h"

namespace Jafg::PC
{

struct MVP final: rhi::pc_template<MVP, vk::ShaderStageFlagBits::eVertex,
    vk::ShaderStageFlagBits::eFragment /**/
    >
{
    LMat4F M;
    LMat4F V;
    LMat4F P;

    static void PushForActor(LActorRenderInfo const& Info, LMaterial const& Material, LPushConstantProvider::LActorInfo const& DrawInfo) noexcept
    {
        MVP{
            .M = maths::model(DrawInfo.Transform),
            .V = Info.WorldData.view,
            .P = Info.WorldData.proj,
            }.upload(Info.CommandBuffer, Material.Pipeline);
    }
};
static_assert(rhi::pc<MVP>);

} /* ~Namespace Jafg::PC */
