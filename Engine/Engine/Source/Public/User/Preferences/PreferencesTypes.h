// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{


template <typename T> struct TIsPreferenceTypeAllowed : std::false_type { };
template <typename T, typename TEnable = void>
struct TPreference;

// Core types. Add more if you want.
template <>           struct TIsPreferenceTypeAllowed<int8>          : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<int16>         : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<int32>         : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<int64>         : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<uint8>         : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<uint16>        : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<uint32>        : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<uint64>        : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<bool>          : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<LString>       : std::true_type { };
template <>           struct TIsPreferenceTypeAllowed<LSimpleString> : std::true_type { };

typedef TPreference<float>         LPreferenceFloat;
typedef TPreference<double>        LPreferenceDouble;
typedef TPreference<int8>          LPreferenceInt8;
typedef TPreference<int16>         LPreferenceInt16;
typedef TPreference<int32>         LPreferenceInt32;
typedef TPreference<int64>         LPreferenceInt64;
typedef TPreference<uint8>         LPreferenceUInt8;
typedef TPreference<uint16>        LPreferenceUInt16;
typedef TPreference<uint32>        LPreferenceUInt32;
typedef TPreference<uint64>        LPreferenceUInt64;
typedef TPreference<bool>          LPreferenceBool;
typedef TPreference<LSimpleString> LPreferenceSimpleString;
typedef TPreference<LString>       LPreferenceString;

template <typename T>
struct TPreference<T, TEnableIfTy<TIsPreferenceTypeAllowed<T>::value>>
{
    static_assert(std::is_floating_point_v<T> == false);

    FORCEINLINE TPreference(const T InValue) : InitValue(InValue), Value(InValue) { }
    FORCEINLINE TPreference(const T InValue, const T InMinValue, const T InMaxValue)
    {
        if (InValue < InMinValue)
        {
            this->InitValue = InMinValue;
            this->Value = InMinValue;
        }
        else if (InValue > InMaxValue)
        {
            this->InitValue = InMaxValue;
            this->Value = InMaxValue;
        }
        else
        {
            this->InitValue = InValue;
            this->Value = InValue;
        }

        return;
    }

    FORCEINLINE TPreference& operator=(const T& InValue)
    {
        this->Value = InValue;
        return *this;
    }
    FORCEINLINE TPreference& operator=(T&& Other)
    {
        this->Value = std::move(Other);
        return *this;
    }

    FORCEINLINE operator T() const { return this->Value; }

    FORCEINLINE const T& GetDefaultValue() const { return this->InitValue; }
    FORCEINLINE const T& GetCurrentValue() const { return this->Value; }

    FORCEINLINE bool operator==(const T& Other) const { return this->Value == Other; }
    FORCEINLINE bool operator!=(const T& Other) const { return !(*this == Other); }
    FORCEINLINE bool operator<(const T& Other) const { return this->Value < Other; }
    FORCEINLINE bool operator>(const T& Other) const { return this->Value > Other; }
    FORCEINLINE bool operator<=(const T& Other) const { return this->Value <= Other; }
    FORCEINLINE bool operator>=(const T& Other) const { return this->Value >= Other; }

    FORCEINLINE bool operator==(const TPreference<T>& Other) const { return this->Value == Other.Value; }
    FORCEINLINE bool operator!=(const TPreference<T>& Other) const { return !(*this == Other); }
    FORCEINLINE bool operator<(const TPreference<T>& Other) const { return this->Value < Other.Value; }
    FORCEINLINE bool operator>(const TPreference<T>& Other) const { return this->Value > Other.Value; }
    FORCEINLINE bool operator<=(const TPreference<T>& Other) const { return this->Value <= Other.Value; }
    FORCEINLINE bool operator>=(const TPreference<T>& Other) const { return this->Value >= Other.Value; }

    T InitValue;
    T Value;
};

template <typename T>
struct TPreference<T, TEnableIfTy<std::is_floating_point_v<T>>>
{
    FORCEINLINE TPreference(const T InValue)
        : InitValue(InValue), Value(InValue) { }
    FORCEINLINE TPreference(const T InValue, const T InMinValue, const T InMaxValue)
        : InitValue(InValue), Value(InValue), MinValue(InMinValue), MaxValue(InMaxValue) { }
    FORCEINLINE TPreference(const T InValue, const bool bNormalize) : InitValue(InValue), Value(InValue)
        { if (bNormalize) { this->MinValue = static_cast<T>(0.0); this->MaxValue = static_cast<T>(1.0); } }

    FORCEINLINE TPreference& operator=(const T& InValue)
    {
        this->Value = InValue;
        return *this;
    }
    FORCEINLINE TPreference& operator=(T&& Other)
    {
        this->Value = std::move(Other);
        return *this;
    }

    FORCEINLINE operator T() const { return this->Value; }

    FORCEINLINE void SetSafeValue(const T InValue);

    FORCEINLINE T    GetDefaultValue() const { return this->InitValue; }
    FORCEINLINE T    GetCurrentValue() const { return this->Value; }
    FORCEINLINE bool HasMinMaxValue() const { return this->MinValue && this->MaxValue; }
    FORCEINLINE bool HasMinValue() const { return this->MinValue; }
    FORCEINLINE bool HasMaxValue() const { return this->MaxValue; }
    FORCEINLINE T    GetMinValue() const { return this->MinValue.GetValue(); }
    FORCEINLINE T    GetMaxValue() const { return this->MaxValue.GetValue(); }

    FORCEINLINE bool operator==(const T& Other) const { return Maths::IsNearlyEqual(this->Value, Other); }
    FORCEINLINE bool operator!=(const T& Other) const { return !(*this == Other); }
    FORCEINLINE bool operator<(const T& Other) const { return this->Value < Other; }
    FORCEINLINE bool operator>(const T& Other) const { return this->Value > Other; }
    FORCEINLINE bool operator<=(const T& Other) const { return this->Value <= Other; }
    FORCEINLINE bool operator>=(const T& Other) const { return this->Value >= Other; }

    FORCEINLINE bool operator==(const TPreference<T>& Other) const { return Maths::IsNearlyEqual(this->Value, Other.Value); }
    FORCEINLINE bool operator!=(const TPreference<T>& Other) const { return !(*this == Other); }
    FORCEINLINE bool operator<(const TPreference<T>& Other) const { return this->Value < Other.Value; }
    FORCEINLINE bool operator>(const TPreference<T>& Other) const { return this->Value > Other.Value; }
    FORCEINLINE bool operator<=(const TPreference<T>& Other) const { return this->Value <= Other.Value; }
    FORCEINLINE bool operator>=(const TPreference<T>& Other) const { return this->Value >= Other.Value; }

    T InitValue;
    T Value;
    TOptional<T> MinValue;
    TOptional<T> MaxValue;
};

template <typename T>
void TPreference<T, TEnableIfTy<std::is_floating_point_v<T>>>::SetSafeValue(const T InValue)
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

template <typename T>
struct ::std::formatter<::Jafg::TPreference<T>> : ::std::formatter<T>
{
    FORCEINLINE auto format
    (
        const ::Jafg::TPreference<T>& InPreference,
        ::std::format_context&        InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<T>::format(InPreference.Value, InContext);
    }
};

template <>
FORCEINLINE void ::Jafg::Deserialize<Jafg::LPreferenceBool>(LPreferenceBool* Destination, const LString& InValue)
{
    checkSlow( Destination )
    Jafg::Deserialize<bool>(&Destination->Value, InValue);
}
