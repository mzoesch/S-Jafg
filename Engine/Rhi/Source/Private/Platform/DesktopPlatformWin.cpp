// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Forward/EngineForward.h"
#include "Platform/DesktopPlatformWin.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.inl>
#include "RhiFramework/Shader.h"
#include <glm/glm.hpp>
#include "Widgets/Viewport.h"

void Jafg::LDesktopPlatformWin::Initialize()
{
    LDesktopPlatformBase::Initialize();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->MasterWindow = this->CreateNativeWindow(LDesktopSurfaceProps());
    if (this->MasterWindow == nullptr)
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glfw window.")
        return;
    }
    glfwSetWindowUserPointer(this->MasterWindow, reinterpret_cast<void*>(this));

    /*
     * We have to call this, as there is no default set by glfw. The default is open for the
     * implementer to decide. Not calling this method directly after initializing the window
     * will cause undefined behavior (There could be a mismatch between the context window and
     * the member variable DesktopPlatformWin#bVSync).
     *
     * @see Official GLFW Documentation:
     *      This function is not called during context creation, leaving the swap interval set to whatever is the
     *      default for that API. This is done because some swap interval extensions used by GLFW do not allow the
     *      swap interval to be reset to zero once it has been set to a non-zero value.
     *      Some GPU drivers do not honor the requested swap interval, either because of a user setting that
     *      overrides the application's request or due to bugs in the driver.
     */
    this->SetVSync(false);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))  // NOLINT(clang-diagnostic-cast-function-type-strict)
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glad.")
        return;
    }

    const TIntVector2 WindowDimensions = this->GetDimensions();
    glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);

    if (this->GetViewport())
    {
        this->GetViewport()->ChangeDimensions(WindowDimensions);
    }

    glfwSetFramebufferSizeCallback(this->MasterWindow, [] (::GLFWwindow* Window, const int32 Width, const int32 Height)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Window, Width, Height);
    });
    glfwSetCursorPosCallback(this->MasterWindow, [] (::GLFWwindow* Window, const double XPos, const double YPos)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->MouseCallback(Window, XPos, YPos);
    });
    glfwSetScrollCallback(this->MasterWindow, [] (::GLFWwindow* Window, const double XOffset, const double YOffset)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->ScrollCallback(Window, XOffset, YOffset);
    });

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);

    return;
}

void Jafg::LDesktopPlatformWin::OnClear()
{
    LDesktopPlatformBase::OnClear();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void RenderText(Jafg::Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
void Jafg::LDesktopPlatformWin::OnUpdate()
{
    LDesktopPlatformBase::OnUpdate();

    glfwSwapBuffers(this->MasterWindow);

    return;
}

void Jafg::LDesktopPlatformWin::TearDown()
{
    LDesktopPlatformBase::TearDown();

    if (this->MasterWindow)
    {
        glfwDestroyWindow(this->MasterWindow);
        this->MasterWindow = nullptr;
    }

    glfwTerminate();

    return;
}

void Jafg::LDesktopPlatformWin::PollInputs()
{
    LDesktopPlatformBase::PollInputs();

    if (glfwGetKey(this->MasterWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::W);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::S);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::A);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::D);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::Q);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::E);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::Escape);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_P) == GLFW_PRESS)
    {
        PLATFORM_BREAK()
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_F1) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_F2) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    return;
}

void Jafg::LDesktopPlatformWin::PollEvents()
{
    LDesktopPlatformBase::PollEvents();

    if (this->MasterWindow)
    {
        if (glfwWindowShouldClose(this->MasterWindow))
        {
            JAFG_ENGINE_FORWARD_REQUEST_EXIT(INDEX_NONE, "Window closed by user.")
        }
    }

    glfwPollEvents();

    return;
}

void Jafg::LDesktopPlatformWin::SetInputMode(const bool bShowCursor)
{
    LDesktopPlatformBase::SetInputMode(bShowCursor);

    if (this->MasterWindow)
    {
        glfwSetInputMode(this->MasterWindow, GLFW_CURSOR, bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    return;
}

int32 Jafg::LDesktopPlatformWin::GetWidth() const
{
    return this->GetDimensions().X;
}

int32 Jafg::LDesktopPlatformWin::GetHeight() const
{
    return this->GetDimensions().Y;
}

TIntVector2<int32> Jafg::LDesktopPlatformWin::GetDimensions() const
{
    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */

    int32 Width, Height;
    glfwGetWindowSize(this->MasterWindow, &Width, &Height);

    return TIntVector2<int32>(Width, Height);
}

void Jafg::LDesktopPlatformWin::SetVSync(const bool bEnabled)
{
    this->bVSync = bEnabled;
    glfwSwapInterval(bEnabled ? 1 : 0);

    return;
}

bool Jafg::LDesktopPlatformWin::IsVSync() const
{
    return this->bVSync;
}

GLFWwindow* Jafg::LDesktopPlatformWin::CreateNativeWindow(const LDesktopSurfaceProps& Props) const
{
    jassert( this->MasterWindow == nullptr && "Currently only supporting one window." )

    ::GLFWwindow* Window = glfwCreateWindow(Props.Width, Props.Height, Props.Title.c_str(), nullptr, nullptr);
    if (Window == nullptr)
    {
        return nullptr;
    }

    glfwMakeContextCurrent(Window);

    return Window;
}

void Jafg::LDesktopPlatformWin::FramebufferSizeCallback(::GLFWwindow*, const int32 Width, const int32 Height)
{
    glViewport(0, 0, Width, Height);

    if (this->GetViewport())
    {
        this->GetViewport()->ChangeDimensions(LIntVector2(Width, Height));
    }

    return;
}

void Jafg::LDesktopPlatformWin::MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos)
{
    this->AddKeyDown(EKeys::MouseX, static_cast<float>(XPos));
    this->AddKeyDown(EKeys::MouseY, static_cast<float>(YPos));

    return;
}

void Jafg::LDesktopPlatformWin::ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset)
{
    this->AddKeyDown(EKeys::MouseWheelAxis, static_cast<float>(YOffset));
}
