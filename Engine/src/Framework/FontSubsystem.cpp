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
#include "Engine/Engine.h"

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

static_assert(std::same_as<Jafg::Detail::MyDynamicAtlasChangeFlags, Jafg::Detail::MyDynamicAtlas::ChangeFlags>);

static_assert(std::to_underlying(Jafg::EFontAtlasChangeFlags::Identity) == std::to_underlying(Jafg::Detail::MyDynamicAtlas::NO_CHANGE));
static_assert(std::to_underlying(Jafg::EFontAtlasChangeFlags::Resized) == std::to_underlying(Jafg::Detail::MyDynamicAtlas::RESIZED));
static_assert(std::to_underlying(Jafg::EFontAtlasChangeFlags::Rearranged) == std::to_underlying(Jafg::Detail::MyDynamicAtlas::REARRANGED));

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

/* Positions are in 26.6 fixed-point (1/64px) scale to pixels. */
inline constexpr f32 HarfBuzzScale{ 1.0f / 64.0f };

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

void Jafg::LFontRenderData::Update(LViewport const& Viewport, JFontSubsystem const& Subsystem, LTextBrushBase const& Brush, LStringView Text)
{
    this->FontSize = Brush.TextScale.InStaticPoints(Viewport);
    this->bDirty = false;

    LVec2F Pencil{maths::zero_vector<LVec2F>};
    this->Collection = Subsystem.GetGlyphInfos(0, Text, this->FontSize, &Pencil);

    //# TODO: This for line gaps. LineHeight + line_gap
    this->DesiredSize = {Pencil.x, this->Collection.LineHeight * Brush.Tightening};
}

void Jafg::LFontRenderData::Render(LNodeRenderInfo const& Info, LRect2F const& Rect, LTextInfo const& TextInfo) const
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

