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
    TPreference<f32> HeaderFontSizeSingle    { 0.40f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeSingle { 0.25f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeSingle      { 0.20f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeSingle   { 0.20f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeSingle     { 0.20f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeSingle      { 0.18f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeDouble    { 0.70f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeDouble { 0.40f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeDouble      { 0.30f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeDouble   { 0.25f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeDouble     { 0.20f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeDouble      { 0.18f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeTriple    { 1.40f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeTriple { 0.80f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeTriple      { 0.60f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeTriple   { 0.50f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeTriple     { 0.40f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeTriple      { 0.36f };

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
