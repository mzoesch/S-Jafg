// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponentForward.h"
#include "Rhi/GraphicsPipeline.h"

namespace Jafg::PC
{

struct MVP final : public TPushConstant<MVP, vk::ShaderStageFlagBits::eVertex>
{
    LMat4F Model;
    LMat4F View;
    LMat4F Proj;

    static void AutoActorPush(LActorRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline, LActorDrawInfo const& DrawInfo) noexcept
    {
        MVP{
            .Model = maths::model(DrawInfo.Transform),
            .View = Info.WorldData.view,
            .Proj = Info.WorldData.proj,
        }.Push(Info, Pipeline);
    }
};

} /* ~Namespace Jafg::PC */
