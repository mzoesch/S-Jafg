// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

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

typedef TPreference<f32>     LPreferencef32;
typedef TPreference<f64>     LPreferencef64;
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
    FORCEINLINE bool IsMinMaxValueValid() const { return this->MinValue.has_value() && this->MaxValue.has_value(); }
    FORCEINLINE bool IsMinValueValid() const { return this->MinValue.has_value(); }
    FORCEINLINE bool IsMaxValueValid() const { return this->MaxValue.has_value(); }
    FORCEINLINE T    GetMinValue() const { return this->MinValue.value(); }
    FORCEINLINE T    GetMaxValue() const { return this->MaxValue.value(); }

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
    if (this->MinValue.has_value() && InValue < this->MinValue.value())
    {
        this->Value = this->MinValue.value();
    }
    else if (MaxValue.has_value() && InValue > this->MaxValue.value())
    {
        this->Value = this->MaxValue.value();
    }
    else
    {
        this->Value = InValue;
    }

    return;
}

} /* ~Namespace Jafg */

namespace Serialization
{

template<typename TSubField> NODISCARD FORCEINLINE constexpr LString ToString(Jafg::TPreference<TSubField> const& Field) noexcept
    requires requires(TSubField const& SubField){ ToString(SubField); }
{
    return ToString<TSubField>(Field.Value);
}

template<> FORCEINLINE constexpr void FromString<Jafg::LPreferencef32>(Jafg::LPreferencef32* Dst, LString const& Value) noexcept
{
    check( Dst )
    f32 ValueF; FromString<f32>(&ValueF, Value);
    Dst->SetSafeValue(ValueF);

    return;
}

template<> FORCEINLINE constexpr void FromString<Jafg::LPreferencef64>(Jafg::LPreferencef64* Dst, LString const& Value) noexcept
{
    check( Dst )
    f64 ValueD; FromString<f64>(&ValueD, Value);
    Dst->SetSafeValue(ValueD);

    return;
}

template<typename TSubField> FORCEINLINE constexpr void FromString(Jafg::TPreference<TSubField>* Dst, LString const& Value) noexcept
    requires requires(TSubField* SubField, LString const& SubValue){ FromString(SubField, SubValue); }
{
    Serialization::FromString<TSubField>(&Dst->Value, Value);
}

} /* ~Namespace Serialization */

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