Jafg::JFontSubsystem::MyFont::MyFont(FT_Library Library, FontCreateInfo const& Info) noexcept
{
    this->Source = Info.Source;
    check(!this->Source.empty())

    if (!is_regular_file(this->Source))
    {
        LOG_FATAL(LogFontSubsystem, "[{}]: No such file.", this->Source)
    }

    /*
     * TODO: Depending on the language the user specified we might want to give this control to the create info.
     *      E.g. in en-us 2048 is enough but if the pack is e.g. ja we might already want to increase this to 4096 or even 8192.
     *      But of course only for core fonts. If this font is used e.g. for italic, world specified immersive fonts.
     *      Then having this to allow for only ASCII is totally reasonable.
     */
    // this->DynamicAtlas = new Detail::MyDynamicAtlas{2'048};
    this->DynamicAtlas = new Detail::MyDynamicAtlas;
    this->DynamicAtlas.Value->atlasGenerator().setThreadCount(4); /* TODO: This is just an arbitrary limit. What is good here for us? */

    // TODO: Ask user preferences to clamp this.
    this->AtlasGlyphSize = Info.MaxAtlasGlyphSize;
    this->PixelRange = Info.MaxPixelRange;

    if (auto Error{FT_New_Face(Library
#if JAFG_PLATFORM_USES_UTF8
        , this->Source.native().c_str()
#else /* JAFG_PLATFORM_USES_UTF8 */
        , this->Source.string().c_str()
#endif /* !JAFG_PLATFORM_USES_UTF8 */
        , 0, &this->My_FT_Face)})
    {
        LOG_FATAL(LogFontSubsystem, "[{}]: Failed to load font face [{}].", Info.Source, Error)
    }
    check(this->My_FT_Face)

    if (auto Error{FT_Set_Pixel_Sizes(this->My_FT_Face, 0, static_cast<FT_UInt>(this->AtlasGlyphSize))})
    {
        LOG_FATAL(LogFontSubsystem, "[{}]: Failed to set pixel size [{}].", Info.Source, Error)
    }

    if (!Info.Tags.empty())
    {
        TArray<std::size_t> UsedTags;
        UsedTags.reserve(Info.Tags.size());

        FT_MM_Var* Variations{};
        FT_Get_MM_Var(this->My_FT_Face, &Variations);
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
                LOG_WARNING(LogFontSubsystem, "[{}]: Tag [{}] was not consumed.", Info.Source, Info.Tags[Idx].first)
            }
        }

        if (auto Error{FT_Set_Var_Design_Coordinates(this->My_FT_Face, 2, Coords)}; Error)
        {
            LOG_FATAL(LogFontSubsystem, "[{}]: Failed to set font variation design coordinates [{}].", Info.Source, Error)
        }
    }

    if (this->My_FT_Face->family_name)
    {
        this->FamilyName = LString{this->My_FT_Face->family_name};
    }
    else
    {
        check(!this->FamilyName)
    }
    if (this->My_FT_Face->style_name)
    {
        this->StyleName = LString{this->My_FT_Face->style_name};
    }
    else
    {
        check(!this->StyleName)
    }

    this->Font = msdfgen::adoptFreetypeFont(this->My_FT_Face);
    this->HbFont = hb_ft_font_create(this->My_FT_Face, nullptr);

    msdfgen::FontMetrics Metrics;
    if (!msdfgen::getFontMetrics(Metrics, this->Font, msdfgen::FONT_SCALING_NONE))
    {
        LOG_FATAL(LogFontSubsystem, "[{}]: Failed to get font metrics.", Info.Source)
    }
    if (Metrics.emSize <= 0.0)
    {
        /* 2048 are the default font units per em. */
        Metrics.emSize = 2048.;
    }

    this->EmSize = static_cast<f32>(Metrics.emSize);
    this->Ascender = static_cast<f32>(Metrics.ascenderY);
    this->Descender = static_cast<f32>(Metrics.descenderY);
    this->Height = static_cast<f32>(Metrics.lineHeight);
    this->MaxAdvanceWidth = static_cast<f32>(this->My_FT_Face->max_advance_width) * HarfBuzzScale * this->AtlasGlyphSize; // TODO: Correct scaling?
    this->MaxAdvanceHeight = static_cast<f32>(this->My_FT_Face->max_advance_height) * HarfBuzzScale * this->AtlasGlyphSize;
    this->UnderlinePosition = static_cast<f32>(Metrics.underlineY);
    this->UnderlineThickness = static_cast<f32>(Metrics.underlineThickness);

    auto Pencil{maths::zero_vector<LVec2F>};
    (void)this->Shape(Info.BaseSet, 12.0f, &Pencil);

    LOG_TRACE(LogFontSubsystem
        , "[{}]: Font loaded. "
          "FamilyName: {}, StyleName: {}, "
          "Ascender: {}, Descender: {}, Height: {}, MaxAdvanceWidth: {}, MaxAdvanceHeight: {}, UnderlinePosition: {}, UnderlineThickness: {}"
        , Info.Source
        , this->FamilyName ? *this->FamilyName : "<null>", this->StyleName ? *this->StyleName : "<null>"
        , this->Ascender, this->Descender, this->Height, this->MaxAdvanceWidth, this->MaxAdvanceHeight, this->UnderlinePosition, this->UnderlineThickness
        )
}

Jafg::JFontSubsystem::MyFont& Jafg::JFontSubsystem::MyFont::operator=(MyFont&& Rhs) noexcept
{
    this->Orphan();

    this->Source = std::move(Rhs.Source);

    this->AtlasGlyphSize = std::exchange(Rhs.AtlasGlyphSize, 0.0f);
    this->PixelRange = std::exchange(Rhs.PixelRange, 0.0f);
    this->AngleThreshold = std::exchange(Rhs.AngleThreshold, 3.0f);

    this->FamilyName = std::exchange(Rhs.FamilyName, std::nullopt);
    this->StyleName = std::exchange(Rhs.StyleName, std::nullopt);

    this->EmSize = std::exchange(Rhs.EmSize, 0.0f);
    this->Ascender = std::exchange(Rhs.Ascender, 0.0f);
    this->Descender = std::exchange(Rhs.Descender, 0.0f);
    this->Height = std::exchange(Rhs.Height, 0.0f);
    this->MaxAdvanceWidth = std::exchange(Rhs.MaxAdvanceWidth, 0.0f);
    this->MaxAdvanceHeight = std::exchange(Rhs.MaxAdvanceHeight, 0.0f);
    this->UnderlinePosition = std::exchange(Rhs.UnderlinePosition, 0.0f);
    this->UnderlineThickness = std::exchange(Rhs.UnderlineThickness, 0.0f);

    this->Glyphs = std::move(Rhs.Glyphs);

    this->AtlasTexture = std::move(Rhs.AtlasTexture);
    this->DynamicAtlas = std::move(Rhs.DynamicAtlas);

    this->My_FT_Face = std::exchange(Rhs.My_FT_Face, nullptr);
    this->Font = std::exchange(Rhs.Font, nullptr);
    this->HbFont = std::exchange(Rhs.HbFont, nullptr);

    return *this;
}

