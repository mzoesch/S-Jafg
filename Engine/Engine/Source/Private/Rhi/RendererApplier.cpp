// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/RendererApplier.h"
#include "RhiVendorInclude.h"
#include "Platform/DesktopPlatform.h"

namespace
{

bool bInitializedGlfw = false;

} /* ~Namespace <Anonymous> */

bool Jafg::RendererApplier::InitializeGlfw()
{
    check( bInitializedGlfw == false)
    bInitializedGlfw = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    return true;
}

bool Jafg::RendererApplier::IsGlfwInitialized()
{
    return bInitializedGlfw;
}

bool Jafg::RendererApplier::ApplyOpenGlToWindow(LNativeWindow* Window)
{
    check( bInitializedGlfw )
    check( Window )
    check( Window->GetNativeWindow() )

    glfwSetWindowUserPointer(Window->NativeWindow, reinterpret_cast<void*>(Window));
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))  // NOLINT(clang-diagnostic-cast-function-type-strict)
    {
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return true;
}
