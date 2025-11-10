// Copyright mzoesch. All rights reserved.

#ifndef INCLUDED_RHI_VENDOR_INCLUDE_H
    #define INCLUDED_RHI_VENDOR_INCLUDE_H
#else /* INCLUDED_RHI_VENDOR_INCLUDE_H */
    #error "RhiVendorInclude.h was included more than once. Was this header included inside another header file?"
    #error "This is disallowed - only include this header inside translation units."
#endif /* !INCLUDED_RHI_VENDOR_INCLUDE_H */

#if !JAFG_NO_GLAD
    // #include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
#endif /* !JAFG_NO_GLAD */

#if !JAFG_NO_GLFW3
    #ifndef GLFW_INCLUDE_NONE
        #define GLFW_INCLUDE_NONE
    #endif /* !GLFW_INCLUDE_NONE */
    // #include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#endif /* !JAFG_NO_GLFW3 */

#include <volk.h>

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
