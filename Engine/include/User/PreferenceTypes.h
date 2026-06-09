// Copyright mzoesch. All rights reserved.

#pragma once

//# Base class for all preferences.
template<typename T>
struct TPreference;
//# Base class for all preferences that have min-max security.
template<typename T>
struct TClampedPreference;

namespace Jafg
{

//# Default implementation for TPreference.
template<typename T>
struct TDefaultPreference
{
    static_assert(std::is_copy_constructible_v<T>);
    static_assert(algo::is_weak_eq_v<T,T>);

    FORCEINLINE constexpr TDefaultPreference() noexcept(std::is_nothrow_default_constructible_v<T>)
        requires std::is_default_constructible_v<T>
        : Default{}, Value{} {}
    FORCEINLINE explicit constexpr TDefaultPreference(T const& InValue) noexcept(std::is_nothrow_constructible_v<T>)
        : Default{InValue}, Value{InValue} {}
    FORCEINLINE constexpr TDefaultPreference& operator=(this auto&& Self, T InValue) noexcept(std::is_nothrow_copy_assignable_v<T>)
        requires std::is_move_assignable_v<T>
    {
        Self.Value = std::move(InValue);
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE constexpr ~TDefaultPreference() noexcept(std::is_nothrow_destructible_v<T>) = default;

    NODISCARD FORCEINLINE constexpr T const& GetDefault() const noexcept { return this->Default; }
    NODISCARD FORCEINLINE constexpr T const& GetValue() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr bool IsModified() const noexcept { return !(this->Value == this->Default); }

    NODISCARD FORCEINLINE explicit constexpr operator T&() noexcept { return this->Value; }
    NODISCARD FORCEINLINE explicit constexpr operator T const&() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T& operator*() noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T const& operator*() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T* operator->() noexcept { return std::addressof(this->Value); }
    NODISCARD FORCEINLINE constexpr T const* operator->() const noexcept { return std::addressof(this->Value); }

    NODISCARD FORCEINLINE constexpr bool operator==(T const& Rhs) const noexcept
    {
        return this->Value == Rhs;
    }
    NODISCARD FORCEINLINE constexpr decltype(auto) operator<=>(T const& Rhs) const noexcept
        requires algo::is_weak_eq_three_way_v<T,T>
    {
        return this->Value <=> Rhs;
    }

    T Default;
    T Value;
};

//# Default implementation for TClampedPreference.
template<typename T>
struct TDefaultClampedPreference
{
    static_assert(std::is_copy_constructible_v<T>);
    static_assert(algo::is_weak_eq_v<T,T>);

    FORCEINLINE explicit constexpr TDefaultClampedPreference(T const& InValue) noexcept(std::is_nothrow_constructible_v<T>)
        : Default{InValue}, Value{InValue} {}
    FORCEINLINE constexpr TDefaultClampedPreference& operator=(this auto&& Self, T InValue) noexcept(std::is_nothrow_copy_assignable_v<T>)
        requires std::is_move_assignable_v<T>
    {
        Self.Value = std::move(InValue);
        return std::forward<decltype(Self)>(Self);
    }
    FORCEINLINE constexpr TDefaultClampedPreference(T InValue, std::optional<T> InMinValue, std::optional<T> InMaxValue) noexcept(std::is_nothrow_constructible_v<T>)
        requires std::is_move_assignable_v<T>
        : Default{InValue}, Value{InValue}, MinValue{std::move(InMinValue)}, MaxValue{std::move(InMaxValue)}
    {
        check(!(this->MinValue.has_value() && this->MaxValue.has_value()) || (*this->MinValue <= *this->MaxValue))
        check(!(this->MinValue.has_value()) || this->Value >= *this->MinValue)
        check(!(this->MaxValue.has_value()) || this->Value <= *this->MaxValue)
    }
    FORCEINLINE constexpr ~TDefaultClampedPreference() noexcept(std::is_nothrow_destructible_v<T>) = default;

    template<bool bLog = true>
    FORCEINLINE T& SetValueSafe(T NewValue) noexcept(std::is_nothrow_copy_assignable_v<T>) requires std::is_move_assignable_v<T>
    {
        if (this->IsMinValid() && NewValue < this->GetMin())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "[{}]: Value is smaller then their allowed min value: [{} < {}]"
                    , algo::type_name<TDefaultClampedPreference>(), serde::ToString(NewValue), serde::ToString(this->GetMin())
                    )
            }
            this->Value = this->GetMin();
        }
        else if (this->IsMaxValid() && NewValue > this->GetMax())
        {
            if constexpr (bLog)
            {
                LOG_WARNING(LogPreferences, "[{}]: Value is bigger then their allowed max value: [{} > {}]"
                    , algo::type_name<TDefaultClampedPreference>(), serde::ToString(NewValue), serde::ToString(this->GetMax())
                    )
            }
            this->Value = this->GetMax();
        }
        else
        {
            this->Value = std::move(NewValue);
        }
        return this->Value;
    }

    NODISCARD FORCEINLINE constexpr T const& GetDefault() const noexcept { return this->Default; }
    NODISCARD FORCEINLINE constexpr T const& GetValue() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr bool IsMinValid() const noexcept { return this->MinValue.has_value(); }
    NODISCARD FORCEINLINE constexpr bool IsMaxValid() const noexcept { return this->MaxValue.has_value(); }
    NODISCARD FORCEINLINE constexpr bool IsMinMaxValid() const noexcept { return this->IsMinValid() && this->IsMaxValid(); }
    NODISCARD FORCEINLINE constexpr T const& GetMin() const { return this->MinValue.value(); }
    NODISCARD FORCEINLINE constexpr T const& GetMax() const { return this->MaxValue.value(); }
    NODISCARD FORCEINLINE constexpr bool IsModified() const noexcept { return !(this->Value == this->Default); }

    NODISCARD FORCEINLINE explicit constexpr operator T&() noexcept { return this->Value; }
    NODISCARD FORCEINLINE explicit constexpr operator T const&() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T& operator*() noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T const& operator*() const noexcept { return this->Value; }
    NODISCARD FORCEINLINE constexpr T* operator->() noexcept { return std::addressof(this->Value); }
    NODISCARD FORCEINLINE constexpr T const* operator->() const noexcept { return std::addressof(this->Value); }

    NODISCARD FORCEINLINE constexpr bool operator==(T const& Rhs) const noexcept
    {
        return this->Value == Rhs;
    }
    NODISCARD FORCEINLINE constexpr decltype(auto) operator<=>(T const& Rhs) const noexcept
        requires algo::is_weak_eq_three_way_v<T,T>
    {
        return this->Value <=> Rhs;
    }

    T Default;
    T Value;

