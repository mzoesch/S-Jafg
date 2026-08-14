// Copyright mzoesch. All rights reserved.

#include "Framework/FontSubsystem.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Rhi/RendererCore.h"
#include "Rhi/VisualInstance.h"
#include "User/UserPreferences.h"
#include "Nodes/Font.h"
#include "Rhi/NodeRenderInfo.h"

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wint-in-bool-context"
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif /* JAFG_WITH_GCC */
    #include <hb.h>
    #include <hb-ft.h>
    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include FT_MULTIPLE_MASTERS_H
    #include <msdfgen.h>
    #include <msdf-atlas-gen/msdf-atlas-gen.h>
    #include <stb_image_write.h>
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */

static_assert(sizeof(signed long) == sizeof(FT_Fixed));
static_assert(std::numeric_limits<signed long>::max() == std::numeric_limits<FT_Fixed>::max());
static_assert(std::numeric_limits<signed long>::lowest() == std::numeric_limits<FT_Fixed>::lowest());

namespace
{

f32 TextBoxInStaticPointsImpl(Jafg::ETextScale TextScale, Jafg::EApplicationScale Scale) noexcept
{
    auto const& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};

    switch (Scale)
    {
    case Jafg::EApplicationScale::Single:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeSingle; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeSingle; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeSingle; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeSingle; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeSingle; }
        }
        std::unreachable();
    }
    case Jafg::EApplicationScale::Double:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeDouble; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeDouble; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeDouble; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeDouble; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeDouble; }
        }
        std::unreachable();
    }
    case Jafg::EApplicationScale::Triple:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeTriple; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeTriple; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeTriple; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeTriple; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeTriple; }
        }
        std::unreachable();
    }
    case Jafg::EApplicationScale::Auto: std::unreachable();
    }

    std::unreachable();
}

inline constexpr u32 AtlasColorChannels{ 3 };
/* Positions are in 26.6 fixed-point (1/64px) scale to pixels. */
inline constexpr f32 HarfBuzzScale{ 1.0f / 64.0f };

