// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
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

DECLARE_JAFG_CLASS()
class ENGINEFRAMEWORK_API JUserPreferences final : public Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JUserPreferences)

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////

public:

    FORCEINLINE constexpr bool GetDefaultVSyncEnabled() const { return JUserPreferences::bDefaultVSyncEnabled; }
    FORCEINLINE constexpr EPolygonMode::Type GetDefaultPolygonMode() const { return JUserPreferences::DefaultPolygonMode; }
    FORCEINLINE bool GetVSyncEnabled() const { return this->bVSyncEnabled; }
    FORCEINLINE void SetVSyncEnabled(const bool bEnabled) { this->bVSyncEnabled = bEnabled; }
    FORCEINLINE EPolygonMode::Type GetPolygonMode() const { return this->PolygonMode; }
    FORCEINLINE void SetPolygonMode(const EPolygonMode::Type NewPolygonMode) { this->PolygonMode = NewPolygonMode; }

private:

    inline static constexpr bool bDefaultVSyncEnabled { true };
    inline static constexpr EPolygonMode::Type DefaultPolygonMode { EPolygonMode::Fill };
    bool bVSyncEnabled { JUserPreferences::bDefaultVSyncEnabled };
    EPolygonMode::Type PolygonMode { JUserPreferences::DefaultPolygonMode };

    ///////////////////////////////////////////////////////////////////////////////
    // Interface
    ///////////////////////////////////////////////////////////////////////////////

public:

    FORCEINLINE constexpr float GetDefaultHeaderFontSize() const { return JUserPreferences::DefaultHeaderFontSize; }
    FORCEINLINE constexpr float GetDefaultSubHeaderFontSize() const { return JUserPreferences::DefaultSubHeaderFontSize; }
    FORCEINLINE constexpr float GetDefaultBodyFontSize() const { return JUserPreferences::DefaultBodyFontSize; }
    FORCEINLINE constexpr float GetDefaultSmallFontSize() const { return JUserPreferences::DefaultSmallFontSize; }
    FORCEINLINE constexpr float GetDefaultTinyFontSize() const { return JUserPreferences::DefaultTinyFontSize; }
    FORCEINLINE float GetHeaderFontSize() const { return this->HeaderFontSize; }
    FORCEINLINE float GetSubHeaderFontSize() const { return this->SubHeaderFontSize; }
    FORCEINLINE float GetBodyFontSize() const { return this->BodyFontSize; }
    FORCEINLINE float GetSmallFontSize() const { return this->SmallFontSize; }
    FORCEINLINE float GetTinyFontSize() const { return this->TinyFontSize; }

private:

    inline static constexpr float DefaultHeaderFontSize    { 0.11f };
    inline static constexpr float DefaultSubHeaderFontSize { 0.10f };
    inline static constexpr float DefaultBodyFontSize      { 0.70f };
    inline static constexpr float DefaultSmallFontSize     { 0.50f };
    inline static constexpr float DefaultTinyFontSize      { 0.30f };
    float HeaderFontSize    { JUserPreferences::DefaultHeaderFontSize    };
    float SubHeaderFontSize { JUserPreferences::DefaultSubHeaderFontSize };
    float BodyFontSize      { JUserPreferences::DefaultBodyFontSize      };
    float SmallFontSize     { JUserPreferences::DefaultSmallFontSize     };
    float TinyFontSize      { JUserPreferences::DefaultTinyFontSize      };
};

} /* ~Namespace Jafg. */
