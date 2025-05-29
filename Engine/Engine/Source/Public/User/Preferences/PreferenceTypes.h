// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

template <typename T> struct TIsPreferenceTypeAllowed : std::false_type { };
template <typename T, typename TEnable = void>
struct TPreference;

// Core types. Add more if you want.
template <> struct TIsPreferenceTypeAllowed<i8>      : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<i16>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<i32>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<i64>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<u8>      : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<u16>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<u32>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<u64>     : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<bool>    : std::true_type { };
template <> struct TIsPreferenceTypeAllowed<LString> : std::true_type { };

typedef TPreference<f32>     LPreferenceFloat;
typedef TPreference<f64>     LPreferenceDouble;
typedef TPreference<i8>      LPreferencei8;
typedef TPreference<i16>     LPreferencei16;
typedef TPreference<i32>     LPreferencei32;
typedef TPreference<i64>     LPreferencei64;
typedef TPreference<u8>      LPreferenceu8;
typedef TPreference<u16>     LPreferenceu16;
typedef TPreference<u32>     LPreferenceu32;
typedef TPreference<u64>     LPreferenceu64;
typedef TPreference<bool>    LPreferenceBool;
typedef TPreference<LString> LPreferenceString;

template <typename T>
struct TPreference<T, TEnableIfTy<TIsPreferenceTypeAllowed<T>::value>>
{
    static_assert(std::is_floating_point_v<T> == false);

    FORCEINLINE TPreference(const T InValue) : DefaultValue(InValue), Value(InValue) { }
    FORCEINLINE TPreference(const T InValue, const T InMinValue, const T InMaxValue)
    {
        if (InValue < InMinValue)
        {
            this->DefaultValue = InMinValue;
            this->Value = InMinValue;
        }
        else if (InValue > InMaxValue)
        {
            this->DefaultValue = InMaxValue;
            this->Value = InMaxValue;
        }
        else
        {
            this->DefaultValue = InValue;
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

    FORCEINLINE const T& GetDefaultValue() const { return this->DefaultValue; }
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

    T DefaultValue;
    T Value;
};

template <typename T>
struct TPreference<T, TEnableIfTy<std::is_floating_point_v<T>>>
{
    FORCEINLINE TPreference(const T InValue)
        : DefaultValue(InValue), Value(InValue) { }
    FORCEINLINE TPreference(const T InValue, const T InMinValue, const T InMaxValue)
        : DefaultValue(InValue), Value(InValue), MinValue(InMinValue), MaxValue(InMaxValue) { }
    FORCEINLINE TPreference(const T InValue, const bool bNormalize) : DefaultValue(InValue), Value(InValue)
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

    FORCEINLINE T    GetDefaultValue() const { return this->DefaultValue; }
    FORCEINLINE T    GetCurrentValue() const { return this->Value; }
    FORCEINLINE bool IsMinMaxValueValid() const { return this->MinValue.IsSet() && this->MaxValue.IsSet(); }
    FORCEINLINE bool IsMinValueValid() const { return this->MinValue.IsSet(); }
    FORCEINLINE bool IsMaxValueValid() const { return this->MaxValue.IsSet(); }
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

    T DefaultValue;
    T Value;
    TOptional<T> MinValue;
    TOptional<T> MaxValue;
};

template <typename T>
void TPreference<T, TEnableIfTy<std::is_floating_point_v<T>>>::SetSafeValue(const T InValue)
{
    if (this->MinValue.IsSet() && InValue < this->MinValue.GetValue())
    {
        this->Value = this->MinValue.GetValue();
    }
    else if (MaxValue.IsSet() && InValue > this->MaxValue.GetValue())
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

template <typename T>
FORCEINLINE void Deserialize(TPreference<T>* Destination, const LString& InValue)
{
    checkSlow( Destination )
    Deserialize<T>(&Destination->Value, InValue);
    return;
}

template <typename T>
FORCEINLINE LString Serialize(const TPreference<T>& InValue)
{
    return LString::SprintF("{}", InValue.GetCurrentValue());
}

} /* ~Namespace Jafg */

template <typename T>
struct std::formatter<::Jafg::TPreference<T>> : std::formatter<T>
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

#if PLATFORM_WASM
namespace Jafg
{
template <> NODISCARD inline auto FormatArgLegacy<TPreference<bool>>(TPreference<bool> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<u8>>(TPreference<u8> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<u16>>(TPreference<u16> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<u32>>(TPreference<u32> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<u64>>(TPreference<u64> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<i8>>(TPreference<i8> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<i16>>(TPreference<i16> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<i32>>(TPreference<i32> Arg) { return Arg.Value; }
template <> NODISCARD inline auto FormatArgLegacy<TPreference<i64>>(TPreference<i64> Arg) { return Arg.Value; }
template <typename T> NODISCARD inline auto FormatArgLegacy(const TPreference<T>& Arg) { return Arg.Value; }
template <typename T> NODISCARD inline auto FormatArgLegacy(      TPreference<T>  Arg) { return Arg.Value; }
} /* ~Namespace Jafg */
#endif /* PLATFORM_WASM */
