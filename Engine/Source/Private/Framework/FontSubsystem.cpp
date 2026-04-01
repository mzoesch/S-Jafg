// Copyright mzoesch. All rights reserved.

#include "Framework/FontSubsystem.h"
#include "Framework/Frontend.h"
#include "Rhi/RendererCore.h"
#include "Framework/TextureSubsystem.h"
#include "Rhi/VisualInstance.h"

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif /* JAFG_WITH_CLANG */
    #include <harfbuzz/hb.h>
    #include <harfbuzz/hb-ft.h>
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include <msdfgen.h>
    #include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <Runtime/Args.h>
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

#include <stb_image_write.h>

namespace
{

inline constexpr u32 AtlasColorChannels{ 3 };
/* Positions are in 26.6 fixed-point (1/64px) scale to pixels. */
inline constexpr f32 HarfBuzzScale{ 1.0f / 64.0f };

Lu32String utf8_to_utf32(LString const& UTF8)
{
    Lu32String Result;

    auto Idx{0uz};
    while (Idx < UTF8.size())
    {
        uint8_t C{static_cast<uint8_t>(UTF8[Idx])};
        uint32_t CP{0};
        auto Bytes{0uz};

        if      (C < 0x80)           { CP = C & 0x7F; Bytes = 1; }
        else if ((C & 0xE0) == 0xC0) { CP = C & 0x1F; Bytes = 2; }
        else if ((C & 0xF0) == 0xE0) { CP = C & 0x0F; Bytes = 3; }
        else if ((C & 0xF8) == 0xF0) { CP = C & 0x07; Bytes = 4; }
        else
        {
            /* Invalid lead byte => skip. */
            ++Idx;
            continue;
        }

        /* Validate that enough bytes exist. */
        if (Idx + Bytes > UTF8.size())
        {
            break;
        }

        for (auto SubIdx{1uz}; SubIdx < Bytes; ++SubIdx)
        {
            uint8_t Cont{static_cast<uint8_t>(UTF8[Idx + SubIdx])};
            if ((Cont & 0xC0) != 0x80)
            {
                /* Invalid continuation. */
                Bytes = 0;
                break;
            }

            CP = (CP << 6) | (Cont & 0x3F);
        }

        if (Bytes > 0)
        {
            Result.push_back(static_cast<char32_t>(CP));
        }
        Idx += Bytes > 0 ? Bytes : 1;
    }

    return Result;
}

} /* ~Namespace <Anonymous> */

void Jafg::JFontSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<JTextureSubsystem>(this);

    check(this->My_FT_Library == nullptr)
    FT_Init_FreeType(&this->My_FT_Library);
    check(this->My_FT_Library)

    check(this->My_Fonts.empty())
    this->ReloadFont({"Content/Fonts/Noto_Sans/static/NotoSans-Bold.ttf"});
    check(this->My_Fonts.size() == 1)

    return;
}

void Jafg::JFontSubsystem::TearDown()
{
    Super::TearDown();

    for (auto const& Font : this->My_Fonts)
    {
        if (Font.Font)
        {
            msdfgen::destroyFont(Font.Font);
        }
        if (Font.My_FT_Face)
        {
            FT_Done_Face(Font.My_FT_Face);
        }
    }

    if (this->My_FT_Library)
    {
        FT_Done_FreeType(this->My_FT_Library);
    }

    return;
}

