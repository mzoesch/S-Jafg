// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Platform/SurfaceForward.h"
#include "Framework/FrontendForward.h"
#include "Rhi/VkForward.h"
#include "Rhi/RhiVendorInclude.h"
#include "Rhi/VkAl.h"
#include "Rhi/Texture2.h"
#include "Rhi/Image.h"

namespace Jafg
{

struct LRenderInfo;

namespace UBO
{

//# UBO for perspective camera draw calls. Usually bound to (0, 0).
struct LPerspectiveCamera
{
    glm::mat4 view;
    glm::mat4 proj;

    inline void Upload(LMappedDeviceBuffer const& Buffer) const noexcept
    {
        check( Buffer.GetData() != nullptr )
        std::memcpy(Buffer.GetData(), this, sizeof(LPerspectiveCamera));
    }

    ENGINE_API void Bind(LRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline) const noexcept;

    static vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eVertex; }
};

} /* ~Namespace UBO */

//# Global rendering info for one render loop.
struct LRenderInfo
{
    LFrontend const& Frontend;
    LSurface const& Surface;

    vk::CommandBuffer CommandBuffer;
    vk::DescriptorPool DescriptorPool;

    u32 Frame;
    u32 Image;

    UBO::LPerspectiveCamera PerspectiveCamera;
    vk::Buffer PerspectiveCameraBuffer;
};

} /* ~Namespace Jafg */

static_assert(Jafg::CUniformBufferObject<Jafg::UBO::LPerspectiveCamera>);
