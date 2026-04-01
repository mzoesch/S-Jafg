// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Framework/FrontendForward.h"
#include "Platform/SurfaceForward.h"
#include "Framework/Eye.h"
#include "Rhi/RendererCore.h"

namespace Jafg
{

class JUserPreferences;

//# Global graphics rendering info for one frame to be probably sentenced to fly on a device in the near future.
struct LRenderInfo
{
    JUserPreferences const& UserPreferences;

    //# The frontend to submit to.
    LFrontend const& Frontend;
    //# The surface to submit to.
    LSurface const& Surface;

    //# The command buffer to submit graphics stuff to.
    vk::CommandBuffer CommandBuffer;
    //# The pool to allocate descriptors from for this frame only.
    vk::DescriptorPool DescriptorPool;

    //# The frame in question.
    u32 Frame;
    //# The image in question.
    u32 Image;
};

} /* ~Namespace Jafg */
