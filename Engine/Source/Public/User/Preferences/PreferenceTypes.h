// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# Struct to determine whether to use the default TPreference implementation.
template<typename T> struct TUseDefaultPreferenceImpl : std::false_type {};
template<typename T> struct TUseDefaultPreferenceImpl<T*> : std::false_type {};
template<typename T> struct TUseDefaultPreferenceImpl<T const*> : std::false_type {};
template<typename T> struct TUseDefaultPreferenceImpl<T&> : std::false_type {};
template<typename T> struct TUseDefaultPreferenceImpl<T const&> : std::false_type {};

template<typename T> requires std::is_integral_v<T> || std::is_floating_point_v<T>
struct TUseDefaultPreferenceImpl<T> : std::true_type {};
template<> struct TUseDefaultPreferenceImpl<bool>    : std::true_type {};
template<> struct TUseDefaultPreferenceImpl<LString> : std::true_type {};
template<> struct TUseDefaultPreferenceImpl<LColor>  : std::true_type {};

template<typename T> requires std::is_enum_v<T>
struct TUseDefaultPreferenceImpl<T> : std::true_type {};

template<typename T>
inline constexpr bool TUseDefaultPreferenceImpl_v{TUseDefaultPreferenceImpl<T>::value};

//# Base class for all preferences.
template<typename T, typename TEnable = void>
struct TPreference;

//# Stores default value and value with min-max security.
template<typename T> requires std::is_copy_constructible_v<T>
    && std::is_copy_assignable_v<T>
    && requires(T t){{t < t}->std::convertible_to<bool>;}
    && requires(T t){{t > t}->std::convertible_to<bool>;}
struct TPreference<T, std::enable_if_t<TUseDefaultPreferenceImpl_v<T>>>
{
    inline static constexpr bool HasMinMax_v{true};

