// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "User/Preferences/PreferencesTypes.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

namespace EPolygonMode
{

enum Type : uint8
{
    Fill      = 0,
    Wireframe = 1,
    Max       = 1,
    Num       = 2,
};

} /* ~Namespace EPolygonMode. */
template <> struct TIsPreferenceTypeAllowed<EPolygonMode::Type> : std::true_type { };
template <>
FORCEINLINE void ::Jafg::Deserialize<TPreference<EPolygonMode::Type>>(TPreference<EPolygonMode::Type>* Destination, const LString& InValue)
{
    static_assert(std::is_enum_v<EPolygonMode::Type>);
    static_assert(std::is_same_v<uint8, std::underlying_type_t<EPolygonMode::Type>>);
    checkSlow( Destination )
    Jafg::Deserialize<uint8>(reinterpret_cast<uint8*>(&Destination->Value), InValue);
    return;
}

DECLARE_JAFG_CLASS(EClassFlags::Config, EClassFlags::Singleton)
class ENGINE_API JUserPreferences final : public JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JUserPreferences)

    ///////////////////////////////////////////////////////////////////////////////
    // Audio
    ///////////////////////////////////////////////////////////////////////////////

public:

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
    CLASS_FIELD(Config)
    TPreference<EPolygonMode::Type> PolygonMode { EPolygonMode::Fill };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

    CLASS_FIELD(Config)
    LPreferenceFloat HeaderFontSize    { 1.30f };
    CLASS_FIELD(Config)
    LPreferenceFloat SubHeaderFontSize { 1.00f };
    CLASS_FIELD(Config)
    LPreferenceFloat BodyFontSize      { 0.70f };
    CLASS_FIELD(Config)
    LPreferenceFloat SmallFontSize     { 0.40f };
    CLASS_FIELD(Config)
    LPreferenceFloat TinyFontSize      { 0.30f };
};

} /* ~Namespace Jafg */

template <>
struct ::std::formatter<::Jafg::EPolygonMode::Type> : ::std::formatter<uint8>
{
    static_assert(std::is_enum_v<::Jafg::EPolygonMode::Type>);
    static_assert(std::is_same_v<uint8, std::underlying_type_t<::Jafg::EPolygonMode::Type>>);
    FORCEINLINE auto format
    (
        const ::Jafg::EPolygonMode::Type& InMode,
        ::std::format_context&            InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<uint8>::format(static_cast<uint8>(InMode), InContext);
    }
};