Jafg::LGlyphCollection Jafg::JFontSubsystem::MyFont::Shape(LStringView Text, f32 FontSize, LVec2F* Pencil)
{
    check(Tasks::IsOnMasterThread())

    check(Pencil)

    check(!this->Source.empty())
    check(this->My_FT_Face && this->Font)

    f32 GeometryScale{this->GeometryScaleFor(FontSize)};
    f32 FreetypeScale{this->FreetypeScaleFor(FontSize)};

    LGlyphCollection Result{
        .Ascender = this->My_FT_Face->size->metrics.ascender * HarfBuzzScale * FreetypeScale,
        .Descender = this->My_FT_Face->size->metrics.descender * HarfBuzzScale * FreetypeScale,
        .PencilBegin = *Pencil,
#if JAFG_DO_CHECKS
        ._check_UsedString = LString{Text},
#endif /* JAFG_DO_CHECKS */
        };
    // TODO: Is this correct? Or should we use the absolute descender value?
    Result.LineHeight = Result.Ascender - Result.Descender;

    hb_buffer_t* Buffer{hb_buffer_create()};
    algo::raii_leave _{[Buffer] noexcept { hb_buffer_destroy(Buffer); }};

    hb_buffer_add_utf8(Buffer, Text.data(), static_cast<int>(Text.size()), 0, static_cast<int>(Text.size()));
    hb_buffer_set_direction(Buffer, HB_DIRECTION_LTR); // TODO: Make option
    hb_buffer_set_script(Buffer, HB_SCRIPT_LATIN); // TODO: Make option
    hb_buffer_set_language(Buffer, hb_language_from_string("en", -1)); // TODO: Make option
    // hb_feature_t HbFeatures[1]; hb_feature_from_string("liga=0", -1, &HbFeatures[0]); // TODO: Allows for complex basic charset characters. This should be an option
    // hb_shape(this->HbFont, Buffer, HbFeatures, 1);
    hb_shape(this->HbFont, Buffer, nullptr, 0);

    u32 GlyphCount;
    hb_glyph_info_t* Infos{hb_buffer_get_glyph_infos(Buffer, &GlyphCount)};
    hb_glyph_position_t* Positions{hb_buffer_get_glyph_positions(Buffer, &GlyphCount)};

    std::unordered_set<LGlyphIndex> GlyphIndices; GlyphIndices.reserve(GlyphCount);
    for (auto Idx{0uz}; Idx < GlyphCount; ++Idx)
    {
        GlyphIndices.insert(Infos[Idx].codepoint); /* After shaping this is the glyph id inside the font. */
    }
    auto ChangeFlags{this->AddGlyphsByIndex(GlyphIndices)};
    if (ChangeFlags != EFontAtlasChangeFlags::Identity)
    {
        if (ChangeFlags & (EFontAtlasChangeFlags::Resized |EFontAtlasChangeFlags::Rearranged))
        {
            // We need to update everything this includes, the unordered set with its values.
        }
        if constexpr (LogFontSubsystem.CompilesFor<ELogVerbosity::Trace>)
        {
            LVec2i32 Extent{this->GetDynamicAtlasDimensions()};
            LOG_TRACE(LogFontSubsystem, "[{}]: Font atlas changed to [{}x{}]. Uploading to device.", this->Source, Extent.x, Extent.y)
        }
        this->UploadToDevice();
        ++this->Generation;
    }

    /* Move to baseline. */
    Pencil->y += this->Ascender * GeometryScale;

    check(this->AtlasTexture.get())
    check(!this->Glyphs.empty())
    auto AtlasExtend{this->AtlasTexture->GetExtentAsVec2F()};
    Result.GlyphInfos.reserve(GlyphCount);
    for (auto Idx{0uz}; Idx < GlyphCount; ++Idx)
    {
        LGlyphIndex GlyphIndex{Infos[Idx].codepoint}; /* After shaping, this is the Glyph-ID. */

        auto It{this->Glyphs.find(GlyphIndex)};
        if (It == this->Glyphs.end()) /* Non-printable or missing. */
        {
            checkNoEntry()
            // TODO: Add like a question mark?
            // *Pencil += Advance;
            // continue;
        }

        GlyphUVs const& GlyphUV{It->second};
        Result.GlyphInfos.push_back({
            .Rect = {
                Pencil->x
                    + static_cast<f32>(Positions[Idx].x_offset) * HarfBuzzScale * FreetypeScale
                    + static_cast<f32>(GlyphUV.PlaneBounds[0]) * FontSize,
                Pencil->y
                    - static_cast<f32>(Positions[Idx].y_offset) * HarfBuzzScale * FreetypeScale
                    - static_cast<f32>(GlyphUV.PlaneBounds[3]) * FontSize,
                static_cast<f32>(GlyphUV.PlaneBounds[2] - GlyphUV.PlaneBounds[0]) * FontSize,
                static_cast<f32>(GlyphUV.PlaneBounds[3] - GlyphUV.PlaneBounds[1]) * FontSize,
                },
            .TexCoordRect = {
                static_cast<f32>(GlyphUV.AtlasBounds[0]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[3]) / AtlasExtend.y,
                static_cast<f32>(GlyphUV.AtlasBounds[2]) / AtlasExtend.x,
                static_cast<f32>(GlyphUV.AtlasBounds[1]) / AtlasExtend.y,
                },
            .BindlessTextureIndex = this->AtlasTexture->GetBindlessIndex(),
            .SamplerIndex = std::to_underlying(UBO::Bindless::Sampler::NearestClampToEdgeSamplerIdx),
            /* TODO: Scale with Retina/HighDPI. */
            .MsdfPixelRange = algo::max(this->PixelRange * (FontSize / this->AtlasGlyphSize), 1.0f),
            .Cluster = Infos[Idx].cluster,
            .Pencil = *Pencil,
            });

        *Pencil += LVec2F{
            static_cast<f32>(Positions[Idx].x_advance) * HarfBuzzScale * FreetypeScale,
            static_cast<f32>(Positions[Idx].y_advance) * HarfBuzzScale * FreetypeScale,
            };
    }

    Result.PencilEnd = *Pencil;

    return Result;
}

