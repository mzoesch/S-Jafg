// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/MaxDesiredFramesInFlight.h"

#if !JAFG_NO_GLAD
    // #include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
#endif /* !JAFG_NO_GLAD */

#if !JAFG_NO_GLFW3
    #ifndef GLFW_INCLUDE_NONE
        #define GLFW_INCLUDE_NONE
    #endif /* !GLFW_INCLUDE_NONE */
    // #include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#endif /* !JAFG_NO_GLFW3 */

#define VK_NO_PROTOTYPES
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan_raii.hpp>
// #include <volk.h>

// We support Vulkan 1.4. This is currently (2025) the latest version. Therefore, we do not need this right now; tm.
// #define VMA_VULKAN_VERSION 1004000
#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif /* JAFG_WITH_CLANG */
    #include "vk_mem_alloc.h"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

#if !JAFG_NO_FREETYPE
    #include <ft2build.h>
    #include FT_FREETYPE_H
#endif /* !JAFG_NO_FREETYPE */

#if PLATFORM_USES_WEBGL_TWO
    #include <GLES3/gl3.h>
#endif /* PLATFORM_USES_WEBGL_TWO */

#include "Framework/FrontendForward.h"

namespace Jafg
{

typedef VmaAllocation LDeviceAllocation;
typedef VmaAllocationInfo LDeviceAllocationInfo;

inline constexpr LSize Vk_GetChannelsPerPixel(vk::Format Format) noexcept
{
    switch (Format)
    {
    case vk::Format::eR8G8B8A8Srgb:
    {
        return 4;
    }
    case vk::Format::eR8G8B8Srgb:
    {
        return 3;
    }
    default:
    {
        panicMsgf( "Unsupported or non-linear format [{}] for channels per pixel query.", vk::to_string(Format) )
    }
    }
}

inline constexpr LSize Vk_GetBytesPerPixel(vk::Format Format) noexcept
{
    switch (Format)
    {
    case vk::Format::eR8Unorm:
    {
        return 1;
    }
    case vk::Format::eR8G8Unorm:
    {
        return 2;
    }
    case vk::Format::eR8G8B8Srgb:
    {
        return 3;
    }
    case vk::Format::eR8G8B8A8Unorm:
    case vk::Format::eR8G8B8A8Srgb:
    {
        return 4;
    }
    case vk::Format::eB8G8R8A8Unorm:
    {
        return 4;
    }
    case vk::Format::eR16G16B16A16Sfloat:
    {
        return 8;
    }
    case vk::Format::eR32G32B32A32Sfloat:
    {
        return 16;
    }
    default:
    {
        panicMsgf( "Unsupported or non-linear format [{}] for bytes per pixel query.", vk::to_string(Format) )
    }
    }
}

inline constexpr vk::SampleCountFlagBits Vk_GetMaxMsaaSamples(vk::SampleCountFlags Counts) noexcept
{
    if (Counts & vk::SampleCountFlagBits::e64)
    {
        return vk::SampleCountFlagBits::e64;
    }
    if (Counts & vk::SampleCountFlagBits::e32)
    {
        return vk::SampleCountFlagBits::e32;
    }
    if (Counts & vk::SampleCountFlagBits::e16)
    {
        return vk::SampleCountFlagBits::e16;
    }
    if (Counts & vk::SampleCountFlagBits::e8)
    {
        return vk::SampleCountFlagBits::e8;
    }
    if (Counts & vk::SampleCountFlagBits::e4)
    {
        return vk::SampleCountFlagBits::e4;
    }
    if (Counts & vk::SampleCountFlagBits::e2)
    {
        return vk::SampleCountFlagBits::e2;
    }
    if (Counts & vk::SampleCountFlagBits::e1)
    {
        return vk::SampleCountFlagBits::e1;
    }

    panicMsgf("Unsupported sample count flags [{}] for max MSAA samples query.", vk::to_string(Counts))
}

} /* ~Namespace Jafg */
