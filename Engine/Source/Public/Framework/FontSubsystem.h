// Copyright mzoesch. All rights reserved.

#pragma once

#include <Rhi/Texture2.h>

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Glyph.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/VisualInstance.h"
#include "FontSubsystem.generated.h"

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace msdfgen
{

class FontHandle;

} /* msdfgen */

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JFontSubsystem : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

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
        //# The base glyphes to add. Has to be non-empty.
        LString BaseSet{ AsciiSet() };
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
    void ReloadFont(FontCreateInfo Info) noexcept;

    struct GlyphInfo final
    {
        LVec4F Rect;
        LVec4F TexCoordRect;
        u32 BindlessTextureIndex;
        u32 SamplerIndex;
        f32 ScreenPxRange;
    };

    TArray<GlyphInfo> GetGlyphInfos(LString const& Text, f32 FontSize, LVec2F Pencil, u32 FontIndex) const noexcept;

private:

    inline static constexpr u32 MaxAllowedFonts{10};

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
        LTexture2Ref Atlas;
        std::unordered_map<u32, GlyphUVs> GlyphUVsMap;
    };

    FT_Library My_FT_Library{};
    TStackArray<MyFont, MaxAllowedFonts> My_Fonts;
};

} /* ~Namespace Jafg */
