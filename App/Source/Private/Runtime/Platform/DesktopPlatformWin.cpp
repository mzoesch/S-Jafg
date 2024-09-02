// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Runtime/Platform/DesktopPlatform.h"
#include "Runtime/Platform/DesktopPlatformWin.h"
#include "Private/Engine/CoreGlobals.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace
{

void GlfwErrorCallback(int32 Error, const char* Description)
{
}

uint32 CompileShader(const uint32 Type, const String& Source)
{
    const uint32 Id  = glCreateShader(Type);
    const char*  Src = Source.c_str();

    glShaderSource(Id, 1, &Src, nullptr);
    glCompileShader(Id);

    int32 Result;
    glGetShaderiv(Id, GL_COMPILE_STATUS, &Result);

    if (Result == GL_FALSE)
    {
        int32 Length;
        glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &Length);

        char* Message = static_cast<char*>(alloca(Length * sizeof(char)));
        glGetShaderInfoLog(Id, Length, &Length, Message);

        jassert( false )

        /*
         * Do not remove this return for now as else we would let go of the char* Message (out of scope)
         * and therefore would not be able to see the error message in the debugger.
         */
        return Id;
    }

    return Id;
}

uint32 CreateShader(const String& InVertexShader, const String& InFragmentShader)
{
    const uint32 Program = glCreateProgram();

    const uint32 VertexShader   = CompileShader(GL_VERTEX_SHADER, InVertexShader);
    const uint32 FragmentShader = CompileShader(GL_FRAGMENT_SHADER, InFragmentShader);

    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    glValidateProgram(Program);

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}

}

void DesktopPlatformWin::Init()
{
    {
        const int32 Result = glfwInit();
        jassert(Result == GLFW_TRUE )
        glfwSetErrorCallback(GlfwErrorCallback);
    }

    this->MasterWindow = this->CreateNativeWindow(DesktopSurfaceProps());

    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    const std::string VertexShader = R"(
        #version 330 core

        layout(location = 0) in vec4 position;

        void main()
        {
            gl_Position = position;
        }
    )";

    const std::string FragmentShader = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        void main()
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    const uint32 Shader = CreateShader(VertexShader, FragmentShader);

    glUseProgram(Shader);

    this->CompiledShaders.emplace_back(Shader);

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

    LOG_DEBUG(LogCore, "Deleting {} shaders.", this->CompiledShaders.size())
    for (const uint32 Shader : this->CompiledShaders)
    {
        glDeleteProgram(Shader);
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

    glClear(GL_COLOR_BUFFER_BIT);

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
