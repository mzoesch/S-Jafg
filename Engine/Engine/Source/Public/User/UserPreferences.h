// Copyright mzoesch. All rights reserved.

#pragma once


#include "Engine/CxxClass.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

namespace EPolygonMode
{

enum Type : u8
{
    Fill      = 0,
    Wireframe = 1,
    Max       = 1,
    Num       = 2,
};

} /* ~Namespace EPolygonMode */

template <> struct TIsPreferenceTypeAllowed<EPolygonMode::Type> : std::true_type { };

} /* ~Namespace Jafg */

template<> FORCEINLINE LString Serialization::ToString<Jafg::EPolygonMode::Type>(Jafg::EPolygonMode::Type const& Field) noexcept
{
    static_assert(std::is_enum_v<Jafg::EPolygonMode::Type>);
    static_assert(std::is_same_v<u8, std::underlying_type_t<Jafg::EPolygonMode::Type>>);

    return ToString(static_cast<u8>(Field));
}

template<> FORCEINLINE void Serialization::FromString<Jafg::EPolygonMode::Type>(Jafg::EPolygonMode::Type* Dst, LString const& Value) noexcept
{
    static_assert(std::is_enum_v<Jafg::EPolygonMode::Type>);
    static_assert(std::is_same_v<u8, std::underlying_type_t<Jafg::EPolygonMode::Type>>);

    check( Dst )
    FromString(reinterpret_cast<u8*>(Dst), Value);
    return;
}

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
    LPreferencef32 MasterVolume { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 MusicVolume  { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 MiscVolume   { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferencef32 VoiceVolume  { 1.0f, true };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceBool bVSyncEnabled { true };
    //# Number of maximum frames per second. Zero means no limit. This requires VSync to be disabled.
    CLASS_FIELD(Config)
    LPreferencei32 MaxFps         { UnlimitedFps };

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
    f32 ApplicationScale{ 1.0f };

    CLASS_FIELD(Config)
    LPreferenceBool bInvertVerticalScrollWheel{ true };
    CLASS_FIELD(Config)
    LPreferenceBool bInvertHorizontalScrollWheel{ true };
    CLASS_FIELD(Config)
    LPreferencef32 MouseWheelScrollSpeed{ 35.0f };

    CLASS_FIELD(Config)
    LPreferencef32 HeaderFontSize    { 0.70f };
    CLASS_FIELD(Config)
    LPreferencef32 SubHeaderFontSize { 0.40f };
    CLASS_FIELD(Config)
    LPreferencef32 BodyFontSize      { 0.30f };
    CLASS_FIELD(Config)
    LPreferencef32 CompactFontSize   { 0.25f };
    CLASS_FIELD(Config)
    LPreferencef32 SmallFontSize     { 0.20f };
    CLASS_FIELD(Config)
    LPreferencef32 TinyFontSize      { 0.18f };

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