Jafg::EFontAtlasChange Jafg::JFontSubsystem::MyFont::AddGlyphsByIndex(std::unordered_set<LGlyphIndex> const& Glyphs)
{
    check(*this->DynamicAtlas)

    TArray<msdf_atlas::GlyphGeometry> Geometries;
    Geometries.reserve(Glyphs.size());
    for (LGlyphIndex Glyph: Glyphs)
    {
        if (!this->Glyphs.contains(Glyph))
        {
            auto& Geometry{Geometries.emplace_back()};
            if (!Geometry.load(this->Font, 1.0 / this->EmSize, msdfgen::GlyphIndex{Glyph}, true))
            {
                LOG_FATAL(LogFontSubsystem, "[{}]: Failed to load glyph geometry [{}].", this->Source, Glyph)
            }
            Geometry.edgeColoring(/* TODO: make option */ &msdfgen::edgeColoringInkTrap, static_cast<f64>(this->AngleThreshold), 0);
            Geometry.wrapBox(this->AtlasGlyphSize, this->PixelRange / this->AtlasGlyphSize, 1.0 /* TODO: make option */);
        }
    }

    EFontAtlasChange Result{};
    if (!Geometries.empty())
    {
        if constexpr (LogFontSubsystem.CompilesFor<ELogVerbosity::Trace>) for (auto& Geometry: Geometries)
        {
            int x, y; Geometry.getBoxSize(x, y);
            LOG_TRACE(LogFontSubsystem, "[{}]: Adding glyph [{}]: {}x{}"
                , this->Source, Geometry.getGlyphIndex().getIndex(), x, y
                )
        }

        Result |= EFontAtlasChangeFlags::Outdated
               |  static_cast<EFontAtlasChangeFlags>(this->DynamicAtlas.get()->add(Geometries.data(), static_cast<int>(Geometries.size())));

        for (auto& Geometry: Geometries)
        {
            GlyphUVs UVs;
            Geometry.getQuadPlaneBounds(UVs.PlaneBounds[0], UVs.PlaneBounds[1], UVs.PlaneBounds[2], UVs.PlaneBounds[3]);
            Geometry.getQuadAtlasBounds(UVs.AtlasBounds[0], UVs.AtlasBounds[1], UVs.AtlasBounds[2], UVs.AtlasBounds[3]);
            this->Glyphs[Geometry.getGlyphIndex().getIndex()] = UVs;
        }
    }
    return Result;
}

