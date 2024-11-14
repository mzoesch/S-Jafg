// Copyright mzoesch. All rights reserved.

#ifndef INCLUDED_RHI_VENDOR_INCLUDE_H
    #define INCLUDED_RHI_VENDOR_INCLUDE_H
#else /* INCLUDED_RHI_VENDOR_INCLUDE_H */
    #error "RhiVendorInclude.h was included more than once. Was this header included inside another header file?"
    #error "This is disallowed - only include this header inside translation units."
#endif /* !INCLUDED_RHI_VENDOR_INCLUDE_H */

#include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */

#if JAFG_WITH_GLFW3
    #include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#endif /* JAFG_WITH_GLFW3 */

#include <stb_image.h>
