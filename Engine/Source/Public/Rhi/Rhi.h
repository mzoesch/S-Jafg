// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Platform/SurfaceForward.h"
#include "Framework/FrontendForward.h"
#include "Rhi/VkForward.h"
#include "Rhi/RhiVendorInclude.h"
#include "Rhi/VkAl.h"
#include "Rhi/Texture2.h"
#include "Framework/Eye.h"

namespace Jafg
{

namespace UBO
{

//# UBO for perspective camera draw calls. Usually bound to [0, 0].
struct LPerspectiveCamera
{
    LMat4F view;
    LMat4F proj;

    inline void Upload(LMappedDeviceBuffer const& Buffer) const noexcept
    {
        check( Buffer.GetData() != nullptr )
        std::memcpy(Buffer.GetData(), this, sizeof(LPerspectiveCamera));
    }

    inline static constexpr vk::DescriptorBufferInfo GetWriteInfo(vk::Buffer Buffer) noexcept
    {
        return {
            .buffer = Buffer,
            .offset = 0,
            .range = sizeof(UBO::LPerspectiveCamera)
            };
    }

    inline static constexpr vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eVertex; }
};

} /* ~Namespace UBO */

//# Global graphics rendering info for one frame to be probably sentenced to fly on a device in the near future.
struct LRenderInfo
{
    //# The frontend to submit to.
    LFrontend const& Frontend;
    //# The surface to submit to.
    LSurface const& Surface;

    //# The command buffer to submit graphics stuff to.
    vk::CommandBuffer CommandBuffer;
    //# The pool to allocate descriptors from.
    vk::DescriptorPool DescriptorPool;

    //# The frame in question.
    u32 Frame;
    //# The image in question.
    u32 Image;

    //# The perspective eye. An absence of a value means to not submit perspective draw calls.
    TOptional<LEye_v2> PerspectiveEye;
    //# The UBO of the perspective camera to freely use if the device pipeline layout
    //# cannot be satisfied with #PerspectiveCameraWriteInfo. Only valid if #PerspectiveEye exists.
    UBO::LPerspectiveCamera PerspectiveCamera;
    //# Perspective device buffer to freely use. Only valid if #PerspectiveEye exists.
    vk::DescriptorBufferInfo PerspectiveCameraWriteInfo;
    vk::DescriptorSet PerspectiveCameraDescriptorSet;

    //#
    //# The default perspective polygon mode. If rendering does not require a specific polygon mode,
    //# this one should be used.
    //# If your draw call requires something else. Then reset the command buffer to this polygon mode after your
    //# buffer submission.
    //#
    vk::PolygonMode DefaultPerspectivePolygonMode;
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
