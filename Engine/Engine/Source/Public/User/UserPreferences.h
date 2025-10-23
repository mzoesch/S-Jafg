// Copyright mzoesch. All rights reserved.

#pragma once


#include "Engine/CxxClass.h"
#include "User/UserPreferencesForward.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Config, ECxxClassFlags::Singleton)
class JUserPreferences final : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JUserPreferences)

public:

    enum { UnlimitedFps = 0 };

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferencef32 MasterVolume{ 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 MusicVolume{ 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 MiscVolume{ 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 VoiceVolume{ 1.0f, true };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceBool bVSyncEnabled{ true };
    //# Number of maximum frames per second. Zero means no limit. This requires VSync to be disabled.
    CLASS_FIELD(Config)
    LPreferencei32 MaxFps{ UnlimitedFps };

    CLASS_FIELD(Config)
    TPreference<EPolygonMode::Type> PolygonMode{ EPolygonMode::Fill };

    CLASS_FIELD(Config)
    i32 ChunkRenderDistance{ 6 };
    CLASS_FIELD(Config)
    i32 ChunkRenderHeight{ 6 };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    TPreference<EApplicationScale::Type> ApplicationScaleMode{ EApplicationScale::Double };

    CLASS_FIELD(Config)
    LPreferenceBool bInvertVerticalScrollWheel{ true };
    CLASS_FIELD(Config)
    LPreferenceBool bInvertHorizontalScrollWheel{ true };
    CLASS_FIELD(Config)
    LPreferencef32 MouseWheelScrollSpeed{ 35.0f };

    CLASS_FIELD(Config)
    LPreferencef32 HeaderFontSizeSingle    { 0.40f };
    CLASS_FIELD(Config)
    LPreferencef32 SubHeaderFontSizeSingle { 0.25f };
    CLASS_FIELD(Config)
    LPreferencef32 BodyFontSizeSingle      { 0.20f };
    CLASS_FIELD(Config)
    LPreferencef32 CompactFontSizeSingle   { 0.20f };
    CLASS_FIELD(Config)
    LPreferencef32 SmallFontSizeSingle     { 0.20f };
    CLASS_FIELD(Config)
    LPreferencef32 TinyFontSizeSingle      { 0.18f };

    CLASS_FIELD(Config)
    LPreferencef32 HeaderFontSizeDouble    { 0.70f };
    CLASS_FIELD(Config)
    LPreferencef32 SubHeaderFontSizeDouble { 0.40f };
    CLASS_FIELD(Config)
    LPreferencef32 BodyFontSizeDouble      { 0.30f };
    CLASS_FIELD(Config)
    LPreferencef32 CompactFontSizeDouble   { 0.25f };
    CLASS_FIELD(Config)
    LPreferencef32 SmallFontSizeDouble     { 0.20f };
    CLASS_FIELD(Config)
    LPreferencef32 TinyFontSizeDouble      { 0.18f };

    CLASS_FIELD(Config)
    LPreferencef32 HeaderFontSizeTriple    { 1.40f };
    CLASS_FIELD(Config)
    LPreferencef32 SubHeaderFontSizeTriple { 0.80f };
    CLASS_FIELD(Config)
    LPreferencef32 BodyFontSizeTriple      { 0.60f };
    CLASS_FIELD(Config)
    LPreferencef32 CompactFontSizeTriple   { 0.50f };
    CLASS_FIELD(Config)
    LPreferencef32 SmallFontSizeTriple     { 0.40f };
    CLASS_FIELD(Config)
    LPreferencef32 TinyFontSizeTriple      { 0.36f };

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

template <>
struct std::formatter<::Jafg::EPolygonMode::Type> : std::formatter<u8>
{
    static_assert(std::is_enum_v<::Jafg::EPolygonMode::Type>);
    static_assert(std::is_same_v<u8, std::underlying_type_t<::Jafg::EPolygonMode::Type>>);
    FORCEINLINE auto format
    (
        const ::Jafg::EPolygonMode::Type& InMode,
        ::std::format_context&            InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<u8>::format(static_cast<u8>(InMode), InContext);
    }
};
