// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/RendererCore.h"
#include "Rhi/Texture2.h"
#include "Nodes/Node.h"
#include "Rhi/Glyph.h"
#include "FontSubsystem.generated.h"

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace msdfgen
{

class FontHandle;

} /* msdfgen */

namespace Jafg
{

enum EFontTypeBits: u16
{
    Identity    = 0x0,
    Thin        = 0x1 << 0,
    ExtraLight  = 0x1 << 1,
    Light       = 0x1 << 2,
    Regular     = 0x1 << 3,
    Medium      = 0x1 << 4,
    Semibold    = 0x1 << 5,
    Bold        = 0x1 << 6,
    ExtraBold   = 0x1 << 7,
    Black       = 0x1 << 8,
    Italic      = 0x1 << 9,
};
ENUM_STRUCT_FLAGS(EFontTypeBits, EFontType)
SERDE_ENUM_MAP(EFontTypeBits, Identity, Thin, ExtraLight, Light, Regular, Medium, Semibold, Bold, ExtraBold, Black, Italic)

DECLARE_JAFG_CLASS()
class ENGINE_API JFontSubsystem : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

    struct GlyphUVs
    {
        /* LBRT in pixels. */
        f64 PlaneBounds[4];
        /* LBRT in normal space. */
        f64 AtlasBounds[4];
    };

    struct MyFont
    {
        MyFont() noexcept = default;

        LPath Source;
        FT_Face My_FT_Face{};
        msdfgen::FontHandle* Font{};
        f32 AtlasGlyphSize{};
        f32 PixelRange{};
        f64 Ascender{};
        f64 Descender{};
        f64 LineHeight{};
        LTexture2Ref Atlas;
        std::unordered_map<u32, GlyphUVs> GlyphUVsMap;
    };

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JFontSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

    struct FontCreateInfo final
    {
        //# Source of the font.
        LPath Source;
        //#
        //# The max allowed size of a glyph in a font atlas.
        //# This value will be used unless the user has set a smaller value in their preferences (prob.
        //# performance reasons).
        //#
        f32 MaxAtlasGlyphSize{ 64.0f };
        //# This value or clamped by user preferences.
        f32 MaxPixelRange{ 6.0f };
        //# Inner and outer padding to apply to each quad.
        LVec2F Padding{ maths::zero_vector<LVec2F> };
        //# The base glyphes to add. Has to be non-empty.
        LString BaseSet{ AsciiSet() };
        //#
        //# Optional tags to set.
        //# If at least one tag was specified, all other tags that a font provides also have to be provided.
        //#
        TArray<std::pair<LString, signed long>> Tags;
    };
    inline static constexpr LString AsciiSet() noexcept
    {
        LString Result; Result.reserve(0x7F - 0x20 + 1);
        for (i32 Char{0x20}; Char < 0x7F; ++Char)
        {
            Result.push_back(static_cast<char>(Char));
        }
        return Result;
    }

    //#
    //# Completely reloads a font.
    //# @return The index of the font for reference.
    //#
    NODISCARD u32 ReloadFont(FontCreateInfo const& Info) noexcept;
    //# TODO: Update existing font atlas with new set...

    NODISCARD bool IsFontValid(u32 FontIndex) const noexcept { return FontIndex < this->My_Fonts.size(); }
    NODISCARD MyFont const& GetFont(u32 FontIndex) const noexcept { return this->My_Fonts[FontIndex]; }

    LGlyphCollection GetGlyphInfos(LStringView Text, f32 FontSize, LVec2F* Pencil, u32 FontIndex) const noexcept;

private:

    inline static constexpr u32 MaxAllowedFonts{10};
    FT_Library My_FT_Library{};
    std::inplace_vector<MyFont, MaxAllowedFonts> My_Fonts;
};

} /* ~Namespace Jafg */
