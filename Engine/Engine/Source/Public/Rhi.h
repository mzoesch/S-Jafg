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
#include "Framework/Eye.h"

namespace Jafg
{

namespace UBO
{

//# UBO for perspective camera draw calls. Usually bound to (0, 0).
struct LPerspectiveCamera
{
    LMatrix4F view;
    LMatrix4F proj;

    inline void Upload(LMappedDeviceBuffer const& Buffer) const noexcept
    {
        check( Buffer.GetData() != nullptr )
        std::memcpy(Buffer.GetData(), this, sizeof(LPerspectiveCamera));
    }

    static vk::DescriptorBufferInfo GetWriteInfo(vk::Buffer Buffer) noexcept
    {
        return {
            .buffer = Buffer,
            .offset = 0,
            .range = sizeof(UBO::LPerspectiveCamera)
            };
    }

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

    TOptional<LEye_v2> Eye;

    UBO::LPerspectiveCamera PerspectiveCamera;
    vk::DescriptorBufferInfo PerspectiveCameraWriteInfo;
};

template<typename T>
inline void TPushConstant<T>::Push(LRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline, u32 Offset /* = 0 */) noexcept
{
    Info.CommandBuffer.pushConstants2({
        .layout = *Pipeline.Layout,
        .stageFlags = T::Flags(),
        .offset = Offset,
        .size = sizeof(T),
        .pValues = this
        });

    return;
}

} /* ~Namespace Jafg */

static_assert(Jafg::CUniformBufferObject<Jafg::UBO::LPerspectiveCamera>);
