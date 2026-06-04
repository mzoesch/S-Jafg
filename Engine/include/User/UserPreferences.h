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

    CLASS_FIELD(Config)
    TPreference<LString> PreferredPhysicalDevice;

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
    TPreference<bool> PerspectiveDepthTest{ true };

    //# If set, this material should be preferred by mesh renders to use.
    std::optional<LMaterialInstanceRef> MeshMaterialPreference;
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
    // Editor
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintX{ Colors::Crimson };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintY{ Colors::LimeGreen };
    CLASS_FIELD(Config)
    TPreference<LColor> EditorAxisTintZ{ Colors::DeepSkyBlue };

    // TODO: This should just be TPreference<TButtonStyle<LBoxBrush>> but we wait until we adopt the cxx26 reflection
    //       system as writing serde for this is just boring und unnecessary when in a couple of months we can completely
    //       automate it.
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
};

} /* ~Namespace Jafg */
