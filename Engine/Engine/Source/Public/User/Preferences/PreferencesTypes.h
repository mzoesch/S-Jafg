// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

template <typename T> struct TIsPreferenceTypeAllowed : std::false_type { };
// Core types. Add more if you want.
template <>           struct TIsPreferenceTypeAllowed<float>   : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<int32>   : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<bool>    : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<LString> : std::true_type { };

template <typename T, typename = TEnableIfTy<TIsPreferenceTypeAllowed<T>::value>>
struct TPreference
{
    FORCEINLINE TPreference(const T InValue) : InitValue(InValue), Value(InValue) { }

    T InitValue;
    T Value;
};

template <>
struct TPreference<float>
{
    FORCEINLINE TPreference(const float InValue)
        : InitValue(InValue), Value(InValue) { }
    FORCEINLINE TPreference(const float InValue, const float InMinValue, const float InMaxValue)
        : InitValue(InValue), Value(InValue), MinValue(InMinValue), MaxValue(InMaxValue) { }
    FORCEINLINE TPreference(const float InValue, const bool bNormalize) : InitValue(InValue), Value(InValue)
        { if (bNormalize) { this->MinValue = 0.0f; this->MaxValue = 1.0f; } }

    FORCEINLINE void SetSafeValue(const float InValue)
    {
        if (this->MinValue && InValue < this->MinValue.GetValue())
        {
            this->Value = this->MinValue.GetValue();
        }
        else if (MaxValue && InValue > this->MaxValue.GetValue())
        {
            this->Value = this->MaxValue.GetValue();
        }
        else
        {
            this->Value = InValue;
        }
    }

    FORCEINLINE float GetDefaultValue() const { return this->InitValue; }
    FORCEINLINE float GetCurrentValue() const { return this->Value; }
    FORCEINLINE bool  HasMinMaxValue() const { return this->MinValue && this->MaxValue; }
    FORCEINLINE bool  HasMinValue() const { return this->MinValue; }
    FORCEINLINE bool  HasMaxValue() const { return this->MaxValue; }
    FORCEINLINE float GetMinValue() const { return this->MinValue.GetValue(); }
    FORCEINLINE float GetMaxValue() const { return this->MaxValue.GetValue(); }

    float InitValue;
    float Value;
    TOptional<float> MinValue;
    TOptional<float> MaxValue;
};

template <>
FORCEINLINE void Deserialize<TPreference<float>>(TPreference<float>* Destination, const LString& InValue)
{
    float InValueF; Deserialize<float>(&InValueF, InValue);
    Destination->SetSafeValue(InValueF);
    return;
}

template <>
FORCEINLINE LString Serialize<TPreference<float>>(const TPreference<float>& InValue)
{
    return LString::SprintF("{}", InValue.GetCurrentValue());
}

} /* ~Namespace Jafg */
