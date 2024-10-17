// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Platform/DesktopPlatformWin.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "Player/LocalPlayer.h"
#include "Player/PlayerInput.h"
#include <Freetype/ft2build.h>
#include <glm/gtc/type_ptr.inl>
#include FT_FREETYPE_H
#include "RHI/Shader.h"

struct Character
{
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;
unsigned int VAO, VBO;
Jafg::Shader FontShaderProgram;

void Jafg::DesktopPlatformWin::Initialize()
{
    DesktopPlatformBase::Initialize();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->MasterWindow = this->CreateNativeWindow(DesktopSurfaceProps());
    if (this->MasterWindow == nullptr)
    {
        check( GEngine )
        GEngine->RequestEngineExit(EPlatformExit::Fatal, "Failed to initialize glfw window.");
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
        check( GEngine )
        GEngine->RequestEngineExit(EPlatformExit::Fatal, "Failed to initialize glad.");
        return;
    }

    const TIntVector2 WindowDimensions = this->GetDimensions();
    glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);
    glfwSetFramebufferSizeCallback(this->MasterWindow, [] (::GLFWwindow* Window, const int32 Width, const int32 Height)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Window, Width, Height);
    });
    glfwSetCursorPosCallback(this->MasterWindow, [] (::GLFWwindow* Window, const double XPos, const double YPos)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->MouseCallback(Window, XPos, YPos);
    });
    glfwSetScrollCallback(this->MasterWindow, [] (::GLFWwindow* Window, const double XOffset, const double YOffset)
    {
        static_cast<DesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->ScrollCallback(Window, XOffset, YOffset);
    });

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);

    FontShaderProgram = Shader("Content/Shaders/vs_font.shader", "Content/Shaders/fs_font.shader");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    FontShaderProgram.Use();
    glUniformMatrix4fv(glGetUniformLocation(FontShaderProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library Ft;
    if (FT_Init_FreeType(&Ft))
    {
        check( GEngine )
        GEngine->RequestEngineExit(EPlatformExit::Fatal, "Failed to initialize Freetype library.");
        return;
    }

    FT_Face Face;
    if (FT_New_Face(Ft, "E:/dev/c/Jafg/Content/Fonts/Core.otf", 0, &Face))
    {
        check( GEngine )
        GEngine->RequestEngineExit(EPlatformExit::Fatal, "Failed to load font face.");
        return;
    }

    FT_Set_Pixel_Sizes(Face, 0, 48); // set size to load glyphs as
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(Face, c, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load Glyph" << '\n';
            std::cout.flush();
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            Face->glyph->bitmap.width,
            Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            Face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
            glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top),
            Face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    FT_Done_Face(Face);
    FT_Done_FreeType(Ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return;
}

void Jafg::DesktopPlatformWin::OnClear()
{
    DesktopPlatformBase::OnClear();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void RenderText(Jafg::Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
void Jafg::DesktopPlatformWin::OnUpdate()
{
    DesktopPlatformBase::OnUpdate();

    RenderText(FontShaderProgram, "Hello, World!", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

    glfwSwapBuffers(this->MasterWindow);

    return;
}

void RenderText(Jafg::Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Jafg::DesktopPlatformWin::TearDown()
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

void Jafg::DesktopPlatformWin::PollInputs()
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

void Jafg::DesktopPlatformWin::PollEvents()
{
    DesktopPlatformBase::PollEvents();

    if (this->MasterWindow)
    {
        if (glfwWindowShouldClose(this->MasterWindow))
        {
            check( GEngine)
            GEngine->RequestEngineExit("Window closed by user.");
        }
    }

    glfwPollEvents();

    return;
}

void Jafg::DesktopPlatformWin::SetInputMode(const bool bShowCursor)
{
    DesktopPlatformBase::SetInputMode(bShowCursor);

    if (this->MasterWindow)
    {
        glfwSetInputMode(this->MasterWindow, GLFW_CURSOR, bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    return;
}

int32 Jafg::DesktopPlatformWin::GetWidth() const
{
    return this->GetDimensions().X;
}

int32 Jafg::DesktopPlatformWin::GetHeight() const
{
    return this->GetDimensions().Y;
}

TIntVector2<int32> Jafg::DesktopPlatformWin::GetDimensions() const
{
    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */

    int32 Width, Height;
    glfwGetWindowSize(this->MasterWindow, &Width, &Height);

    return TIntVector2<int32>(Width, Height);
}

void Jafg::DesktopPlatformWin::SetVSync(const bool bEnabled)
{
    this->bVSync = bEnabled;
    glfwSwapInterval(bEnabled ? 1 : 0);

    return;
}

bool Jafg::DesktopPlatformWin::IsVSync() const
{
    return this->bVSync;
}

GLFWwindow* Jafg::DesktopPlatformWin::CreateNativeWindow(const DesktopSurfaceProps& Props) const
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

void Jafg::DesktopPlatformWin::FramebufferSizeCallback(::GLFWwindow* Window, const int32 Width, const int32 Height)
{
    glViewport(0, 0, Width, Height);
}

void Jafg::DesktopPlatformWin::MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos)
{
    if (GEngine)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseX, static_cast<float>(XPos));
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseY, static_cast<float>(YPos));
    }

    return;
}

void Jafg::DesktopPlatformWin::ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset)
{
    if (GEngine)
    {
        GEngine->GetLocalPlayer()->GetPlayerInput()->AddKeyDown(EKeys::MouseWheelAxis, static_cast<float>(YOffset));
    }

    return;
}
