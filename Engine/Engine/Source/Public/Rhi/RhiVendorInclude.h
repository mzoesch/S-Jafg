// Copyright mzoesch. All rights reserved.

#pragma once

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
#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif /* LAL_WITH_CLANG */
    #include "vk_mem_alloc.h"
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

#if !JAFG_NO_FREETYPE
    #include <ft2build.h>
    #include FT_FREETYPE_H
#endif /* !JAFG_NO_FREETYPE */

#include <stb_image.h>
#include <stb_image_write.h>

#if PLATFORM_USES_WEBGL_TWO
    #include <GLES3/gl3.h>
#endif /* PLATFORM_USES_WEBGL_TWO */
