// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Framework/FrontendForward.h"
#include "Platform/SurfaceForward.h"
#include "Rhi/RendererCore.h"

namespace Jafg
{

class JUserPreferences;
class JTextureSubsystem;
class JMaterialSubsystem;
class JShaderSubsystem;
class JMeshSubsystem;
class JFontSubsystem;

//# Global graphics rendering info for one frame to be probably sentenced to fly on a device in the near future.
struct LRenderInfo
{
    JUserPreferences const& UserPreferences;

    //# The frontend to submit to.
    LFrontend& Frontend;
    //# The surface to submit to.
    LSurface& Surface;

    JTextureSubsystem& TextureSubsystem;
    JMaterialSubsystem& MaterialSubsystem;
    JShaderSubsystem& ShaderSubsystem;
    JMeshSubsystem& MeshSubsystem;
    JFontSubsystem& FontSubsystem;

    //# The command buffer to submit graphics stuff to.
    vk::CommandBuffer CommandBuffer;
    //# The pool to allocate descriptors from for this frame only.
    vk::DescriptorPool DescriptorPool;

    //# The frame in question.
    u32 Frame;
    //# The image in question.
    u32 Image;

    vk::Viewport VkViewport;
    vk::Rect2D VkScissor;
};

} /* ~Namespace Jafg */
