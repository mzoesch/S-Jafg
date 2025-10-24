// Copyright mzoesch. All rights reserved.

#include "Rhi/OrthographicTextShader.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

bool Jafg::LOrthographicTextShader::Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants)
{
    if (const bool bOut{ Super::Make(InName, std::move(InConstants)) }; bOut == false)
    {
        return false;
    }

    LOG_VERBOSE(LogRhi, "Creating OrthographicTextShader at [{}].", InName)

    this->Program = LShader{LEnginePath{EEnginePaths::Shaders, "Font"}, this->Constants};
    this->Program.Use();

    FT_Library Library;
    if (::FT_Init_FreeType(&Library))
    {
        LOG_ERROR(LogRhi, "Failed to initialize Freetype library.")
        return false;
    }

    FT_Face Face;
    TArray FontData{ Finder::ReadFileAsBinary(this->FontPath.ResolvePath()) };
    if (::FT_New_Memory_Face(Library, reinterpret_cast<const FT_Byte*>(FontData.data()), static_cast<FT_Long>(FontData.size()), 0, &Face))
    {
        LOG_ERROR(LogRhi, "Failed to load font face.")
        return false;
    }

    ::FT_Set_Pixel_Sizes(Face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (u8 Ascii{ 0 }; Ascii < 128; ++Ascii)
    {
        if (::FT_Load_Char(Face, Ascii, FT_LOAD_RENDER))
        {
            LOG_ERROR(LogFontSubsystem, "Failed to load ascii Glyph [{}].", static_cast<i32>(Ascii))
            continue;
        }

        u32 Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        LCharacter Character{
              Texture
            , Lu32Vector2{ Face->glyph->bitmap.width, Face->glyph->bitmap.rows }
            , Li32Vector2{ Face->glyph->bitmap_left, Face->glyph->bitmap_top }
            , Li64Vector2{ Face->glyph->advance.x, Face->glyph->advance.y }
            };

        Characters.insert(std::pair{Ascii, Character});
    }
    glBindTexture(GL_TEXTURE_2D, 1);
    FT_Done_Face(Face);
    FT_Done_FreeType(Library);

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

    this->ApproxBearingHeight = -1.0f;

    LStringView TheCharsWeCareAbout{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        };

    for (auto Char : TheCharsWeCareAbout)
    {
        LCharacter const& Ch{ Characters.at(static_cast<u8>(Char)) };
        this->ApproxBearingHeight = Maths::Max(this->ApproxBearingHeight, static_cast<f32>(Ch.Bearing.Y));
    }

    return true;
}

void Jafg::LOrthographicTextShader::UpdateViewportUniforms(const LViewport& Context)
{
    Super::UpdateViewportUniforms(Context);

    this->Program.Use();
    this->Program.SetMatrixUniform("Projection", Context.GetCachedOrthographicProjectionMatrix());

    return;
}

void Jafg::LOrthographicTextShader::OnFree()
{
    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

    Super::OnFree();

    return;
}

void Jafg::LOrthographicTextShader::Draw
(
    const LViewport& Context,
    const LVector2& Size,
    const LVector2& TopLeft,
    const LPadding& Padding,
    const LVector2& TextDesiredSize,
    const ETextHAlign::Type TextHAlign,
    const ETextVAlign::Type TextVAlign,
    const Lal::LColor& TextColor,
    const f32 TextScale,
    const LString& Content
) const
{
    check( this->Characters.empty() == false )

    glCullFace(GL_FRONT);

    LPadding SptPadding{ Padding.InSpt(Context) };
    check( SptPadding.Type == EWidgetSize::StaticPoints )

    const f32       ScaleFactor      = Context.GetScaleFactor();
    const f32       YFromBottom      = Context.GetHeightF();
    const LVector2    Offset           =
        TopLeft
        + ((Size - SptPadding.GetDesiredSizeRaw() - TextDesiredSize) * LVector2
        (
            ETextHAlign::IsLeft(TextHAlign) ? 0.0f : (ETextHAlign::IsCenter(TextHAlign) ? 0.5f : 1.0f),
            ETextVAlign::IsTop(TextVAlign)  ? 0.0f : (ETextVAlign::IsCenter(TextVAlign) ? 0.5f : 1.0f)
        ));

    this->Program.Use();
    this->Program.SetColorVec3Uniform("Color", TextColor);
    this->Program.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());

    glBindVertexArray(this->Vao);

    f32 X = Offset.X + SptPadding.Left;
    for (const u8 Rune : Content)
    {
        const LCharacter& Ch = Characters.at(Rune);

        const f32 PosX = X + static_cast<f32>(Ch.Bearing.X) * TextScale * ScaleFactor;
        const f32 PosY = (YFromBottom - Offset.Y - (static_cast<f32>(Ch.Size.Y - Ch.Bearing.Y) * TextScale) - TextDesiredSize.Y - SptPadding.Top) * ScaleFactor;

        const f32 CharW = static_cast<f32>(Ch.Size.X) * TextScale * ScaleFactor;
        const f32 CharH = static_cast<f32>(Ch.Size.Y) * TextScale * ScaleFactor;

        const f32 Vertices[6][4]
        {
            { PosX,         PosY + CharH, 0.0f, 0.0f },
            { PosX,         PosY,         0.0f, 1.0f },
            { PosX + CharW, PosY,         1.0f, 1.0f },

            { PosX,         PosY + CharH, 0.0f, 0.0f },
            { PosX + CharW, PosY,         1.0f, 1.0f },
            { PosX + CharW, PosY + CharH, 1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, Ch.Handle);
        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        X += (Ch.Advance.X >> 6) * TextScale;

        continue;
    }

    glBindVertexArray(0);

    /* Reset to default state machine. */
    glCullFace(GL_BACK);

    return;
}
