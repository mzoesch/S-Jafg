// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/VkAl.h"
#include "Widgets/RegionForward.h"

// https://github.com/SaschaWillems/Vulkan/blob/master/examples/triangle/triangle.cpp

namespace Jafg
{

// struct LOrthoRegionShader
// {
//     struct LVertex
//     {
//         LVector3 Position;
//         Lal::LColor Color;
//     };
//
//     void Draw() const;
//
// private:
//
//     LMappedDeviceBuffer VertexBuffer;
// };

struct LOrthoRegionDrawInfo
{
    LViewport    const& Viewport;
    LVector2     const& TopLeft;
    LVector2     const& Size;
    LRegionBrush const& Brush;
};

struct LOrthoRegion
{
    // LDeviceRenderPass Pass; //???

    ENGINE_API void Draw(LOrthoRegionDrawInfo const& Info) const;
};

} /* ~Namespace Jafg */
