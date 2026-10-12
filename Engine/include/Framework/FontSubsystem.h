// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/RendererCore.h"
#include "Rhi/Texture2.h"
#include "Nodes/Node.h"
#include "Rhi/Glyph.h"
#include "FontSubsystem.generated.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;
typedef FT_LibraryRec_* FT_Library;
typedef FT_FaceRec_* FT_Face;

struct hb_font_t;
typedef hb_font_t hb_font_t;

namespace msdfgen
{

class FontHandle;
template <typename T, int N> struct BitmapSection;

} /* msdfgen */

namespace msdf_atlas
{

typedef unsigned char byte;
class GlyphGeometry;
struct GeneratorAttributes;
extern void scanlineGenerator(const msdfgen::BitmapSection<float, 1> &output, const GlyphGeometry &glyph, const GeneratorAttributes &attribs);
extern void sdfGenerator(const msdfgen::BitmapSection<float, 1> &output, const GlyphGeometry &glyph, const GeneratorAttributes &attribs);
extern void psdfGenerator(const msdfgen::BitmapSection<float, 1> &output, const GlyphGeometry &glyph, const GeneratorAttributes &attribs);
extern void msdfGenerator(const msdfgen::BitmapSection<float, 3> &output, const GlyphGeometry &glyph, const GeneratorAttributes &attribs);
extern void mtsdfGenerator(const msdfgen::BitmapSection<float, 4> &output, const GlyphGeometry &glyph, const GeneratorAttributes &attribs);

template<typename AtlasGenerator> class DynamicAtlas;
template <typename T, int N> using GeneratorFunction = void (*)(msdfgen::BitmapSection<T, N> const&, GlyphGeometry const&, GeneratorAttributes const&);
template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage> class ImmediateAtlasGenerator;
template <typename T, int N> class BitmapAtlasStorage;

} /* msdf_atlas */

namespace Jafg
{

//# Local index to a glyph in a font. This is font specific and cannot be used across fonts.
typedef u32 LGlyphIndex;

namespace Detail
{

//#
//# We use multi-channel signed distance fields for font rendering, which requires 3 channels (RGB).
//# TODO: We may want to enable some user preferences for this because of bad performance.
//#
inline constexpr u32 FontAtlasColorChannels{ 3 };

typedef msdf_atlas::DynamicAtlas<msdf_atlas::ImmediateAtlasGenerator<
      f32, FontAtlasColorChannels, msdf_atlas::msdfGenerator
    , msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, FontAtlasColorChannels>
    >> MyDynamicAtlas;
typedef int MyDynamicAtlasChangeFlags;

} /* ~Namespace Detail */

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
SERDE_ENUM(EFontTypeBits, Identity, Thin, ExtraLight, Light, Regular, Medium, Semibold, Bold, ExtraBold, Black, Italic)

enum struct EFontAtlasChangeFlags: u16
{
    Identity    = 0x0,
    Resized     = 0x1 << 0,
    Rearranged  = 0x1 << 1,
    Outdated    = 0x1 << 2,
};
ENUM_STRUCT_FLAGS(EFontAtlasChangeFlags, EFontAtlasChange)

DECLARE_JAFG_CLASS()
class ENGINE_API JFontSubsystem : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

public:

    struct FontCreateInfo final
    {
        //# Source of the font.
        LPath Source;
        //#
        //# The max allowed size of a glyph in a font atlas.
        //# This value will be used unless the user has set a smaller value in their preferences (prob.
        //# performance reasons).
        //#
        f32 MaxAtlasGlyphSize{ 32.0f };
        //# This value or clamped by user preferences.
        f32 MaxPixelRange{ 2.0f };
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

private:

    struct GlyphUVs final
    {
        /* LBRT in pixels. */
        f64 PlaneBounds[4];
        /* LBRT in normal space. */
        f64 AtlasBounds[4];
    };

    //# A loaded font.
    struct MyFont final
    {
        explicit ENGINE_API MyFont(FT_Library Library, FontCreateInfo const& Info) noexcept;
        PROHIBIT_COPY(MyFont)
        explicit MyFont(MyFont&& Rhs) noexcept { *this = std::move(Rhs); }
        ENGINE_API MyFont& operator=(MyFont&& Rhs) noexcept;
        ENGINE_API ~MyFont() noexcept { this->Orphan(); }

        NODISCARD ENGINE_API LGlyphCollection Shape(LStringView Text, f32 FontSize, LVec2F* Pencil);
        NODISCARD ENGINE_API EFontAtlasChange AddGlyphsByIndex(std::unordered_set<LGlyphIndex> const& Glyphs);
        ENGINE_API void UploadToDevice();

        NODISCARD FORCEINLINE constexpr f32 GeometryScaleFor(f32 FontScale) const noexcept
        {
            check(this->EmSize > 0.0f)
            return FontScale / this->EmSize;
        }
        NODISCARD FORCEINLINE constexpr f32 FreetypeScaleFor(f32 FontScale) const noexcept
        {
            return FontScale / this->AtlasGlyphSize;
        }