    FORCEINLINE constexpr TPreference(T const& InValue)
        noexcept(std::is_nothrow_constructible_v<T>)
        : DefaultValue(InValue), Value(InValue) { }
    FORCEINLINE constexpr TPreference(T const& InValue, std::optional<T> const& InMinValue, std::optional<T> const& InMaxValue)
        noexcept(std::is_nothrow_constructible_v<T>)
        requires std::is_copy_assignable_v<T>
        : MinValue{InMinValue}, MaxValue{InMaxValue}
    {
        check(!(this->MinValue.has_value() && this->MaxValue.has_value()) || *this->MinValue <= *this->MaxValue)
        if (this->MinValue.has_value() && InValue < *this->MinValue)
        {
            this->DefaultValue = *this->MinValue;
            this->Value = *this->MinValue;
        }
        else if (this->MaxValue.has_value() && InValue > *this->MaxValue)
        {
            this->DefaultValue = *this->MaxValue;
            this->Value = *this->MaxValue;
        }
        else
        {
            this->DefaultValue = InValue;
            this->Value = InValue;
        }
        return;
    }
    FORCEINLINE constexpr TPreference& operator=(this auto&& Self, T const& InValue) noexcept(std::is_nothrow_copy_assignable_v<T>)
        requires std::is_copy_assignable_v<T>
    {
        Self.Value = InValue;
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE constexpr TPreference& operator=(this auto&& Self, T&& Other) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires std::is_move_assignable_v<T>
    {
        Self.Value = std::move(Other);
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE ~TPreference() noexcept(std::is_nothrow_destructible_v<T>) = default;

    template<bool bLog = true>
    FORCEINLINE void SetValueSafe(T const& NewValue) noexcept(std::is_nothrow_copy_assignable_v<T>) requires std::is_copy_assignable_v<T>
    {
        if (this->MinValue.has_value() && NewValue < this->MinValue.value())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "Value of type [{}] is smaller then their allowed min value: [{} < {}]"
                    , GetTypeName<TPreference>(), Serde::ToString(NewValue), Serde::ToString(this->MinValue.value())
                    )
            }
            this->Value = this->MinValue.value();
        }
        else if (MaxValue.has_value() && NewValue > this->MaxValue.value())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "Value of type [{}] is greater then their allowed max value: [{} > {}]"
                    , GetTypeName<TPreference>(), Serde::ToString(NewValue), Serde::ToString(this->MaxValue.value())
                    )
            }
            this->Value = this->MaxValue.value();
        }
        else
        {
            this->Value = NewValue;
        }
    }
    template<bool bLog = true>
    FORCEINLINE void SetValueSafe(T&& NewValue) noexcept(std::is_nothrow_move_assignable_v<T>) requires std::is_move_assignable_v<T>
    {
        if (this->MinValue.has_value() && NewValue < this->MinValue.value())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "Value of type [{}] is smaller then their allowed min value: [{} < {}]"
                    , GetTypeName<TPreference>(), Serde::ToString(NewValue), Serde::ToString(this->MinValue.value())
                    )
            }
            this->Value = this->MinValue.value();
        }
        else if (MaxValue.has_value() && NewValue > this->MaxValue.value())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "Value of type [{}] is greater then their allowed max value: [{} > {}]"
                    , GetTypeName<TPreference>(), Serde::ToString(NewValue), Serde::ToString(this->MaxValue.value())
                    )
            }
            this->Value = this->MaxValue.value();
        }
        else
        {
            this->Value = std::move(NewValue);
        }
    }

    FORCEINLINE explicit constexpr operator T&() noexcept { return this->Value; }
    FORCEINLINE explicit constexpr operator T const&() const noexcept { return this->Value; }
    FORCEINLINE constexpr T& operator*() noexcept { return this->Value; }
    FORCEINLINE constexpr T const& operator*() const noexcept { return this->Value; }

    FORCEINLINE constexpr T const& GetDefaultValue() const noexcept { return this->DefaultValue; }
    FORCEINLINE constexpr T const& GetCurrentValue() const noexcept { return this->Value; }

    FORCEINLINE constexpr bool IsMinMaxValueValid() const noexcept { return this->MinValue.has_value() && this->MaxValue.has_value(); }
    FORCEINLINE constexpr bool IsMinValueValid() const noexcept { return this->MinValue.has_value(); }
    FORCEINLINE constexpr bool IsMaxValueValid() const noexcept { return this->MaxValue.has_value(); }
    FORCEINLINE constexpr T const& GetMinValue() const { return this->MinValue.value(); }
    FORCEINLINE constexpr T const& GetMaxValue() const { return this->MaxValue.value(); }

    FORCEINLINE constexpr bool operator==(TPreference const& Other) const noexcept
        requires requires(T t){{t == t}->std::convertible_to<bool>;}
    {
        return this->Value == Other.Value;
    }
    FORCEINLINE constexpr decltype(auto) operator<=>(TPreference const& Other) const noexcept
        requires std::three_way_comparable<T>
    {
        return this->Value <=> Other.Value;
    }

    FORCEINLINE constexpr bool operator==(T const& Other) const noexcept
        requires requires(T t){{t == t}->std::convertible_to<bool>;}
    {
        return this->Value == Other;
    }
    FORCEINLINE constexpr decltype(auto) operator<=>(T const& Other) const noexcept
        requires std::three_way_comparable<T>
    {
        return this->Value <=> Other;
    }

    T DefaultValue;
    T Value;
    std::optional<T> MinValue;
    std::optional<T> MaxValue;
};

//# Stores default value and value.
template<typename T> requires std::is_copy_constructible_v<T>
    && (!requires(T t){{t < t}->std::convertible_to<bool>;} || !std::is_copy_assignable_v<T>)
    && (!requires(T t){{t > t}->std::convertible_to<bool>;} || !std::is_copy_assignable_v<T>)
struct TPreference<T, std::enable_if_t<TUseDefaultPreferenceImpl_v<T>>>
{
    inline static constexpr bool HasMinMax_v{};