private:

    std::optional<T> MinValue;
    std::optional<T> MaxValue;
};

} /* ~Namespace Jafg */

#define JAFG_PREF_OF(type) \
    template<> struct TPreference<type> : public Jafg::TDefaultPreference<type>\
    { \
        using TDefaultPreference::TDefaultPreference; \
        using TDefaultPreference::operator=; \
    };
#define JAFG_CLAMPED_PREF_OF(type) \
    template<> struct TClampedPreference<type> : public Jafg::TDefaultClampedPreference<type>\
    {\
        using TDefaultClampedPreference::TDefaultClampedPreference; \
        using TDefaultClampedPreference::operator=; \
    };

JAFG_PREF_OF(bool)
JAFG_PREF_OF(u8)
JAFG_PREF_OF(u16)
JAFG_PREF_OF(u32)
JAFG_PREF_OF(u64)
JAFG_PREF_OF(i8)
JAFG_PREF_OF(i16)
JAFG_PREF_OF(i32)
JAFG_PREF_OF(i64)
JAFG_PREF_OF(f32)
JAFG_PREF_OF(f64)

JAFG_CLAMPED_PREF_OF(u8)
JAFG_CLAMPED_PREF_OF(u16)
JAFG_CLAMPED_PREF_OF(u32)
JAFG_CLAMPED_PREF_OF(u64)
JAFG_CLAMPED_PREF_OF(i8)
JAFG_CLAMPED_PREF_OF(i16)
JAFG_CLAMPED_PREF_OF(i32)
JAFG_CLAMPED_PREF_OF(i64)
JAFG_CLAMPED_PREF_OF(f32)
JAFG_CLAMPED_PREF_OF(f64)

JAFG_PREF_OF(LString)
JAFG_PREF_OF(LPath)
JAFG_PREF_OF(LColor)

template<typename T>
struct TPreference<TArray<T>> : public Jafg::TDefaultPreference<TArray<T>>
{
    static_assert(std::is_copy_constructible_v<T>);
    static_assert(algo::is_weak_eq_v<T,T>);

    typedef Jafg::TDefaultPreference<TArray<T>> Super;
    using Super::Super;
};

template<typename T, typename TArchive> requires serde::CSerializable<T, TArchive>
    && serde::os_string_archive_v<TArchive>
struct serde::TSerializer<TPreference<T>, TArchive>
{
    void operator()(TArchive& Ar, TPreference<T> const& Field) const noexcept
    {
        TSerializer<T, TArchive>{}(Ar, Field.Value);
    }
};
template<typename T, typename TArchive> requires serde::CSerializable<T, TArchive>
    && serde::os_string_archive_v<TArchive>
struct serde::TSerializer<TClampedPreference<T>, TArchive>
{
    void operator()(TArchive& Ar, TClampedPreference<T> const& Field) const noexcept
    {
        TSerializer<T, TArchive>{}(Ar, Field.Value);
    }
};

template<typename T, typename TArchive> requires serde::CDeserializable<T, TArchive>
    && serde::is_string_archive_v<TArchive>
struct serde::TDeserializer<TPreference<T>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TPreference<T>& Field) const noexcept
    {
        return TDeserializer<T, TArchive>{}(Ar, Field.Value);
    }
};

template<typename T, typename TArchive> requires serde::CDeserializable<T, TArchive>
    && std::is_default_constructible_v<T> && std::is_move_assignable_v<T>
    && serde::is_string_archive_v<TArchive>
struct serde::TDeserializer<TClampedPreference<T>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TClampedPreference<T>& Field) const noexcept
    {
        T Dummy{};
        auto R{TDeserializer<T, TArchive>{}(Ar, Dummy)};
        if (R.Errc == decltype(R.Errc){})
        {
            Field.SetValueSafe(std::move(Dummy));
        }
        return R;
    }
};

template<typename T>
struct std::formatter<TPreference<T>> : std::formatter<T>
{
    FORCEINLINE std::format_context::iterator format(TPreference<T> const& Preference, std::format_context& Context) const
    {
        return std::formatter<T>::format(*Preference, Context);
    }
};

template<typename T>
struct std::formatter<TClampedPreference<T>> : std::formatter<T>
{
    FORCEINLINE std::format_context::iterator format(TClampedPreference<T> const& Preference, std::format_context& Context) const
    {
        return std::formatter<T>::format(*Preference, Context);
    }
};
