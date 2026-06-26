// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/RenderInfo.h"
#include "Engine/WorldData.h"

namespace Jafg
{

struct LActorRenderInfo: LRenderInfo
{
    //# The perspective eye.
    LEye_v2 const& PerspectiveEye;

    //#
    //# The default perspective polygon mode. If rendering does not require a specific polygon mode,
    //# this one should be used.
    //# If your draw call requires something else. Then reset the command buffer to this polygon mode after your
    //# buffer submission.
    //#
    vk::PolygonMode DefaultPerspectivePolygonMode;

    LMaterialInstance* PreferredMaterial;

    // TODO:
    //  Remove world data from this render ino.
    //  Each world should have their own RenderInfo. Where this lives.
    //  Also the frontend should not own the world data buffer and descriptor set.
    //  This should also be per world basis. The layout is maybe ok in the frontend?
    UBO::WorldData WorldData;
    vk::DescriptorSet WorldDataDescriptorSet;
};

} /* ~Namespace Jafg */
