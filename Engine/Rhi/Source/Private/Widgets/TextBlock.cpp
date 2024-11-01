// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Widgets/TextBlock.h"
#include "RhiFramework/Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.inl>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Forward/EngineForward.h"

namespace
{

struct Character final
{
    uint32     TextureId = 0;             // ID handle of the glyph texture
    glm::ivec2 Size      = glm::ivec2();  // Size of glyph
    glm::ivec2 Bearing   = glm::ivec2();  // Offset from baseline to left/top of glyph
    LIntVector2 Advance  = LIntVector2(); // Offset to advance to next glyph

    Character() = default;
    Character(const Character& InOther)
    {
        this->TextureId = InOther.TextureId;
        this->Size      = InOther.Size;
        this->Bearing   = InOther.Bearing;
        this->Advance   = InOther.Advance;

        return;
    }

    Character(const uint32 InTextureId, const glm::ivec2& InSize, const glm::ivec2& InBearing, const LIntVector2& InAdvance)
    {
        this->TextureId = InTextureId;
        this->Size      = InSize;
        this->Bearing   = InBearing;
        this->Advance   = InAdvance;

        return;
    }
};

/**
 * Loaded characters, private storage for this translation unit but usable for all WTextBlock instances.
 */
std::map<char, Character> Characters;

}

void Jafg::WTextBlock::Construct()
{
    Super::Construct();

    LIntVector2 WindowDimensions = this->GetViewportSize();

    this->FontShaderProgram = new Shader("Content/Shaders/vs_font.shader", "Content/Shaders/fs_font.shader");
    checkSlow( this->FontShaderProgram )
    this->FontShaderProgram->Use();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    glUniformMatrix4fv(glGetUniformLocation(FontShaderProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (Characters.empty())
    {
        LOG_VERBOSE(LogWidgets, "Loading standard font.")
        this->FirstTimeLoadCharacters();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);
    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return;
}

void Jafg::WTextBlock::Draw(LViewport* Context) const
{
    Super::Draw(Context);

    checkSlow( Characters.empty() == false )
    check( this->FontShaderProgram )

    this->FontShaderProgram->Use();
    glUniform3f(glGetUniformLocation(this->FontShaderProgram->ID, "textColor"), this->Brush.Color.R, this->Brush.Color.G, this->Brush.Color.B);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->Vao);

    LIntVector2 WindowDimensions = this->GetViewportSize();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    glUniformMatrix4fv(glGetUniformLocation(FontShaderProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    const LVector2 Offset = this->GetRelativeTopLeftFromMostOuter(this);

    const float YFromBottom = static_cast<float>(WindowDimensions.Y) - Offset.Y;

    float X = Offset.X;

    std::string text = this->Content.ToC();
    // iterate through all characters
    for (std::string::const_iterator c = text.begin(); c != text.end(); ++c)
    {
        Character ch = Characters[*c];

        float xpos = X + ch.Bearing.x * this->Brush.Scale;
        // float ypos = YFromBottom - (ch.Size.y - ch.Bearing.y) * this->Brush.Scale;
        float ypos = YFromBottom - ch.Size.y * this->Brush.Scale;

        float w = ch.Size.x * this->Brush.Scale;
        float h = ch.Size.y * this->Brush.Scale;
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
        glBindTexture(GL_TEXTURE_2D, ch.TextureId);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        X += (ch.Advance.X >> 6) * this->Brush.Scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return;
}

void Jafg::WTextBlock::UpdateDesiredSize() const
{
    if (this->Content.IsEmpty())
    {
        this->SetDesiredSize(LVector2::Zero());
        return;
    }

    const Character& Ch = Characters.at(this->Content.GetCharAt(0));

    const float X = static_cast<float>(Ch.Advance.X) * this->Brush.Scale * static_cast<float>(this->Content.GetRuneCount()) / 64.0f;
    const float Y = static_cast<float>(Ch.Size.y) * this->Brush.Scale;
    this->SetDesiredSize(LVector2(X, Y));

    return;
}

void Jafg::WTextBlock::FirstTimeLoadCharacters()
{
    FT_Library Ft;
    if (FT_Init_FreeType(&Ft))
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize Freetype library.")
        return;
    }

    FT_Face Face;
    if (FT_New_Face(Ft, "E:/dev/c/Jafg/Content/Fonts/Core.otf", 0, &Face))
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to load font face.")
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
        Character character =
        {
            texture,
            glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
            glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top),
            LIntVector2(Face->glyph->advance.x, Face->glyph->advance.y)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    FT_Done_Face(Face);
    FT_Done_FreeType(Ft);

    return;
}
