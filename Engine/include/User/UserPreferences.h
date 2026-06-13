// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "User/UserPreferencesForward.h"
#include "Rhi/Material.h"
#include "Nodes/ButtonBase.h"
#include "Nodes/Box.h"
#include "Nodes/TextBox.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

class WEditor;

DECLARE_JAFG_CLASS(EJxxClassBits::Config, EJxxClassBits::Singleton)
class JUserPreferences final : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JUserPreferences)

public:

    enum { UnlimitedTps = 0 };

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TClampedPreference<f32> MasterVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> MusicVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> MiscVolume{ 1.0f, 0.0f, 1.0f };
    CLASS_FIELD(Config)
    TClampedPreference<f32> VoiceVolume{ 1.0f, 0.0f, 1.0f };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    //# System dependent. But should generally stay consistent.
    CLASS_FIELD(Config)
    TPreference<LString> PreferredPhysicalDevice;
    //# System dependent. But should generally stay consistent.
    CLASS_FIELD(Config)
    TPreference<LString> PreferredMonitor;
    //# Preferred bits per pixel. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<LVec3i32> PreferredBpp{ maths::zero_vector<LVec3i32> };
    //# Preferred resolution in pixel. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<LVec2i32> PreferredResolutionPx{ maths::zero_vector<LVec2u32> };
    //# Preferred refresh rate in hz. Zero means default. This is a legacy feature as most compositors straight up ignore this.
    CLASS_FIELD(Config)
    TPreference<i32> PreferredRefreshRateHz{ 0 };

    CLASS_FIELD(Config)
    TPreference<rhi::present_mode> DesiredPresentMode{ rhi::present_mode::Fifo };
    //#
    //# The cap for ticks per second.
    //# If Jafg is running with the local layer then this preference only has an effect if the present mode
    //# is not blocking.
    //#
    CLASS_FIELD(Config)
    TPreference<i32> MaxTps{ UnlimitedTps };

    CLASS_FIELD(Config)
    TPreference<EPolygonMode> PolygonMode{ EPolygonMode::Fill };

    CLASS_FIELD(Config)
    TPreference<bool> EditorPerspectiveDepthTestHint{ true };
    //# If set, this material should be preferred by mesh renders to use.
    std::optional<LMaterialInstanceRef> EditorMeshMaterialPreference;
    //# Might not be supported in all configurations.
    bool bHighlightFontRects{};
    LColor FontRectHighlightColor{ 0xFF000030_color };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TPreference<f32> InterfaceGamma{ 2.4f };

    CLASS_FIELD(Config)
    TPreference<EApplicationScale> ApplicationScaleMode{ EApplicationScale::Double };

    CLASS_FIELD(Config)
    TPreference<bool> bInvertVerticalScrollWheel{ true };
    CLASS_FIELD(Config)
    TPreference<bool> bInvertHorizontalScrollWheel{ true };
    CLASS_FIELD(Config)
    TPreference<f32> MouseWheelScrollSpeed{ 35.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeSingle    { 15.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeSingle { 14.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeSingle      { 11.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeSingle   { 10.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeSingle     {  9.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeDouble    { 19.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeDouble { 17.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeDouble      { 12.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeDouble   { 11.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeDouble     { 10.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeTriple    { 21.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeTriple { 19.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeTriple      { 13.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeTriple   { 12.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeTriple     { 10.0f };

    CLASS_FIELD(Config)
    TPreference<LColor> BackgroundColor         { 0x15 };
    CLASS_FIELD(Config)
    TPreference<LColor> AccentColor             { 0x1C };
    CLASS_FIELD(Config)
    TPreference<LColor> ForegroundColor         { 0x24 };
    CLASS_FIELD(Config)
    TPreference<LColor> ForegroundColorVariant  { 0x2F };
    CLASS_FIELD(Config)
    TPreference<LColor> InputColor              { 0x0F };
    CLASS_FIELD(Config)
    TPreference<LColor> OverlayColor            { 0x38 };
    CLASS_FIELD(Config)
    TPreference<LColor> DisabledColor           { 0x1B };
    CLASS_FIELD(Config)
    TPreference<LColor> TextColor               { 0xFF };

    TPreference<LStylePalette> PrimaryPalette{{
        .Normal=  {.Tint=0x337AB7FF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Hover=   {.Tint=0x286090FF_color,.Outline=0x204D74FF_color,.TextTint=0xF0F0F0FF_color},
        .Press=   {.Tint=0x337AB7FF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Selected={.Tint=0x337AB7FF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Disabled={.Tint=0x122B40FF_color,.Outline=0x2E6D43FF_color,.TextTint=0x808080FF_color},
        }};
    TPreference<LStylePalette> SecondaryPalette{{
        .Normal=  {.Tint=0x2F2F2FFF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Hover=   {.Tint=0x3F3F3FFF_color,.Outline=0x204D74FF_color,.TextTint=0xF0F0F0FF_color},
        .Press=   {.Tint=0x4F4F4FFF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Selected={.Tint=0x5F5F5FFF_color,.Outline=0x2E6D43FF_color,.TextTint=0xF0F0F0FF_color},
        .Disabled={.Tint=0x1F1F1FFF_color,.Outline=0x2E6D43FF_color,.TextTint=0x808080FF_color},
        }};

    CLASS_FIELD(Config)
    TPreference<LColor> PrimaryColor            { LColor{0x1D, 0x78, 0xD6} };
    CLASS_FIELD(Config)
    TPreference<LColor> PrimaryColorVariant     { LColor{0x02, 0x1B, 0x35} };
    CLASS_FIELD(Config)
    TPreference<LColor> PrimaryColor2           { LColor{0x40, 0x57, 0x6F} };
    CLASS_FIELD(Config)
    TPreference<LColor> PrimaryColorVariant2    { LColor{0x2C, 0x32, 0x3A} };
    CLASS_FIELD(Config)
    TPreference<LColor> ProximityColorA         { 0x1A };
    CLASS_FIELD(Config)
    TPreference<LColor> ProximityColorB         { 0x15 };
    CLASS_FIELD(Config)
    TPreference<LColor> DangerColor             { LColor{0x94, 0x33, 0x33} };

    NODISCARD FORCEINLINE LColor const& GetProximityColor(std::integral auto Idx) const noexcept
    {
        if (Idx % 2)
        {
            return *this->ProximityColorA;
        }
        return *this->ProximityColorB;
    }

    CLASS_FIELD(Config)
    TPreference<std::size_t> PreferredDragPadding{ 3 };
    CLASS_FIELD(Config)
    TPreference<std::size_t> PreferredDragOverlap{ 2 };
    CLASS_FIELD(Config)
    TPreference<LColor> ViewportBackgroundTint  { Colors::Black };

    ///////////////////////////////////////////////////////////////////////////////
    // Foreign plugins
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional plugin search paths that are used to fetch plugin info metadata.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> AdditionalPluginsSearchPaths;

    //#
    //# The plugins that are loaded when the engine loads.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> EnabledEnginePlugins{ {"JafgGameplayCore"} };

    ///////////////////////////////////////////////////////////////////////////////
    // Storage
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional saves search paths that are used to fetch saves info metadata.
    //# The default ist Saved/Saves.
    //#
    CLASS_FIELD(Config)
    TPreference<TArray<LString>> AdditionalSavesSearchPaths;

    ///////////////////////////////////////////////////////////////////////////////
    // Editor
    ///////////////////////////////////////////////////////////////////////////////

    //# The editor if active.
    WEditor* Editor{};

    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintX{ Colors::Crimson };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintY{ Colors::LimeGreen };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintZ{ Colors::DeepSkyBlue };

    CLASS_FIELD(Config)
    TPreference<bool> EditorSortDirectoriesFirst{ true };

    CLASS_FIELD(Config)
    TPreference<LString> EditorLastWorldName{ "Editor World" };
    CLASS_FIELD(Config)
    TPreference<LString> EditorLastWorldLevelName;
    CLASS_FIELD(Config)
    TPreference<bool> EditorAutoLaunchLastWorld;
    CLASS_FIELD(Config)
    TPreference<bool> EditorShowRate{ true };
    CLASS_FIELD(Config)
    TPreference<LPath> EditorLastLayout{ "Config/DefaultEditorLayout.json" };

    ///////////////////////////////////////////////////////////////////////////////
    // Factories
    ///////////////////////////////////////////////////////////////////////////////

    template<typename TBrush>
    inline static constexpr bool valid_brush_v{std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>};
    template<typename TBrush>
    inline static constexpr bool paddable_brush_v{valid_brush_v<TBrush> && std::is_base_of_v<LBoxBrush, TBrush>};

    template<typename TBrush> requires valid_brush_v<TBrush>
    void ApplyPaletteForState(TBrush& Brush, LStylePalette::State const& State) const noexcept
    {
        Brush.Tint = State.Tint;
        Brush.BorderTint = State.Tint;
        Brush.OutlineTint = State.Outline;
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    void ApplyIconsizedPaletteForState(TBrush& Brush, LStylePalette::State const& State) const noexcept
    {
        Brush.Tint = State.TextTint;
        Brush.BorderTint = State.Tint;
        Brush.OutlineTint = State.Outline;
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    void ApplyPalette(TButtonStyle<TBrush>& Style, LStylePalette const& Palette) const noexcept
    {
        this->ApplyPaletteForState(Style.NormalBrush, Palette.Normal);
        this->ApplyPaletteForState(Style.HoverBrush, Palette.Hover);
        this->ApplyPaletteForState(Style.PressBrush, Palette.Press);
        this->ApplyPaletteForState(Style.SelectedBrush, Palette.Selected);
        this->ApplyPaletteForState(Style.DisabledBrush, Palette.Disabled);
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    void ApplyIconizedPalette(TButtonStyle<TBrush>& Style, LStylePalette const& Palette) const noexcept
    {
        this->ApplyIconsizedPaletteForState(Style.NormalBrush, Palette.Normal);
        this->ApplyIconsizedPaletteForState(Style.HoverBrush, Palette.Hover);
        this->ApplyIconsizedPaletteForState(Style.PressBrush, Palette.Press);
        this->ApplyIconsizedPaletteForState(Style.SelectedBrush, Palette.Selected);
        this->ApplyIconsizedPaletteForState(Style.DisabledBrush, Palette.Disabled);
    }

    template<typename TBrush> requires valid_brush_v<TBrush>
    void PadStyle(TButtonStyle<TBrush>& Brush, LPadding Padding) const noexcept
    {
        if constexpr (std::is_base_of_v<LBoxBrush, TBrush>)
        {
            Brush.template SetEverywhere<&LBoxBrush::Padding>(Padding);
        }
    }

    template<typename TBrush> requires valid_brush_v<TBrush>
    NODISCARD TButtonStyle<TBrush> EditorBaseButton() const noexcept
    {
        TButtonStyle<TBrush> Result;
        Result.template ChainEverywhere<&TBrush::OutlineThickness, &TBrush::Radii>(1, LVec4F{5.0f});
        this->PadStyle(Result, {5_spt, 0});
        return Result;
    }

    template<typename TBrush> requires valid_brush_v<TBrush>
    NODISCARD TButtonStyle<TBrush> EditorPrimaryButton() const noexcept
    {
        TButtonStyle<TBrush> Result{this->EditorBaseButton<TBrush>()};
        this->ApplyPalette(Result, *this->PrimaryPalette);
        return Result;
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    NODISCARD TButtonStyle<TBrush> EditorPrimaryButton(LOptionalTexture2Ref Icon) const noexcept
    {
        TButtonStyle<TBrush> Result{this->EditorBaseButton<TBrush>()};
        this->ApplyIconizedPalette(Result, *this->PrimaryPalette);
        Result.template SetEverywhere<&LRegionBrush::Background>(LRegionBrush::Icon(std::move(Icon)));
        return Result;
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    NODISCARD TButtonStyle<TBrush> EditorSecondaryButton() const noexcept
    {
        TButtonStyle<TBrush> Result{this->EditorBaseButton<TBrush>()};
        this->ApplyPalette(Result, *this->SecondaryPalette);
        return Result;
    }
    template<typename TBrush> requires valid_brush_v<TBrush>
    NODISCARD TButtonStyle<TBrush> EditorSecondaryButton(LOptionalTexture2Ref Icon) const noexcept
    {
        TButtonStyle<TBrush> Result{this->EditorBaseButton<TBrush>()};
        this->ApplyIconizedPalette(Result, *this->SecondaryPalette);
        Result.template SetEverywhere<&LRegionBrush::Background>(LRegionBrush::Icon(std::move(Icon)));
        return Result;
    }




    template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>
    NODISCARD TButtonStyle<TBrush> EditorPrimaryButtonStyle() const noexcept
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};

        TButtonStyle<TBrush> Result;
        Result.template ChainEverywhere
            <&TBrush::Tint, &TBrush::OutlineTint, &TBrush::OutlineThickness, &TBrush::Radii>
            (*Prefs.PrimaryColor, {0x8F}, 1, LVec4F{5.0f});
            this->PadStyle(Result, {5_spt, 0});
        Result.template Chain<EStyleBits::Hover, &TBrush::Tint>(*Prefs.PrimaryColorVariant);
        Result.template Chain<EStyleBits::Press | EStyleBits::Selected, &TBrush::Tint>(*Prefs.PrimaryColorVariant);
        Result.template Chain<EStyleBits::Disabled, &TBrush::Tint>(*this->DisabledColor);
        return Result;
    }
    template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>
    NODISCARD TButtonStyle<TBrush> EditorSecondaryButtonStyle() const noexcept
    {
        TButtonStyle<TBrush> Result;
        Result.template ChainEverywhere<
            &TBrush::Tint, &TBrush::OutlineTint, &TBrush::OutlineThickness, &TBrush::Radii>
            (*this->AccentColor, {0x8F}, 1, LVec4F{5.0f});
        if constexpr (std::same_as<TBrush, LTextBoxBrush>)
        {
            Result.template SetEverywhere<&TBrush::Padding>({5_spt, 0});
        }
        Result.template Chain<EStyleBits::Hover, &TBrush::Tint>(*this->PrimaryColor);
        Result.template Chain<EStyleBits::Press | EStyleBits::Selected, &TBrush::Tint>(*this->PrimaryColorVariant);
        Result.template Chain<EStyleBits::Disabled, &TBrush::Tint>(*this->DisabledColor);
        return Result;
    }

    template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>
    NODISCARD TButtonStyle<TBrush> EditorEditableTextButtonStyle() const noexcept
    {
        TButtonStyle<TBrush> Result;
        Result.template ChainEverywhere<
            &TBrush::Tint, &TBrush::OutlineTint, &TBrush::OutlineThickness, &TBrush::Radii, &TBrush::Padding
            >(*this->InputColor, {0x8F}, 1, LVec4F{5.0f}, {5_spt, 0});
        Result.template Chain<EStyleBits::Normal|EStyleBits::Disabled, &TBrush::OutlineTint>({0x5F});
        return Result;
    }
    NODISCARD TButtonStyle<LTextBoxBrush> EditorEditableTextButtonTextStyle() const noexcept
    {
        TButtonStyle<LTextBoxBrush> Result;
        Result.Chain<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray);
        return Result;
    }

    template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>
    NODISCARD TButtonStyle<TBrush> EditorProximityBoxStyle(std::integral auto Idx) const noexcept
    {
        TButtonStyle<TBrush> Style;
        Style.template Set<EStyleBits::Normal, &TBrush::Tint>(this->GetProximityColor(Idx));
        Style.template Chain<EStyleBits::Hover, &TBrush::Tint>(*this->PrimaryColor);
        Style.template Chain<EStyleBits::Press | EStyleBits::Selected, &TBrush::Tint>(*this->PrimaryColorVariant);
        return Style;
    }
    NODISCARD TButtonStyle<LTextBoxBrush> EditorProximityBoxTextStyle() const noexcept
    {
        TButtonStyle<LTextBoxBrush> Result;
        Result.Chain<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray);
        return Result;
    }

    template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush> || std::is_base_of_v<LTextBoxBrush, TBrush>
    NODISCARD TButtonStyle<TBrush> EditorProximityBoxStyle2(std::integral auto Idx) const noexcept
    {
        TButtonStyle<TBrush> Style;
        Style.template Set<EStyleBits::Normal, &TBrush::Tint>(this->GetProximityColor(Idx));
        Style.template Chain<EStyleBits::Hover, &TBrush::Tint>(*this->PrimaryColor2);
        Style.template Chain<EStyleBits::Press | EStyleBits::Selected, &TBrush::Tint>(*this->PrimaryColorVariant2);
        return Style;
    }
    NODISCARD TButtonStyle<LTextBoxBrush> EditorProximityBoxTextStyle2() const noexcept
    {
        TButtonStyle<LTextBoxBrush> Result;
        Result.Chain<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray);
        return Result;
    }
};

} /* ~Namespace Jafg */
