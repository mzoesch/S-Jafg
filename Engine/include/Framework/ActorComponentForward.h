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
    LWorldEye const& PerspectiveEye;

    //# RO buffers you should try to use. Do not modify.
    algo::transparent_unordered_string_map<vk::DescriptorSet> const& Sets;
    algo::transparent_unordered_string_map<rhi::mapped_device_buffer*> const& Buffers;

    //#
    //# The default perspective polygon mode. If rendering does not require a specific polygon mode,
    //# this one should be used.
    //# When it is your turn to draw. The rendering pipeline will be in this state. So do not set it explicitly.
    //# If your draw call requires something else; change it and then reset the command buffer to this polygon
    //# mode after your buffer submission.
    //#
    vk::PolygonMode DefaultPerspectivePolygonMode;

    LMaterialInstance* PreferredMaterial{};

    UBO::WorldData WorldData;

    //# Delegates called after all employed renderables have been rendered.
    //# You may even add delegates here while already being in a post render delegate.
    mutable TArray<TFunction2<void(LActorRenderInfo const& Info)>> PostRenderDelegates;
};

} /* ~Namespace Jafg */
