// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Rhi/Shader.h"
#include "Rhi/RhiVendorInclude.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Forward/EngineForward.h"
#include "Rhi/OrthographicBoxShader.h"
#include "User/UserPreferences.h"
#include "Widgets/Viewport.h"
#include "System/EnginePath.h"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.inl>

namespace
{

FORCEINLINE Jafg::LTextBoxBrush GetDefaultTextBoxBrush()
{
    Jafg::LTextBoxBrush Brush;
    Brush.Tint = Jafg::LColor::Transparent;
    Brush.Padding = {4.5f};
    return Brush;
}

Jafg::LShader FontShaderProgram;
Jafg::LShader& GetFontShaderProgram() { return ::FontShaderProgram; }

struct Character final
{
    u32     TextureId = 0;             // ID handle of the glyph texture
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

    Character(const u32 InTextureId, const glm::ivec2& InSize, const glm::ivec2& InBearing, const Jafg::LIntVector2& InAdvance)
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
std::map<u8, Character> Characters;

} /* ~Namespace <Anonymous> */

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Header()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->HeaderFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::SubHeader()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->SubHeaderFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Body()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->BodyFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Compact()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->CompactFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Small()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->SmallFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Tiny()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->TinyFontSize);
}

void Jafg::WTextBox::Construct()
{
    Super::Construct();

    if (Characters.empty())
    {
        this->FirstTimeLoadCharacters();
    }

    ::GetFontShaderProgram().Use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &this->Vao);
    glBindVertexArray(this->Vao);

    glGenBuffers(1, &this->Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glDisable(GL_BLEND);

    return;
}

void Jafg::WTextBox::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    if (this->GetTint().A > 0)
    {
        GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
        (
            Context,
            this->GetAnchoredSize(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->GetTint()
        );
    }

    check( this->IsSlotValid() )
    checkSlow( Characters.empty() == false )

    glCullFace(GL_FRONT);

    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const float       ScaleFactor      = Context.GetScaleFactor();
    const float       YFromBottom      = static_cast<float>(WindowDimensions.Y);
    const LVector2    Offset           =
        this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context)
        + ((this->GetAnchoredSize() - this->GetPadding().GetDesiredSize() - this->TextDesiredSize) * LVector2
        (
            this->IsLeftAligned() ? 0.0f : (this->IsHCenterAligned() ? 0.5f : 1.0f),
            this->IsTopAligned()  ? 0.0f : (this->IsVCenterAligned() ? 0.5f : 1.0f)
        ));

    ::GetFontShaderProgram().Use();
    ::GetFontShaderProgram().SetColorVec3Uniform("Color", this->GetTextColor());
    ::GetFontShaderProgram().SetMatrixUniform("Projection", Maths::MakeOrthographicProjectionMatrix(WindowDimensions));
    ::GetFontShaderProgram().SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    glBindVertexArray(this->Vao);

    float X = Offset.X + this->GetPadding().Left;
    for (const u8 Rune : this->Content)
    {
        const Character& Ch = Characters[Rune];

        const float PosX = X + static_cast<float>(Ch.Bearing.x) * this->TextScale * ScaleFactor;
        const float PosY = (YFromBottom - Offset.Y - (static_cast<float>(Ch.Size.y - Ch.Bearing.y) * this->TextScale) - this->TextDesiredSize.Y - this->GetPadding().Top) * ScaleFactor;

        const float CharW = static_cast<float>(Ch.Size.x) * this->TextScale * ScaleFactor;
        const float CharH = static_cast<float>(Ch.Size.y) * this->TextScale * ScaleFactor;

        // update VBO for each character
        const float Vertices[6][4] = {
            { PosX,         PosY + CharH, 0.0f, 0.0f },
            { PosX,         PosY,         0.0f, 1.0f },
            { PosX + CharW, PosY,         1.0f, 1.0f },

            { PosX,         PosY + CharH, 0.0f, 0.0f },
            { PosX + CharW, PosY,         1.0f, 1.0f },
            { PosX + CharW, PosY + CharH, 1.0f, 0.0f }
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
        X += (Ch.Advance.X >> 6) * this->TextScale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);

    // Reset to default state machine.
    glCullFace(GL_BACK);

    return;
}

void Jafg::WTextBox::UpdateDesiredSize() const
{
    if (this->Content.IsEmpty())
    {
        this->SetDesiredSize(this->GetPadding().GetDesiredSize());
        return;
    }

    LVector2 DesiredSize = LVector2::Zero();
    for (const u8 Rune : this->Content)
    {
        const Character& Ch = Characters.at(static_cast<i8>(Rune));
        DesiredSize.X += static_cast<float>(Ch.Advance.X) * this->TextScale / 64.0f;
        DesiredSize.Y = Maths::Max(DesiredSize.Y, static_cast<float>(Ch.Size.y) * this->TextScale);
    }
    this->TextDesiredSize = DesiredSize;

    DesiredSize += this->GetPadding().GetDesiredSize();
    this->SetDesiredSize(DesiredSize);

    return;
}

void Jafg::WTextBox::FirstTimeLoadCharacters()
{
    LOG_VERBOSE(LogFontSubsystem, "Loading characters for the first time.")

    ::FontShaderProgram = LShader(LEnginePath(EEnginePaths::Shaders, "Font"));

    FT_Library Ft;
    if (FT_Init_FreeType(&Ft))
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize Freetype library.")
        return;
    }

    FT_Face Face;
    const u8* FontData = nullptr;
    u64 FontDataSize = 0;
    Finder::ReadFileAsBinary(LEnginePath(EEnginePaths::Fonts, "Core.otf"), &FontData, &FontDataSize);
    if (FT_New_Memory_Face(Ft, reinterpret_cast<const FT_Byte*>(FontData), static_cast<FT_Long>(FontDataSize), 0, &Face))
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to load font face.")
        return;
    }

    FT_Set_Pixel_Sizes(Face, 0, 48); // set size to load glyphs as
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // load first 128 characters of ASCII set
    for (u8 C = 0; C < 128; C++)
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
        Characters.insert(std::pair<u8, Character>(C, character));
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    FT_Done_Face(Face);
    FT_Done_FreeType(Ft);

    Finder::FreeReadFileBinaryBuffer(&FontData);

    return;
}
