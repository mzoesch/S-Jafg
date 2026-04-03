// Copyright mzoesch. All rights reserved.

#pragma once


#include "Engine/CxxClass.h"
#include "User/UserPreferencesForward.h"
#include "Rhi/Material.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Config, ECxxClassFlags::Singleton)
class JUserPreferences final : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JUserPreferences)

public:

    enum { UnlimitedFps = 0 };

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TPreference<f32> MasterVolume{ 1.f, 0.f, 1.f };
    CLASS_FIELD(Config)
    TPreference<f32> MusicVolume{ 1.f, 0.f, 1.f };
    CLASS_FIELD(Config)
    TPreference<f32> MiscVolume{ 1.f, 0.f, 1.f };
    CLASS_FIELD(Config)
    TPreference<f32> VoiceVolume{ 1.f, 0.f, 1.f };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LString PreferredPhysicalDevice;

    CLASS_FIELD(Config)
    TPreference<bool> bVSyncEnabled{ true };
    //# Number of maximum frames per second. Zero means no limit. This requires VSync to be disabled.
    CLASS_FIELD(Config)
    TPreference<i32> MaxFps{ UnlimitedFps };

    CLASS_FIELD(Config)
    TPreference<EPolygonMode> PolygonMode{ EPolygonMode::Fill };

    //# If set, this material should be preferred by mesh renders to use.
    TOptional<LMaterialInstanceRef> MeshMaterialPreference;

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

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
    TPreference<LColor> ForegroundColorVariant  { 0x0F };
    CLASS_FIELD(Config)
    TPreference<LColor> DisabledColor           { 0x0F };
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

    ///////////////////////////////////////////////////////////////////////////////
    // Foreign plugins
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional plugin search paths that are used to fetch plugin info metadata.
    //#
    CLASS_FIELD(Config)
    TArray<LString> AdditionalPluginsSearchPaths;

    //#
    //# The plugins that are loaded when the engine loads.
    //#
    CLASS_FIELD(Config)
    TArray<LString> EnabledEnginePlugins{ "JafgGameplayCore" };

    ///////////////////////////////////////////////////////////////////////////////
    // Storage
    ///////////////////////////////////////////////////////////////////////////////

    //#
    //# Additional saves search paths that are used to fetch saves info metadata.
    //# The default ist Saved/Saves.
    //#
    CLASS_FIELD(Config)
    TArray<LString> AdditionalSavesSearchPaths;
};

} /* ~Namespace Jafg */