void Jafg::JFontSubsystem::ReloadFont(FontCreateInfo Info) noexcept
{
    MyFont Result; Result.Source = std::move(Info.Source);

    // TODO: Ask user preferences to clamp this.
    Result.AtlasGlyphSize = Info.MaxAtlasGlyphSize;
    Result.PixelRange = Info.MaxPixelRange;

#if JAFG_PLATFORM_USES_UTF8
    FT_New_Face(this->My_FT_Library, Result.Source.native().c_str(), 0, &Result.My_FT_Face);
#else /* JAFG_PLATFORM_USES_UTF8 */
    FT_New_Face(this->My_FT_Library, Result.Source.string().c_str(), 0, &Result.My_FT_Face);
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    check(Result.My_FT_Face)

    Result.Font = msdfgen::adoptFreetypeFont(Result.My_FT_Face);

    std::vector<msdf_atlas::GlyphGeometry> Glyphes;
    msdf_atlas::FontGeometry FontGeometry(&Glyphes);
    msdf_atlas::Charset Charset;
    for (char32_t Codepoint : utf8_to_utf32(Info.BaseSet))
    {
        Charset.add(Codepoint);
    }
    FontGeometry.loadCharset(Result.Font, 1.0, Charset);
    for (msdf_atlas::GlyphGeometry &Glyph : Glyphes)
    {
        Glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, /* @see edge-coloring.h */3.0, 0);
    }

    msdf_atlas::TightAtlasPacker Packer;
    Packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE); /* Also consider #setDimensions to find the best value. */
    Packer.setMinimumScale(Result.AtlasGlyphSize);
    Packer.setPixelRange(Result.PixelRange); /* Also consider setUnitRange. */
    Packer.setMiterLimit(1.0);
    Packer.setInnerPixelPadding(Info.Padding.x);
    Packer.setOuterPixelPadding(Info.Padding.y);
    Packer.pack(Glyphes.data(), Glyphes.size());
    LVec2i32 AtlasDimensions; Packer.getDimensions(AtlasDimensions.x, AtlasDimensions.y);

    msdf_atlas::GeneratorAttributes Attributes;
    msdf_atlas::ImmediateAtlasGenerator<f32, AtlasColorChannels, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, AtlasColorChannels>> Generator(AtlasDimensions.x, AtlasDimensions.y);
    Generator.setAttributes(Attributes);
    Generator.setThreadCount(4);
    Generator.generate(Glyphes.data(), Glyphes.size());

    auto Bitmap{static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(Generator.atlasStorage())};
    LByteBulkData Bulk; Bulk.Serialize(Bitmap.pixels, Bitmap.width * Bitmap.height * 3);
    Result.Atlas = LTexture2::FromMemory(
        "FontAtlas", std::move(Bulk), vk::Format::eR8G8B8Unorm, LTexture2Extent{static_cast<u32>(Bitmap.width), static_cast<u32>(Bitmap.height)},
        LTexture2::HostInfo{.Format = vk::Format::eR8G8B8A8Srgb,}
        );
    Result.Atlas->LoadToDevice(LTexture2::DeviceInfo{.DesiredMipLevels = 1,.Samples = vk::SampleCountFlagBits::e1,});
    Result.Atlas->FreeFromHost();
    this->GetFrontend().Vk_AddTextureToGlobalBindlessArray(&*Result.Atlas);
    check(Result.Atlas->IsBindless())

    for (auto const& Glyph : Glyphes)
    {
        GlyphUVs UVs;
        Glyph.getQuadPlaneBounds(UVs.PlaneBounds[0], UVs.PlaneBounds[1], UVs.PlaneBounds[2], UVs.PlaneBounds[3]);
        Glyph.getQuadAtlasBounds(UVs.AtlasBounds[0], UVs.AtlasBounds[1], UVs.AtlasBounds[2], UVs.AtlasBounds[3]);
        Result.GlyphUVsMap[Glyph.getGlyphIndex().getIndex()] = UVs;
    }

    Result.Ascender = LVec2D{FontGeometry.getMetrics().ascenderY, FontGeometry.getMetrics().descenderY};

    // stbi_write_png("Temp/Atlas.png", Bitmap.width, Bitmap.height, AtlasColorChannels, Bitmap.pixels, Bitmap.width * AtlasColorChannels);

    check(Result.Atlas->GetExtent().Width == static_cast<u32>(AtlasDimensions.x) && Result.Atlas->GetExtent().Height == static_cast<u32>(AtlasDimensions.y))
    check(Result.Atlas->IsOnHost() == false && Result.Atlas->IsOnDevice())
    this->My_Fonts.push_back(std::move(Result));
    check(Result.Atlas.get() == nullptr)

    return;
}