        std::size_t Generation{};

        //# Source file of the font.
        LPath Source;

        f32 AtlasGlyphSize{};
        f32 PixelRange{};
        // TODO: Edge coloring functor
        f32 AngleThreshold{3.0f}; /* @see edge-coloring.h */
        // TODO: Seed

        //# The face's family name. This is an ASCII string, usually in
        //# English, that describes the typeface's family (like 'Times New
        //# Roman', 'Bodoni', 'Garamond', etc). This is a least common
        //# denominator used to list fonts. Some formats (TrueType & OpenType)
        //# provide localized and Unicode versions of this string. Applications
        //# should use the format-specific interface to access them. Can be
        //# 'NULL' (e.g., in fonts embedded in a PDF file).
        //#
        //# In case the font doesn't provide a specific family name entry,
        //# FreeType tries to synthesize one, deriving it from other name
        //# entries.
        std::optional<LString> FamilyName;
        //# The face's style name.  This is an ASCII string, usually in English,
        //# that describes the typeface's style (like 'Italic', 'Bold',
        //# 'Condensed', etc). Not all font formats provide a style name, so
        //# this field is optional, and can be set to 'NULL'. As for
        //# 'family_name', some formats provide localized and Unicode versions
        //# of this string. Applications should use the format-specific
        //# interface to access them.
        std::optional<LString> StyleName;

        //# The size of one em.
        f32 EmSize;
        //# The typographic ascender of the face, expressed in font units. For
        //# font formats not having this information, it is set to 'bbox.yMax'.
        //# Only relevant for scalable formats.
        f32 Ascender;
        //# The typographic descender of the face, expressed in font units. For
        //# font formats not having this information, it is set to 'bbox.yMin'.
        //# Note that this field is negative for values below the baseline.
        //# Only relevant for scalable formats.
        f32 Descender;
        //# This value is the vertical distance between two consecutive
        //# baselines, expressed in font units. It is always positive. Only
        //# relevant for scalable formats.
        //#
        //# If you want the global glyph height, use 'ascender - descender'.
        f32 Height;

        //# The maximum advance width, in font units, for all glyphs in this
        //# face. This can be used to make word wrapping computations faster.
        //# Only relevant for scalable formats.
        f32 MaxAdvanceWidth;
        //# The maximum advance height, in font units, for all glyphs in this
        //# face. This is only relevant for vertical layouts, and is set to
        //# 'height' for fonts that do not provide vertical metrics. Only
        //# relevant for scalable formats.
        f32 MaxAdvanceHeight;

        //# The position, in font units, of the underline line for this face.
        //# It is the center of the underlining stem. Only relevant for
        //# scalable formats.
        f32 UnderlinePosition;
        //# The thickness, in font units, of the underline for this face. Only
        //# relevant for scalable formats.
        f32 UnderlineThickness;

        //# The glyph index to uv cache. The index is not the codepoint.
        std::unordered_map<LGlyphIndex, GlyphUVs> Glyphs;

        //# Device texture handle. For internal purposes only. Do not use.
        LTexture2Ref AtlasTexture;
        //# Host texture generator. For internal purposes only. Do not use.
        algo::exchange_storage<Detail::MyDynamicAtlas*> DynamicAtlas;
        NODISCARD ENGINE_API LVec2i32 GetDynamicAtlasDimensions() const noexcept;

        //# For internal purposes only. Do not use.
        FT_Face My_FT_Face{};
        //# For internal purposes only. Do not use.
        msdfgen::FontHandle* Font{};
        //# For internal purposes only. Do not use.
        hb_font_t* HbFont{};

    private:

        ENGINE_API void Orphan() noexcept;
    };

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JFontSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

    //#
    //# Completely reloads a font.
    //# @return The index of the font for reference.
    //# @warning Very slow.
    //#
    NODISCARD u32 ReloadFont(FontCreateInfo const& Info) noexcept;
    //# TODO: Update existing font atlas with new set...

    NODISCARD bool IsFontValid(u32 FontIndex) const noexcept { return FontIndex < this->My_Fonts.size(); }
    NODISCARD MyFont const& GetFont(u32 FontIndex) const noexcept { return this->My_Fonts[FontIndex]; }

    NODISCARD FORCEINLINE LGlyphCollection GetGlyphInfos(u32 FontIndex, LStringView Text, f32 FontSize, LVec2F* Pencil) const noexcept
    {
        check(this->IsFontValid(FontIndex))
        return this->My_Fonts[FontIndex].Shape(Text, FontSize, Pencil);
    }

private:

    inline static constexpr u32 MaxAllowedFonts{10};
    FT_Library My_FT_Library{};
    mutable std::inplace_vector<MyFont, MaxAllowedFonts> My_Fonts;
};

} /* ~Namespace Jafg */
