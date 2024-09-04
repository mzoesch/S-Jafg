// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Runtime/Platform/DesktopPlatform.h"
#include "Runtime/Platform/DesktopPlatformWin.h"
#include "Private/Engine/CoreGlobals.h"
#include "Glfw/Renderer.h"
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace
{

void GlfwErrorCallback(int32 Error, const char* Description)
{
    LOG_WARNING(LogCore, "GLFW Error: {} - {}", Error, Description)
    checkNoEntry()

    return;
}

}

void DesktopPlatformWin::Init()
{
    {
        const int32 Result = glfwInit();
        jassert(Result == GLFW_TRUE )
        glfwSetErrorCallback(GlfwErrorCallback);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->MasterWindow = this->CreateNativeWindow(DesktopSurfaceProps());

    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    // BEGIN Just Temp
    glfwSwapInterval(1);

    constexpr float Locations[] =
    {
        -0.5f, -0.5f, 0.0f, 0.0f, // 0: Bottom Left
         0.5f, -0.5f, 1.0f, 0.0f, // 1: Bottom Right
         0.5f,  0.5f, 1.0f, 1.0f, // 2: Top Right
        -0.5f,  0.5f, 0.0f, 1.0f, // 3: Top Left
    };

    constexpr uint32 Indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    GL_CALL( glEnable(GL_BLEND) )
    GL_CALL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) )

    Va = new VertexArray();
    Vb = new VertexBuffer(Locations, 4ull * 4ull * sizeof(float));

    VertexBufferLayout Layout;
    Layout.Push<float>(2);
    Layout.Push<float>(2);
    Va->AddBuffer(*Vb, Layout);

    Ib = new IndexBuffer(Indices, 6ull);

    const glm::mat4 Proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

    Sh = new Shader(R"(E:\dev\c\Jafg\Content\Shaders\Basic.shader)");
    Sh->Bind();
    Sh->SetUniformMat4F("u_ModelViewProjection", Proj);

    Tx = new Texture(R"(E:\dev\c\Jafg\Content\Textures\pp.png)");
    Tx->Bind(0);
    Sh->SetUniform1I("u_Texture", 0);

    Va->Unbind();
    Vb->Unbind();
    Ib->Unbind();
    Sh->Unbind();

    MyRenderer = new Renderer();

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

    MyRenderer->Clear();
    MyRenderer->Draw(*Va, *Ib, *Sh);

    if (this->r > 1.0f)
    {
        this->increment = -0.005f;
    }
    else if (this->r < 0.0f)
    {
        this->increment = 0.005f;
    }
    this->r += this->increment;

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
