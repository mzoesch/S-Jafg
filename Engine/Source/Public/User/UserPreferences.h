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
    TPreference<f32> HeaderFontSizeSingle    { 16.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeSingle { 15.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeSingle      { 11.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeSingle   { 10.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeSingle     {  9.0f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeSingle      {  8.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeDouble    { 20.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeDouble { 18.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeDouble      { 16.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeDouble   { 15.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeDouble     { 14.0f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeDouble      { 13.0f };

    CLASS_FIELD(Config)
    TPreference<f32> HeaderFontSizeTriple    { 22.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SubHeaderFontSizeTriple { 20.0f };
    CLASS_FIELD(Config)
    TPreference<f32> BodyFontSizeTriple      { 18.0f };
    CLASS_FIELD(Config)
    TPreference<f32> CompactFontSizeTriple   { 17.0f };
    CLASS_FIELD(Config)
    TPreference<f32> SmallFontSizeTriple     { 16.0f };
    CLASS_FIELD(Config)
    TPreference<f32> TinyFontSizeTriple      { 15.0f };

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