TArray<Jafg::LGlyphInfo> Jafg::JFontSubsystem::GetGlyphInfos(LString const& Text, f32 FontSize, LVec2F Pencil, u32 FontIndex) const noexcept
{
    auto& Font{this->My_Fonts[FontIndex]};
    check(Font.Source.empty() == false)
    check(Font.My_FT_Face && Font.Font)
    check(Font.Atlas.get())
    check(Font.GlyphUVsMap.empty() == false)

    auto AtlasExtend{Font.Atlas->GetExtentAsVec2F()};

    FT_Set_Pixel_Sizes(Font.My_FT_Face, 0, FontSize);
    hb_font_t* HBFont{hb_ft_font_create(Font.My_FT_Face, nullptr)};
    hb_buffer_t* HBBuffer{hb_buffer_create()};

    hb_buffer_add_utf8(HBBuffer, Text.c_str(), -1, 0, -1);
    hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(HBBuffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(HBBuffer, hb_language_from_string("en", -1));
    hb_shape(HBFont, HBBuffer, nullptr, 0);

    uint32_t GlyphCount;
    hb_glyph_info_t* GlyphInfos{hb_buffer_get_glyph_infos(HBBuffer, &GlyphCount)};
    hb_glyph_position_t* GlyphPositions{hb_buffer_get_glyph_positions(HBBuffer, &GlyphCount)};

    /* Move to baseline. */
    Pencil.y += FontSize * Font.Ascender.x;

    TArray<LGlyphInfo> Result; Result.reserve(GlyphCount);
    for (auto Idx{0uz}; Idx < GlyphCount; ++Idx)
    {
        uint32_t GlyphIndex{GlyphInfos[Idx].codepoint}; /* After shaping, this is the Glyph-ID. */
        LVec2F Advance{GlyphPositions[Idx].x_advance * HarfBuzzScale, GlyphPositions[Idx].y_advance * HarfBuzzScale};

        auto It{Font.GlyphUVsMap.find(GlyphIndex)};
        if (It == Font.GlyphUVsMap.end()) /* Non-printable or missing. */
        {
            Pencil += Advance;
            continue;
        }

        GlyphUVs const& GlyphUV{It->second};
        LVec2F Offset{GlyphPositions[Idx].x_offset * HarfBuzzScale, GlyphPositions[Idx].y_offset * HarfBuzzScale};
        LVec2F LeftTop{
            Pencil.x + Offset.x + static_cast<f32>(GlyphUV.PlaneBounds[0]) * FontSize,
            Pencil.y - Offset.y - static_cast<f32>(GlyphUV.PlaneBounds[3]) * FontSize,
            };
        Result.emplace_back(
            LVec4F{
                LeftTop.x,
                LeftTop.y,
                (Pencil.x + Offset.x + static_cast<f32>(GlyphUV.PlaneBounds[2]) * FontSize) - LeftTop.x,
                (Pencil.y - Offset.y - static_cast<f32>(GlyphUV.PlaneBounds[1]) * FontSize) - LeftTop.y,
                },
            LVec4F{
                static_cast<f32>(GlyphUV.AtlasBounds[0]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[3]) / AtlasExtend.y,
                static_cast<f32>(GlyphUV.AtlasBounds[2]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[1]) / AtlasExtend.y,
                },
            Font.Atlas->GetBindlessIndex(),
            UBO::BindlessTextureArray::ClampToEdgeSamplerIdx,
            (Font.PixelRange / Font.AtlasGlyphSize) * FontSize
            );

        Pencil += Advance;
        continue;
    }

    hb_buffer_destroy(HBBuffer);
    hb_font_destroy(HBFont);

    return Result;
}