Lu32String utf8_to_utf32(LString const& Utf8)
{
    Lu32String Result;

    auto Idx{0uz};
    while (Idx < Utf8.size())
    {
        uint8_t C{static_cast<uint8_t>(Utf8[Idx])};
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
        if (Idx + Bytes > Utf8.size())
        {
            break;
        }

        for (auto SubIdx{1uz}; SubIdx < Bytes; ++SubIdx)
        {
            uint8_t Cont{static_cast<uint8_t>(Utf8[Idx + SubIdx])};
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

f32 Jafg::LTextScale::InStaticPointsImpl(LViewport const& Viewport, ETextScale TextScale) noexcept
{
    EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    if (EApplicationScale UserMaxScale{*GetSingleton<JUserPreferences>().ApplicationScaleMode}; UserMaxScale != EApplicationScale::Auto)
    {
        Scale = EApplicationScale{maths::min(std::to_underlying(Scale), std::to_underlying(UserMaxScale))};
    }
    return ::TextBoxInStaticPointsImpl(TextScale, Scale);
}

void Jafg::LRenderData::Update(LViewport const& Viewport, JFontSubsystem const& Subsystem, LTextBrushBase const& Brush, LStringView Text)
{
    this->FontSize = Brush.TextScale.InStaticPoints(Viewport);
    this->bDirty = false;

    LVec2F Pencil{maths::zero_vector<LVec2F>};
    this->Collection = Subsystem.GetGlyphInfos(Text, this->FontSize, &Pencil, 0);

    //# TODO: This for line gaps. LineHeight + line_gap
    this->DesiredSize = {Pencil.x, this->Collection.LineHeight * Brush.Tightening};

    return;
}

void Jafg::LRenderData::Render(LNodeRenderInfo const& Info, LRect2F const& Rect, LTextInfo const& TextInfo) const
{
    auto AddGlyph{[&](LGlyphCollection::Info const& Glyph)
    {
        if (TextInfo.bFadeLeftOverdraw)
        {
            if (Glyph.Pencil.x + TextInfo.LeftThrust < 0.0f)
            {
                f32 Missed{Glyph.Pencil.x + Glyph.Rect.z + TextInfo.LeftThrust};
                if (Missed > 0.0f)
                {
                    Info.AddInstance({
                        .Rect = {
                            .offset = {Rect.offset.x + Glyph.Rect.x + (Glyph.Rect.z - Missed), Rect.offset.y + Glyph.Rect.y},
                            .extent = {Missed, Glyph.Rect.w}
                            },
                        .TexCoordRect = {
                            Glyph.TexCoordRect.x + ((Glyph.TexCoordRect.z - Glyph.TexCoordRect.x) * ((Glyph.Rect.z - Missed) / Glyph.Rect.z)),
                            Glyph.TexCoordRect.y,
                            Glyph.TexCoordRect.z,
                            Glyph.TexCoordRect.w
                            },
                        .Tint = TextInfo.Tint,
                        .OutlineTint = TextInfo.OutlineTint,
                        .OutlineThickness = TextInfo.OutlineThickness,
                        .TextureIndex = Glyph.BindlessTextureIndex,
                        .SamplerIndex = Glyph.SamplerIndex,
                        .MsdfPixelRange = Glyph.MsdfPixelRange,
                        });
                }
                return;
            }
        }

        Info.AddInstance({
            .Rect = {{Rect.offset.x + Glyph.Rect.x, Rect.offset.y + Glyph.Rect.y}, {Glyph.Rect.z, Glyph.Rect.w}},
            .TexCoordRect = Glyph.TexCoordRect,
            .Tint = TextInfo.Tint,
            .OutlineTint = TextInfo.OutlineTint,
            .OutlineThickness = TextInfo.OutlineThickness,
            .TextureIndex = Glyph.BindlessTextureIndex,
            .SamplerIndex = Glyph.SamplerIndex,
            .MsdfPixelRange = Glyph.MsdfPixelRange,
            });

        return;
    }};

    switch (TextInfo.Cutoff)
    {
    case ETextCutoff::Cutoff:
    {
        for (LGlyphCollection::Info const& Glyph: this->Collection)
        {
            if (Glyph.Pencil.x + Glyph.Rect.z > Rect.extent.x)
            {
                break;
            }
            AddGlyph(Glyph);
        }
        break;
    }
    case ETextCutoff::Fade:
    {
        for (LGlyphCollection::Info const& Glyph: this->Collection)
        {
            if (Glyph.Pencil.x + Glyph.Rect.z > Rect.extent.x)
            {
                break;
            }
            AddGlyph(Glyph);
        }
        break;
    }
    case ETextCutoff::NoBounds:
    {
        for (LGlyphCollection::Info const& Glyph: this->Collection)
        {
            AddGlyph(Glyph);
        }
        break;
    }
    default: std::unreachable();
    }

#if !JAFG_IN_SHIPPING
    auto& Prefs{GetSingleton<JUserPreferences>()};
    if (Prefs.bHighlightFontRects)
    {
        Info.AddInstance({
            .Rect = Rect,
            .Tint = Prefs.FontRectHighlightColor,
            });
    }
#endif /* !JAFG_IN_SHIPPING */

    return;
}

void Jafg::JFontSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<JTextureSubsystem>(this);
    Collection.InitializeDependency<JMaterialSubsystem>(this);

    check(this->My_FT_Library == nullptr)
    FT_Init_FreeType(&this->My_FT_Library);
    check(this->My_FT_Library)

    check(this->My_Fonts.empty())
    (void)this->ReloadFont({
        .Source="Content/Fonts/NotoSans-VariableFont_wdth,wght.ttf",
        .Tags={
            {"Weight", 700},
            {"Width", 100},
            },
        });
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

u32 Jafg::JFontSubsystem::ReloadFont(FontCreateInfo const& Info) noexcept
{
    if (this->My_Fonts.size() >= JFontSubsystem::MaxAllowedFonts)
    {
        LOG_FATAL(LogFontSubsystem
            , "Cannot load [{}] because the maximum allowed font count [{}] has been reached."
            , Info.Source, JFontSubsystem::MaxAllowedFonts
            )
    }

    LOG_TRACE(LogFontSubsystem, "[{}]: Loading font.", Info.Source)

    if (is_regular_file(Info.Source) == false)
    {
        LOG_FATAL(LogFontSubsystem
            , "[{}]: No such file."
            , Info.Source
            )
    }

    //# TODO: Fix existing font and update it instead.
    //#       Also msdf-gen supports on the fly atlas updates. We need to support this as well.
    MyFont Result; Result.Source = Info.Source;

    // TODO: Ask user preferences to clamp this.
    Result.AtlasGlyphSize = Info.MaxAtlasGlyphSize;
    Result.PixelRange = Info.MaxPixelRange;

#if JAFG_PLATFORM_USES_UTF8
    FT_New_Face(this->My_FT_Library, Result.Source.native().c_str(), 0, &Result.My_FT_Face);
#else /* JAFG_PLATFORM_USES_UTF8 */
    FT_New_Face(this->My_FT_Library, Result.Source.string().c_str(), 0, &Result.My_FT_Face);
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    check(Result.My_FT_Face)

    if (!Info.Tags.empty())
    {
        TArray<std::size_t> UsedTags;
        UsedTags.reserve(Info.Tags.size());

        FT_MM_Var* Variations{};
        FT_Get_MM_Var(Result.My_FT_Face, &Variations);
        check(Variations)
        FT_Fixed Coords[2];
        for (FT_UInt Idx{}; Idx < Variations->num_axis; ++Idx)
        {
            FT_Var_Axis const& Axis{Variations->axis[Idx]};
            LOG_TRACE(LogFontSubsystem, "[{}]: {}: min({}), max({}); value: {}",
                Info.Source, Axis.name
                , static_cast<f32>(Axis.minimum) / 65536.0f, static_cast<f32>(Axis.maximum) / 65536.0f
                , static_cast<f32>(Axis.def) / 65536.0f
                )

            if (auto It{algo::find(Info.Tags, LStringView{Axis.name}, algo::pair_first)}; It != Info.Tags.end())
            {
                signed long Value{Info.Tags[UsedTags.emplace_back(static_cast<std::size_t>(algo::distance(Info.Tags, It)))].second};
                if (   static_cast<f32>(Value) < (static_cast<f32>(Axis.minimum) / 65536.0f)
                    || static_cast<f32>(Value) > (static_cast<f32>(Axis.maximum) / 65536.0f))
                {
                    LOG_FATAL(LogFontSubsystem
                        , "[{}]: Tag [{}] value [{}] is out of range [{},{}]."
                        , Info.Source, Axis.name
                        , Value
                        , static_cast<f32>(Axis.minimum) / 65536.0f, static_cast<f32>(Axis.maximum) / 65536.0f
                        )
                }
                Coords[Idx] = Value * 65536;
            }
            else
            {
                LOG_WARNING(LogFontSubsystem
                    , "[{}]: Tag [{}] was not provided. Using default value [{}]."
                    , Info.Source, Axis.name, static_cast<f32>(Axis.def) / 65536.0f
                    )
                Coords[Idx] = Axis.def;
            }
        }

        for (auto Idx{0uz}; Idx < Info.Tags.size(); ++Idx)
        {
            if (!algo::contains(UsedTags, Idx))
            {
                LOG_WARNING(LogFontSubsystem
                    , "[{}]: Tag [{}] was not consumed."
                    , Info.Source, Info.Tags[Idx].first
                    )
            }
        }

        if (auto Error{FT_Set_Var_Design_Coordinates(Result.My_FT_Face, 2, Coords)}; Error)
        {
            LOG_FATAL(LogFontSubsystem
                , "[{}]: Failed to set font variation desgin coordinates [{}]."
                , Info.Source, Error
                )
        }
    }

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
    Packer.pack(Glyphes.data(), static_cast<int>(Glyphes.size()));
    LVec2i32 AtlasDimensions; Packer.getDimensions(AtlasDimensions.x, AtlasDimensions.y);

    msdf_atlas::GeneratorAttributes Attributes;
    msdf_atlas::ImmediateAtlasGenerator<
        f32, AtlasColorChannels, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, AtlasColorChannels>>
        Generator(AtlasDimensions.x, AtlasDimensions.y);
    Generator.setAttributes(Attributes);
    Generator.setThreadCount(4);
    Generator.generate(Glyphes.data(), static_cast<int>(Glyphes.size()));

    auto Bitmap{static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(Generator.atlasStorage())};
    static_assert(sizeof(algo::byte_bulk::value_type) == sizeof(msdf_atlas::byte));
    algo::byte_bulk Bulk; Bulk.serialize(reinterpret_cast<algo::byte_bulk::value_type const*>(Bitmap.pixels), static_cast<std::size_t>(Bitmap.width * Bitmap.height * 3));
    Result.Atlas = LTexture2::FromMemory(
        "FontAtlas", std::move(Bulk), vk::Format::eR8G8B8Unorm,
        {static_cast<rhi::extent2::domain_type>(Bitmap.width), static_cast<rhi::extent2::domain_type>(Bitmap.height)},
        LTexture2::HostInfo{.Format = vk::Format::eR8G8B8A8Srgb,}
        );
    Result.Atlas->LoadToDevice(LTexture2::DeviceInfo{.DesiredMipLevels = 1,.Samples = vk::SampleCountFlagBits::e1,});
    Result.Atlas->FreeFromHost();
    this->GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->AddTextureToGlobalBindlessArray(&*Result.Atlas);
    check(Result.Atlas->IsBindless())

    for (auto const& Glyph : Glyphes)
    {
        GlyphUVs UVs;
        Glyph.getQuadPlaneBounds(UVs.PlaneBounds[0], UVs.PlaneBounds[1], UVs.PlaneBounds[2], UVs.PlaneBounds[3]);
        Glyph.getQuadAtlasBounds(UVs.AtlasBounds[0], UVs.AtlasBounds[1], UVs.AtlasBounds[2], UVs.AtlasBounds[3]);
        Result.GlyphUVsMap[Glyph.getGlyphIndex().getIndex()] = UVs;
    }

    Result.Ascender = FontGeometry.getMetrics().ascenderY;
    Result.Descender = FontGeometry.getMetrics().descenderY;
    Result.LineHeight = FontGeometry.getMetrics().lineHeight;

    //# TODO: Make this a program arg.
    // stbi_write_png("Temp/Atlas.png", Bitmap.width, Bitmap.height, AtlasColorChannels, Bitmap.pixels, Bitmap.width * AtlasColorChannels);

    check(Result.Atlas->GetExtent().width == static_cast<u32>(AtlasDimensions.x) && Result.Atlas->GetExtent().height == static_cast<u32>(AtlasDimensions.y))
    check(Result.Atlas->IsOnHost() == false && Result.Atlas->IsOnDevice())
    this->My_Fonts.push_back(std::move(Result));
    check(Result.Atlas.get() == nullptr)

    return static_cast<u32>(static_cast<i64>(this->My_Fonts.size()) - 1);
}

Jafg::LGlyphCollection Jafg::JFontSubsystem::GetGlyphInfos(LStringView Text, f32 FontSize, LVec2F* Pencil, u32 FontIndex) const noexcept
{
    check(Pencil)

    auto& Font{this->My_Fonts[FontIndex]};
    check(Font.Source.empty() == false)
    check(Font.My_FT_Face && Font.Font)
    check(Font.Atlas.get())
    check(Font.GlyphUVsMap.empty() == false)

    FT_Set_Pixel_Sizes(Font.My_FT_Face, 0, static_cast<FT_UInt>(FontSize));

    LGlyphCollection Result{
        .Ascender = Font.My_FT_Face->size->metrics.ascender * HarfBuzzScale,
        .Descender = Font.My_FT_Face->size->metrics.descender * HarfBuzzScale,
        .PencilBegin = *Pencil,
        ._check_UsedString = LString{Text},
        };
    // TODO: Is this correct? Or should we use the absolute descender value?
    Result.LineHeight = Result.Ascender - Result.Descender;

    hb_font_t* HBFont{hb_ft_font_create(Font.My_FT_Face, nullptr)};
    hb_buffer_t* HBBuffer{hb_buffer_create()};

    hb_buffer_add_utf8(HBBuffer, Text.data(), static_cast<int>(Text.size()), 0, static_cast<int>(Text.size()));
    hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(HBBuffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(HBBuffer, hb_language_from_string("en", -1));

    // TODO: When we can dynamically update our atlas disable this / make it an user option.
    hb_feature_t HBFeatures[1];
    hb_feature_from_string("liga=0", -1, &HBFeatures[0]);

    hb_shape(HBFont, HBBuffer, HBFeatures, 1);

    uint32_t GlyphCount;
    hb_glyph_info_t* GlyphInfos{hb_buffer_get_glyph_infos(HBBuffer, &GlyphCount)};
    hb_glyph_position_t* GlyphPositions{hb_buffer_get_glyph_positions(HBBuffer, &GlyphCount)};

    /* Move to baseline. */
    Pencil->y += FontSize * static_cast<f32>(Font.Ascender);

    auto AtlasExtend{Font.Atlas->GetExtentAsVec2F()};
    Result.GlyphInfos.reserve(GlyphCount);
    for (auto Idx{0uz}; Idx < GlyphCount; ++Idx)
    {
        uint32_t GlyphIndex{GlyphInfos[Idx].codepoint}; /* After shaping, this is the Glyph-ID. */
        LVec2F Advance{ static_cast<f32>(GlyphPositions[Idx].x_advance) * HarfBuzzScale, static_cast<f32>(GlyphPositions[Idx].y_advance) * HarfBuzzScale};

        auto It{Font.GlyphUVsMap.find(GlyphIndex)};
        if (It == Font.GlyphUVsMap.end()) /* Non-printable or missing. */
        {
            *Pencil += Advance;
            continue;
        }

        GlyphUVs const& GlyphUV{It->second};
        LVec2F Offset{ static_cast<f32>(GlyphPositions[Idx].x_offset) * HarfBuzzScale, static_cast<f32>(GlyphPositions[Idx].y_offset) * HarfBuzzScale};
        LVec2F LeftTop{
            Pencil->x + Offset.x + static_cast<f32>(GlyphUV.PlaneBounds[0]) * FontSize,
            Pencil->y - Offset.y - static_cast<f32>(GlyphUV.PlaneBounds[3]) * FontSize,
            };
        Result.GlyphInfos.emplace_back(
            LVec4F{
                LeftTop.x,
                LeftTop.y,
                // (Pencil->x + Offset.x + static_cast<f32>(GlyphUV.PlaneBounds[2]) * FontSize) - LeftTop.x,
                // (Pencil->y - Offset.y - static_cast<f32>(GlyphUV.PlaneBounds[1]) * FontSize) - LeftTop.y,
                static_cast<f32>(GlyphUV.PlaneBounds[2] - GlyphUV.PlaneBounds[0]) * FontSize,
                static_cast<f32>(GlyphUV.PlaneBounds[3] - GlyphUV.PlaneBounds[1]) * FontSize,
                },
            LVec4F{
                static_cast<f32>(GlyphUV.AtlasBounds[0]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[3]) / AtlasExtend.y,
                static_cast<f32>(GlyphUV.AtlasBounds[2]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[1]) / AtlasExtend.y,
                },
            Font.Atlas->GetBindlessIndex(),
            std::to_underlying(UBO::Bindless::Sampler::NearestClampToEdgeSamplerIdx),
            (Font.PixelRange / Font.AtlasGlyphSize) * FontSize,
            GlyphInfos[Idx].cluster,
            *Pencil
            );

        *Pencil += Advance;
        continue;
    }

    hb_buffer_destroy(HBBuffer);
    hb_font_destroy(HBFont);

    Result.PencilEnd = *Pencil;

    return Result;
}
