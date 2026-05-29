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

    CLASS_FIELD(Config)
    TPreference<std::size_t> PreferredDragPadding{ 3 };
    CLASS_FIELD(Config)
    TPreference<LColor> ViewportBackgroundTint  { Colors::Black };

    ///////////////////////////////////////////////////////////////////////////////
    // Editor
    ///////////////////////////////////////////////////////////////////////////////

    // TODO: This should just be TPreference<TButtonStyle<LBoxBrush>> but we wait until we adopt the cxx26 reflection
    //       system as writing serde for this is just boring und unnecessary when in a couple of months we can completely
    //       automate it.
    TButtonStyle<LBoxBrush> EditorEditableTextButtonStyle() const noexcept
    {
        TButtonStyle<LBoxBrush> Result;
        Result.ChainEverywhere<
            &LBoxBrush::Tint, &LBoxBrush::OutlineTint, &LBoxBrush::OutlineThickness, &LBoxBrush::Radii, &LBoxBrush::Padding
            >(*this->InputColor, {0x8F}, 1, LVec4F{4}, {5_spt, 0});
        Result.Chain<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::OutlineTint>({0x5F});
        return Result;
    }
    TButtonStyle<LTextBoxBrush> EditorEditableTextButtonTextStyle() const noexcept
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
