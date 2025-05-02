// Copyright mzoesch. All rights reserved.

#pragma once


#include "Engine/ObjectBase.h"
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

} /* ~Namespace EPolygonMode. */
template <> struct TIsPreferenceTypeAllowed<EPolygonMode::Type> : std::true_type { };
template <>
FORCEINLINE void Deserialize<EPolygonMode::Type>(EPolygonMode::Type* Destination, const LString& InValue)
{
    static_assert(std::is_enum_v<EPolygonMode::Type>);
    static_assert(std::is_same_v<u8, std::underlying_type_t<EPolygonMode::Type>>);
    checkSlow( Destination )
    Deserialize<u8>(reinterpret_cast<u8*>(&Destination), InValue);
    return;
}
#if PLATFORM_WASM
    template <> NODISCARD inline auto FormatArgLegacy<const TPreference<EPolygonMode::Type>&>(const TPreference<EPolygonMode::Type>& Arg) { return static_cast<u8>(Arg.Value); }
    template <> NODISCARD inline auto FormatArgLegacy<TPreference<EPolygonMode::Type>>(TPreference<EPolygonMode::Type> Arg) { return static_cast<u8>(Arg.Value); }
    template <> NODISCARD inline auto FormatArgLegacy<TPreference<EPolygonMode::Type>&>(TPreference<EPolygonMode::Type>& Arg) { return static_cast<u8>(Arg.Value); }
#endif /* PLATFORM_WASM */

DECLARE_JAFG_CLASS(EClassFlags::Config, EClassFlags::Singleton)
class ENGINE_API JUserPreferences final : public JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JUserPreferences)

public:

    enum { UnlimitedFps = 0 };

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceFloat MasterVolume { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferenceFloat MusicVolume  { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferenceFloat MiscVolume   { 1.0f, true };
    CLASS_FIELD(Config)
    LPreferenceFloat VoiceVolume  { 1.0f, true };

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceBool bVSyncEnabled { true };
    //# Number of maximum frames per second. Zero means no limit. This requires VSync to be disabled.
    CLASS_FIELD(Config)
    LPreferencei32 MaxFps       { UnlimitedFps };

    CLASS_FIELD(Config)
    TPreference<EPolygonMode::Type> PolygonMode { EPolygonMode::Fill };

    CLASS_FIELD(Config)
    i32 ChunkRenderDistance { 6 };
    CLASS_FIELD(Config)
    i32 ChunkRenderHeight   { 6 };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceFloat HeaderFontSize    { 0.70f };
    CLASS_FIELD(Config)
    LPreferenceFloat SubHeaderFontSize { 0.40f };
    CLASS_FIELD(Config)
    LPreferenceFloat BodyFontSize      { 0.30f };
    CLASS_FIELD(Config)
    LPreferenceFloat CompactFontSize   { 0.25f };
    CLASS_FIELD(Config)
    LPreferenceFloat SmallFontSize     { 0.20f };
    CLASS_FIELD(Config)
    LPreferenceFloat TinyFontSize      { 0.18f };
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
