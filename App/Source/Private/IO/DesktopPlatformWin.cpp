// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "IO/DesktopPlatformWin.h"
#include "Private/Engine/CoreGlobals.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

static void GLFWErrorCallback(int32 Error, const char* Description)
{
}

void DesktopPlatformWin::Init()
{
    {
        int32 Result = glfwInit();
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    this->MasterWindow = this->CreateNativeWindow(DesktopSurfaceProps());

    {
        int32 Result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    // BEGIN Just Temp
    constexpr float Locations[6] =
    {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    uint32 Buffer;
    glGenBuffers(1, &Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), Locations, GL_STATIC_DRAW);

    return;
}

void DesktopPlatformWin::TearDown()
{
    if (this->MasterWindow)
    {
        glfwDestroyWindow(this->MasterWindow);
        --this->WindowCount;
        this->MasterWindow = nullptr;
    }

    glfwTerminate();

    return;
}

void DesktopPlatformWin::OnUpdate()
{
    if (this->MasterWindow == nullptr)
    {
        return;
    }

    if (glfwWindowShouldClose(this->MasterWindow))
    {
        RequestEngineExit("Window closed.");
        return;
    }

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(this->MasterWindow);

    glfwPollEvents();

    return;
}

int32 DesktopPlatformWin::GetWidth() const
{
    return 0;
}

int32 DesktopPlatformWin::GetHeight() const
{
    return 0;
}

void DesktopPlatformWin::SetVSync(const bool bEnabled)
{
}

bool DesktopPlatformWin::IsVSync() const
{
    return false;
}

GLFWwindow* DesktopPlatformWin::CreateNativeWindow(const DesktopSurfaceProps& Props)
{
    GLFWwindow* Window = glfwCreateWindow(Props.Width, Props.Height, Props.Title.c_str(), nullptr, nullptr);
    ++this->WindowCount;

    glfwMakeContextCurrent(Window);

    return Window;
}