    FORCEINLINE constexpr TPreference(T const& InValue) noexcept(std::is_nothrow_constructible_v<T>)
        : DefaultValue(InValue), Value(InValue) { }
    FORCEINLINE constexpr TPreference& operator=(this auto&& Self, T const& InValue) noexcept(std::is_nothrow_copy_assignable_v<T>)
        requires std::is_copy_assignable_v<T>
    {
        Self.Value = InValue;
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE constexpr TPreference& operator=(this auto&& Self, T&& Other) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires std::is_move_assignable_v<T>
    {
        Self.Value = std::move(Other);
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE ~TPreference() noexcept(std::is_nothrow_destructible_v<T>) = default;

    FORCEINLINE explicit constexpr operator T&() noexcept { return this->Value; }
    FORCEINLINE explicit constexpr operator T const&() const noexcept { return this->Value; }
    FORCEINLINE constexpr T& operator*() noexcept { return this->Value; }
    FORCEINLINE constexpr T const& operator*() const noexcept { return this->Value; }

    FORCEINLINE constexpr T const& GetDefaultValue() const noexcept { return this->DefaultValue; }
    FORCEINLINE constexpr T const& GetCurrentValue() const noexcept { return this->Value; }

    FORCEINLINE constexpr bool operator==(TPreference const& Other) const noexcept
        requires requires(T t){{t == t}->std::convertible_to<bool>;}
    {
        return this->Value == Other.Value;
    }
    FORCEINLINE constexpr decltype(auto) operator<=>(TPreference const& Other) const noexcept
        requires std::three_way_comparable<T>
    {
        return this->Value <=> Other.Value;
    }

    FORCEINLINE constexpr bool operator==(T const& Other) const noexcept
        requires requires(T t){{t == t}->std::convertible_to<bool>;}
    {
        return this->Value == Other;
    }
    FORCEINLINE constexpr decltype(auto) operator<=>(T const& Other) const noexcept
        requires std::three_way_comparable<T>
    {
        return this->Value <=> Other;
    }

    T DefaultValue;
    T Value;
};

} /* ~Namespace Jafg */

template<typename T, typename TArchive> requires Serde::CSerializable<T, TArchive>
    && Serde::IsTextOArchive_v<TArchive>
struct Serde::TSerializer<Jafg::TPreference<T>, TArchive>
{
    void operator()(TArchive& Ar, Jafg::TPreference<T> const& Field) const noexcept
    {
        TSerializer<T, TArchive>{}(Ar, Field.Value);
    }
};
template<typename T, typename TArchive> requires Serde::CDeserializable<T, TArchive>
    && (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>)
    && Serde::IsTextIArchive_v<TArchive>
struct Serde::TDeserializer<Jafg::TPreference<T>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, Jafg::TPreference<T>& Field) const noexcept
    {
        if (auto R{TDeserializer<T, TArchive>{}(Ar, Field.Value)}; R.Errc == decltype(R.Errc){})
        {
            if constexpr (Jafg::TPreference<T>::HasMinMax_v)
            {
                if constexpr (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
                {
                    T Temp{std::move(Field.Value)};
                    Field.SetValueSafe(std::move(Temp));
                }
                else if constexpr (std::is_move_constructible_v<T>)
                {
                    T Temp{std::move(Field.Value)};
                    Field.SetValueSafe(Temp);
                }
                else if constexpr (std::is_move_assignable_v<T>)
                {
                    T Temp{Field.Value};
                    Field.SetValueSafe(std::move(Temp));
                }
                else
                {
                    T Temp{Field.Value};
                    Field.SetValueSafe(Temp);
                }
            }
            return R;
        }
        else
        {
            if constexpr (std::is_copy_assignable_v<T>)
            {
                LOG_ERROR(LogPreferences, "Config [{}] could not interpret [{}] as [{}]. Failed with [{}]: {}"
                    , Jafg::GetTypeName<Jafg::TPreference<T>>(), Ar.Stream, Jafg::GetTypeName<T>()
                    , std::to_underlying(R.Errc), R.Error.has_value() ? R.Error.value() : "<unknown-error>"
                    )
                if constexpr (Jafg::TPreference<T>::HasMinMax_v)
                {
                    Field.SetValueSafe(Field.DefaultValue);
                }
                else
                {
                    Field.Value = Field.DefaultValue;
                }
            }
            else
            {
                LOG_FATAL(LogPreferences, "Config [{}] could not interpret [{}] as [{}]. Failed with [{}]: {}"
                    , Jafg::GetTypeName<Jafg::TPreference<T>>(), Ar.Stream, Jafg::GetTypeName<T>()
                    , std::to_underlying(R.Errc), R.Error.has_value() ? R.Error.value() : "<unknown-error>"
                    )
            }
        }
        return {};
    }
};

template<typename T>
struct std::formatter<Jafg::TPreference<T>> : std::formatter<T>
{
    FORCEINLINE std::format_context::iterator format(Jafg::TPreference<T> const& InPreference, std::format_context& InContext) const
    {
        return std::formatter<T>::format(InPreference.Value, InContext);
    }
};
