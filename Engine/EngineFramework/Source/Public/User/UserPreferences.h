// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "UserPreferences.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINEFRAMEWORK_API JUserPreferences final : public Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JUserPreferences)

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
