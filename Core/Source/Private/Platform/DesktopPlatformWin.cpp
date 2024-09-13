// Copyright mzoesch. All rights reserved.

#include "Platform/DesktopPlatformWin.h"
#include "Engine/CoreGlobals.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "Player/LocalPlayer.h"
#include "Player/PlayerInput.h"

void DesktopPlatformWin::Initialize()
{
    DesktopPlatformBase::Initialize();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->MasterWindow = this->CreateNativeWindow(DesktopSurfaceProps());
    if (this->MasterWindow == nullptr)
    {
        RequestEngineExit(EPlatformExit::Fatal, "Failed to initialize glfw window.");
        return;
    }

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        RequestEngineExit(EPlatformExit::Fatal, "Failed to initialize glad.");
        return;
    }

    const TIntVector2 WindowDimensions = this->GetDimensions();
    glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);
    glfwSetFramebufferSizeCallback(this->MasterWindow, [] (GLFWwindow* Window, const int32 Width, const int32 Height)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Window, Width, Height);
    });
    glfwSetCursorPosCallback(this->MasterWindow, [] (GLFWwindow* Window, const double XPos, const double YPos)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->MouseCallback(Window, XPos, YPos);
    });
    glfwSetScrollCallback(this->MasterWindow, [] (GLFWwindow* Window, const double XOffset, const double YOffset)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->ScrollCallback(Window, XOffset, YOffset);
    });

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);

    return;
}

void DesktopPlatformWin::OnClear()
{
    DesktopPlatformBase::OnClear();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void DesktopPlatformWin::OnUpdate()
{
    DesktopPlatformBase::OnUpdate();

    glfwSwapBuffers(this->MasterWindow);

    return;
}

void DesktopPlatformWin::TearDown()
{
    DesktopPlatformBase::TearDown();

    if (this->MasterWindow)
    {
        glfwDestroyWindow(this->MasterWindow);
        this->MasterWindow = nullptr;
    }

    glfwTerminate();

    return;
}

void DesktopPlatformWin::PollInputs()
{
    DesktopPlatformBase::PollInputs();

    check( GEngine->HasLocalPlayer() )

    if (glfwGetKey(this->MasterWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::W);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::S);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::A);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::D);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::Q);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::E);
    }
    if (glfwGetKey(this->MasterWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::Escape);
    }

    return;
}

void DesktopPlatformWin::PollEvents()
{
    DesktopPlatformBase::PollEvents();

    if (this->MasterWindow)
    {
        if (glfwWindowShouldClose(this->MasterWindow))
        {
            RequestEngineExit("Window closed by user.");
        }
    }

    glfwPollEvents();

    return;
}

void DesktopPlatformWin::SetInputMode(const bool bShowCursor)
{
    DesktopPlatformBase::SetInputMode(bShowCursor);

    if (this->MasterWindow)
    {
        glfwSetInputMode(this->MasterWindow, GLFW_CURSOR, bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    return;
}

int32 DesktopPlatformWin::GetWidth() const
{
    return this->GetDimensions().X;
}

int32 DesktopPlatformWin::GetHeight() const
{
    return this->GetDimensions().Y;
}

TIntVector2<int32> DesktopPlatformWin::GetDimensions() const
{
    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */

    int32 Width, Height;
    glfwGetWindowSize(this->MasterWindow, &Width, &Height);

    return TIntVector2<int32>(Width, Height);
}

void DesktopPlatformWin::SetVSync(const bool bEnabled)
{
    this->bVSync = bEnabled;
    glfwSwapInterval(bEnabled ? 1 : 0);

    return;
}

bool DesktopPlatformWin::IsVSync() const
{
    return this->bVSync;
}

GLFWwindow* DesktopPlatformWin::CreateNativeWindow(const DesktopSurfaceProps& Props) const
{
    jassert( this->MasterWindow == nullptr && "Currently only supporting one window." )

    GLFWwindow* Window = glfwCreateWindow(Props.Width, Props.Height, Props.Title.c_str(), nullptr, nullptr);
    if (Window == nullptr)
    {
        return nullptr;
    }

    glfwMakeContextCurrent(Window);

    return Window;
}

void DesktopPlatformWin::FramebufferSizeCallback(GLFWwindow* Window, const int32 Width, const int32 Height)
{
    glViewport(0, 0, Width, Height);
}

void DesktopPlatformWin::MouseCallback(GLFWwindow* Window, const double XPos, const double YPos)
{
    if (GEngine)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseX, static_cast<float>(XPos));
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseY, static_cast<float>(YPos));
    }

    return;
}

void DesktopPlatformWin::ScrollCallback(GLFWwindow* Window, const double XOffset, const double YOffset)
{
    if (GEngine)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseWheelAxis, static_cast<float>(YOffset));
    }

    return;
}