void Jafg::JFontSubsystem::MyFont::UploadToDevice()
{
    check(*this->DynamicAtlas)

    static_assert(sizeof(algo::byte_bulk::value_type) == sizeof(msdf_atlas::byte));
    auto Bitmap{static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(this->DynamicAtlas.Value->atlasGenerator().atlasStorage())};
    algo::byte_bulk Bulk;
    Bulk.serialize(reinterpret_cast<algo::byte_bulk::value_type const*>(Bitmap.pixels), static_cast<std::size_t>(Bitmap.width * Bitmap.height * 3));

    //# TODO: Make this a program arg.
    auto AtlasDimensions{this->GetDynamicAtlasDimensions()};
    stbi_write_png("Temp/Atlas.png", Bitmap.width, Bitmap.height, Detail::FontAtlasColorChannels, Bitmap.pixels, Bitmap.width * Detail::FontAtlasColorChannels);

    this->AtlasTexture = LTexture2::FromMemory(
        "FontAtlas", std::move(Bulk), vk::Format::eR8G8B8Unorm,
        {static_cast<rhi::extent2::domain_type>(Bitmap.width), static_cast<rhi::extent2::domain_type>(Bitmap.height)},
        LTexture2::HostInfo{.Format = vk::Format::eR8G8B8A8Unorm/* TODO: is this correct? eR8G8B8A8Srgb looks very different */,}
        );
    this->AtlasTexture->LoadToDevice(LTexture2::DeviceInfo{.DesiredMipLevels = 1,.Samples = vk::SampleCountFlagBits::e1,});
    this->AtlasTexture->FreeFromHost();
    Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->AddTextureToGlobalBindlessArray(&*this->AtlasTexture);
    check(this->AtlasTexture->IsBindless())
    check(!this->AtlasTexture->IsOnHost() && this->AtlasTexture->IsOnDevice())
    checkCode
    (

        check(this->AtlasTexture->GetExtent().width  == static_cast<u32>(AtlasDimensions.x))
        check(this->AtlasTexture->GetExtent().height == static_cast<u32>(AtlasDimensions.y))
    )
}

LVec2i32 Jafg::JFontSubsystem::MyFont::GetDynamicAtlasDimensions() const noexcept
{
    msdfgen::BitmapConstRef<msdf_atlas::byte, 3> Bitmap{this->DynamicAtlas.Value->atlasGenerator().atlasStorage()};
    return {Bitmap.width, Bitmap.height};
}

void Jafg::JFontSubsystem::MyFont::Orphan() noexcept
{
    delete *this->DynamicAtlas;
    hb_font_destroy(this->HbFont);
    msdfgen::destroyFont(this->Font);
    FT_Done_Face(this->My_FT_Face);
}

void Jafg::JFontSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<JTextureSubsystem>(this);
    Collection.InitializeDependency<JMaterialSubsystem>(this);

    check(this->My_FT_Library == nullptr)
    if (FT_Init_FreeType(&this->My_FT_Library))
    {
        LOG_FATAL(LogFontSubsystem, "Failed to initialize FreeType.")
    }
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
}

void Jafg::JFontSubsystem::TearDown()
{
    Super::TearDown();

    this->My_Fonts.clear();

    if (this->My_FT_Library)
    {
        FT_Done_FreeType(this->My_FT_Library);
    }
}

u32 Jafg::JFontSubsystem::ReloadFont(FontCreateInfo const& Info) noexcept
{
    LOG_TRACE(LogFontSubsystem, "[{}]: Loading font.", Info.Source)
    check(Tasks::IsOnMasterThread()) //# TODO: Allow offloading.

    auto It{algo::find(this->My_Fonts, Info.Source, &MyFont::Source)};
    if (It == this->My_Fonts.end())
    {
        if (this->My_Fonts.size() >= JFontSubsystem::MaxAllowedFonts)
        {
            LOG_FATAL(LogFontSubsystem
                , "Cannot load [{}] because the maximum allowed font count [{}] has been reached."
                , Info.Source, JFontSubsystem::MaxAllowedFonts
                )
        }
        this->My_Fonts.emplace_back(this->My_FT_Library, Info);
        It = algo::prev(this->My_Fonts.end());
    }
    else
    {
        *It = MyFont{this->My_FT_Library, Info};
    }

    return static_cast<u32>(algo::distance(this->My_Fonts, It));
}
