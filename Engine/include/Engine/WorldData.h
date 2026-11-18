// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Objects.h"
#include "Framework/EditorForward.h"

namespace Jafg::UBO
{

//# @see Content/Shaders/include/Jafg/WorldData.slang
struct WorldData final : rhi::ubo_template<WorldData, vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment>
{
    LMat4F View;
    LMat4F Proj;

    LLinearColor3 Ambient{ ESkipInit::Here };
    f32 Exposure;

    f32 Gamma;

    f32 IblSpecularMaxLod;
    f32 IblDiffuseIntensity;
    f32 IblSpecularIntensity;

    f32 _pad0;
    //# Bindless index of the screen space ambient occlusion for the draw or #rhi::invalid_resource_index.
    rhi::resource_index_t AmbientOcclusionTexture;

#if JAFG_WITH_EDITOR
    EWorldRenderFlags Flags;
#else /* JAFG_WITH_EDITOR */
    f32 _pad2;
#endif /* !JAFG_WITH_EDITOR */

    LVec3F CameraPosition;

    f32 ViewportHeight;
    f32 VerticalFov;
};
static_assert(rhi::ubo<WorldData>);
static_assert(sizeof(WorldData) % 16 == 0);
static_assert(sizeof(WorldData) == 192);

} /* ~Namespace Jafg::UBO */
