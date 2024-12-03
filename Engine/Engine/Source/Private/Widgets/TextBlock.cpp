// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/TextBlock.h"
#include "Rhi/Shader.h"
#include "Rhi/RhiVendorInclude.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.inl>
#include "Forward/EngineForward.h"
#include "User/UserPreferences.h"
#include "Widgets/Viewport.h"
#include "System/EnginePath.h"

namespace
{

struct Character final
{
    uint32     TextureId = 0;             // ID handle of the glyph texture
    glm::ivec2 Size      = glm::ivec2();  // Size of glyph
    glm::ivec2 Bearing   = glm::ivec2();  // Offset from baseline to left/top of glyph
    Jafg::LIntVector2 Advance  = Jafg::LIntVector2(); // Offset to advance to next glyph

    Character() = default;
    Character(const Character& InOther)
    {
        this->TextureId = InOther.TextureId;
        this->Size      = InOther.Size;
        this->Bearing   = InOther.Bearing;
        this->Advance   = InOther.Advance;

        return;
    }

    Character(const uint32 InTextureId, const glm::ivec2& InSize, const glm::ivec2& InBearing, const Jafg::LIntVector2& InAdvance)
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
std::map<uint8, Character> Characters;

}

Jafg::LTextBlockBrush Jafg::LTextBlockBrush::MakeDefaultSmall()
{
    return LTextBlockBrush
    {
        .Tint  = LColor(0, 0, 0, 128),
        .Scale = GetDefault<JUserPreferences>()->GetDefaultSmallFontSize()
    };
}

void Jafg::WTextBlock::Construct()
{
    Super::Construct();

    const LIntVector2 WindowDimensions = this->GetViewportSize();

    this->FontShaderProgram = new LShader(LEnginePath(EEnginePaths::Shaders, "Font"));
    checkSlow( this->FontShaderProgram )
    this->FontShaderProgram->Use();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    glUniformMatrix4fv(glGetUniformLocation(FontShaderProgram->GetId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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

void Jafg::WTextBlock::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    if (this->Brush.Tint != LColor::Transparent)
    {
        if (this->TintShaderContext.IsMeaningful() == false)
        {
            this->TintShaderContext.Make();
        }

        this->TintShaderContext.Draw(
            Context,
            this->GetDesiredSize(),
            this->GetRelativeTopLeftFromMostOuter(this),
            this->Brush.Tint
        );
    }
    else
    {
        if (this->TintShaderContext.IsMeaningful())
        {
            this->TintShaderContext.Free();
        }
    }

    checkSlow( Characters.empty() == false )
    check( this->FontShaderProgram )

    this->FontShaderProgram->Use();
    glUniform3f(glGetUniformLocation(this->FontShaderProgram->GetId(), "textColor"), this->Brush.Color.R, this->Brush.Color.G, this->Brush.Color.B);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->Vao);

#if !PLATFORM_WASM
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif /* !PLATFORM_WASM */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    LIntVector2 WindowDimensions = this->GetViewportSize();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    glUniformMatrix4fv(glGetUniformLocation(FontShaderProgram->GetId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    const LVector2 Offset = this->GetRelativeTopLeftFromMostOuter(this);
    const float ScaleFactor = Context.GetScaleFactor();

    const float YFromBottom = static_cast<float>(WindowDimensions.Y);

    float X = Offset.X + this->Padding.Left;

    glUniform1f(glGetUniformLocation(this->FontShaderProgram->GetId(), "OrthoZDepth"), Context.GetFrameOrthoZLayerDepth());

    for (const uint8 Rune : this->Content)
    {
        const Character& Ch = Characters[Rune];

        const int32 PosX = static_cast<int32>((X + Ch.Bearing.x * this->Brush.Scale) * ScaleFactor);
        const int32 PosY =
            static_cast<int32>
            (
                 YFromBottom-this->GetDesiredSize().Y -
                 ((-this->Padding.Bottom + Offset.Y - ((Ch.Size.y - Ch.Bearing.y) * this->Brush.Scale)) * ScaleFactor)
            );

        const int32 CharW = static_cast<int32>(Ch.Size.x * this->Brush.Scale * ScaleFactor);
        const int32 CharH = static_cast<int32>(Ch.Size.y * this->Brush.Scale * ScaleFactor);

        const float PosXf = static_cast<float>(PosX);
        const float PosYf = static_cast<float>(PosY);
        const float CharWf = static_cast<float>(CharW);
        const float CharHf = static_cast<float>(CharH);

        // update VBO for each character
        const float Vertices[6][4] = {
            { PosXf,     PosYf + CharHf,   0.0f, 0.0f },
            { PosXf,     PosYf,       0.0f, 1.0f },
            { PosXf + CharWf, PosYf,       1.0f, 1.0f },

            { PosXf,     PosYf + CharHf,   0.0f, 0.0f },
            { PosXf + CharWf, PosYf,       1.0f, 1.0f },
            { PosXf + CharWf, PosYf + CharHf,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, Ch.TextureId);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        X += (Ch.Advance.X >> 6) * this->Brush.Scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return;
}

void Jafg::WTextBlock::UpdateDesiredSize() const
{
    if (this->Content.IsEmpty())
    {
        this->SetDesiredSize(this->Padding.GetDesiredSize());
        return;
    }

    LVector2 DesiredSize = LVector2::Zero();
    for (const uint8 Rune : this->Content)
    {
        const Character& Ch = Characters.at(static_cast<int8>(Rune));
        DesiredSize.X += static_cast<float>(Ch.Advance.X) * this->Brush.Scale / 64.0f;
        DesiredSize.Y = Maths::Max(DesiredSize.Y, static_cast<float>(Ch.Size.y) * this->Brush.Scale);
    }

    DesiredSize += this->Padding.GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

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
    for (uint8 C = 0; C < 128; C++)
    {
        // load character glyph
        if (FT_Load_Char(Face, C, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load Glyph" << '\n';
            std::cout.flush();
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // int width = Face->glyph->bitmap.width;
        // int rows = Face->glyph->bitmap.rows;
        // uint8* flippedBuffer = new uint8[Face->glyph->bitmap.width * Face->glyph->bitmap.rows];
        // for (int y = 0; y < rows; y++) {
        //     // Copy each row from bottom to top
        //     memcpy(flippedBuffer + y * width, Face->glyph->bitmap.buffer + (rows - 1 - y) * width, width);
        // }

        // FontHeight = Face->height;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            Face->glyph->bitmap.width,
            Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            // flippedBuffer
            Face->glyph->bitmap.buffer
        );
        // delete[] flippedBuffer;
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
        Characters.insert(std::pair<uint8, Character>(C, character));
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    FT_Done_Face(Face);
    FT_Done_FreeType(Ft);

    return;
}
