// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

template<typename T>
struct TCdrIgnoreBase
{
    static_assert(std::is_default_constructible_v<T>);

    T value; // Small "v" to preserve stl convention.

    constexpr TCdrIgnoreBase() noexcept(std::is_nothrow_default_constructible_v<T>)
        : value() {}

    template<typename... TArgs> requires std::is_constructible_v<T, TArgs&&...>
    constexpr explicit TCdrIgnoreBase(TArgs&&... Args) noexcept(std::is_nothrow_constructible_v<T, TArgs&&...>)
        : value(std::forward<TArgs>(Args)...) {}

    template<typename U> requires std::is_assignable_v<T&, U&&>
    constexpr TCdrIgnoreBase& operator=(U&& Other) noexcept(std::is_nothrow_assignable_v<T&, U&&>)
    {
        this->value = std::forward<U>(Other);
        return *this;
    }

    //# Ignore the CDR value and default construct instead.
    constexpr TCdrIgnoreBase(TCdrIgnoreBase const&) noexcept(std::is_nothrow_default_constructible_v<T>)
        : value{} {}

    //# J-Objects cannot be copy assigned.
    TCdrIgnoreBase& operator=(TCdrIgnoreBase const&) noexcept = delete;

    //# J-Objects cannot be moved.
    TCdrIgnoreBase(TCdrIgnoreBase&&) noexcept = delete;
    TCdrIgnoreBase& operator=(TCdrIgnoreBase&&) noexcept = delete;

    constexpr operator T&() & noexcept { return this->value; }
    constexpr operator T const&() const& noexcept { return this->value; }
    //# J-Objects cannot be moved.
    constexpr operator T&&() && noexcept = delete;

    constexpr T* operator->() noexcept { return std::addressof(this->value); }
    constexpr T const* operator->() const noexcept { return std::addressof(this->value); }

    constexpr T& operator*() & noexcept { return this->value; }
    constexpr T const& operator*() const& noexcept { return this->value; }
    //# J-Objects cannot be moved.
    constexpr T&& operator*() && noexcept = delete;

    constexpr T& get() & noexcept { return this->value; }
    constexpr T const& get() const& noexcept { return this->value; }
    //# J-Objects cannot be moved.
    constexpr T&& get() && noexcept = delete;

    constexpr T* get_ptr() noexcept { return std::addressof(this->value); }
    constexpr T const* get_ptr() const noexcept { return std::addressof(this->value); }

    constexpr T* operator&() & noexcept { return std::addressof(this->value); }
    constexpr T const* operator&() const& noexcept { return std::addressof(this->value); }

    constexpr decltype(auto) operator[](auto&& Arg) noexcept
        requires requires(T t, decltype(Arg) a) { t[a]; }
    {
        return this->value[std::forward<decltype(Arg)>(Arg)];
    }

    constexpr decltype(auto) operator[](auto&& Arg) const noexcept
    requires requires(T t, decltype(Arg) a) { t[a]; }
    {
        return this->value[std::forward<decltype(Arg)>(Arg)];
    }

    constexpr decltype(auto) begin() noexcept requires requires(T t) { t.begin(); } {return this->value.begin(); }
    constexpr decltype(auto) begin() const noexcept requires requires(T t) { t.begin(); } { return this->value.begin(); }
    constexpr decltype(auto) end() noexcept requires requires(T t) { t.end(); } { return this->value.end(); }
    constexpr decltype(auto) end() const noexcept requires requires(T t) { t.end(); } { return this->value.end(); }
    constexpr decltype(auto) rbegin() noexcept requires requires(T t) { t.rbegin(); } { return this->value.rbegin(); }
    constexpr decltype(auto) rbegin() const noexcept requires requires(T t) { t.rbegin(); } { return this->value.rbegin(); }
    constexpr decltype(auto) rend() noexcept requires requires(T t) { t.rend(); } { return this->value.rend(); }
    constexpr decltype(auto) rend() const noexcept requires requires(T t) { t.rend(); } { return this->value.rend(); }
    constexpr decltype(auto) cbegin() const noexcept requires requires(T t) { t.cbegin(); } { return this->value.cbegin(); }
    constexpr decltype(auto) cend() const noexcept requires requires(T t) { t.cend(); } { return this->value.cend(); }
    constexpr decltype(auto) crbegin() const noexcept requires requires(T t) { t.crbegin(); } { return this->value.crbegin(); }
    constexpr decltype(auto) crend() const noexcept requires requires(T t) { t.crend(); } { return this->value.crend(); }
};

template<typename T>
struct TCdrIgnoreBase<T*>
{
    T* value;

    //# Explicitly keep the UB.
    constexpr TCdrIgnoreBase() noexcept {}
    constexpr TCdrIgnoreBase(LNullptrTy) noexcept : value(nullptr) {}

    template<typename U> requires std::is_convertible_v<U*, T*>
    constexpr TCdrIgnoreBase(U* Ptr) noexcept : value(Ptr) {}
    constexpr TCdrIgnoreBase& operator=(LNullptrTy) noexcept { this->value = nullptr; return *this; }
    template<typename U> requires std::is_convertible_v<U*, T*>
    constexpr TCdrIgnoreBase& operator=(U* Ptr) noexcept { this->value = Ptr; return *this; }

    constexpr TCdrIgnoreBase(TCdrIgnoreBase const&) noexcept
        : value(nullptr) {}

    //# J-Objects cannot be copy assigned.
    TCdrIgnoreBase& operator=(TCdrIgnoreBase const&) noexcept = delete;

    //# J-Objects cannot be moved.
    TCdrIgnoreBase(TCdrIgnoreBase&&) noexcept = delete;
    TCdrIgnoreBase& operator=(TCdrIgnoreBase&&) noexcept = delete;

    constexpr bool operator==(LNullptrTy) const noexcept { return this->value == nullptr; }

    constexpr operator T*() const noexcept { return this->value; }
    template<typename U> requires(std::is_same_v<T, U> == false && std::is_convertible_v<T*, U*>)
    constexpr operator U*() const noexcept { return this->value; }

    constexpr T* operator->() noexcept { return this->value; }
    constexpr T const* operator->() const noexcept { return this->value; }

    constexpr decltype(auto) operator*() & noexcept requires(!std::is_void_v<T>) { return *this->value; }
    constexpr decltype(auto) operator*() const& noexcept requires(!std::is_void_v<T>) { return *this->value; }
    constexpr decltype(auto) operator*() && noexcept requires(!std::is_void_v<T>) { return std::move(*this->value); }

    constexpr T** operator&() & noexcept { return std::addressof(this->value); }
    constexpr T* const* operator&() const& noexcept { return std::addressof(this->value); }

    constexpr decltype(auto) operator[](auto&& Arg) noexcept
        requires requires(T t, decltype(Arg) a) { t[a]; }
    {
        return this->value[std::forward<decltype(Arg)>(Arg)];
    }

    constexpr decltype(auto) operator[](auto&& Arg) const noexcept
        requires requires(T t, decltype(Arg) a) { t[a]; }
    {
        return this->value[std::forward<decltype(Arg)>(Arg)];
    }
};

//#
//# Expect a default value for the CDR member.
//#
//# Overload #Jafg::IsCdrMemberConsideredDefault function for your type T, if you want to use it with the expected check.
//#
template<typename T> NODISCARD FORCEINLINE bool IsCdrMemberConsideredDefault(T const& Value) noexcept = delete;
template<typename T> NODISCARD FORCEINLINE bool IsCdrMemberConsideredDefault(T* const& Value) noexcept { return Value == nullptr; }
template<typename T>
NODISCARD FORCEINLINE bool IsCdrMemberConsideredDefault(TArray<T> const& Value) noexcept { return Value.empty(); }
template<typename T>
struct TCdrExpectDefaultBase : public TCdrIgnoreBase<T>
{
    using TCdrIgnoreBase<T>::TCdrIgnoreBase;
    using TCdrIgnoreBase<T>::operator=;
    using TCdrIgnoreBase<T>::operator->;
    // using TCdrIgnoreBase<T>::operator*;
    using TCdrIgnoreBase<T>::operator[];
    using TCdrIgnoreBase<T>::operator&;

    //# Ignore the CDR value and default construct instead.
    constexpr TCdrExpectDefaultBase(TCdrExpectDefaultBase const& CDRMember) noexcept(std::is_nothrow_default_constructible_v<T>)
        : TCdrIgnoreBase<T>{static_cast<TCdrIgnoreBase<T> const&>(CDRMember)}
    {
        check( Jafg::IsCdrMemberConsideredDefault(CDRMember.value) )
    }
};

} /* ~Namespace Jafg */

//# Ignores the CDR value of the underlying type T and default constructs it instead.
template<typename T>
using TCdrIgnore = Jafg::TCdrIgnoreBase<T>;
//#
//# Expects the CDR value to be some kind of default value.
//# Overload the function #Jafg::IsCdrMemberConsideredDefault for your type T.
//#
#if JAFG_DO_CHECKS
    template<typename T>
    using TCdrExpectDefault = Jafg::TCdrExpectDefaultBase<T>;
#else /* JAFG_DO_CHECKS */
    template<typename T>
    using TCdrExpectDefault = TCdrIgnore<T>;
#endif /* !JAFG_DO_CHECKS */

//#
//# During a copy operation, the cdr array has to be empty.
//# This can also be used for arrays of non-copyable types. The array will be default constructed instead of
//# copy-constructed.
//#
template<typename T> requires std::is_same_v<T, TArray<typename T::value_type, typename T::allocator_type>>
using TCdrEmptyArray = TCdrExpectDefault<T>;

//# When in a development check that the underlying pointer is indeed null.
#if JAFG_DO_CHECKS
    #define CDR_NULL_PTR(...)                                           TCdrExpectDefault<__VA_ARGS__>
#else /* JAFG_DO_CHECKS */
    #define CDR_NULL_PTR(...)                                           __VA_ARGS__
#endif /* !JAFG_DO_CHECKS */

namespace Serialization
{

template<typename TSubField>
NODISCARD FORCEINLINE constexpr LString ToString(TCdrIgnore<TSubField> const& Field) noexcept
    requires requires(TSubField const& SubField){ ToString(SubField); }
{
    return ToString(Field.value);
}

template<typename TSubField>
FORCEINLINE constexpr void FromString(TCdrIgnore<TSubField>* Field, LString const& Value) noexcept
    requires requires(TSubField* SubField, LString const& SubValue){ FromString(SubField, SubValue); }
{
    check( Field )
    FromString(std::addressof(Field->value), Value);
    return;
}

} /* ~Namespace Serialization */

#include "Engine/CdrMemberManipulationCoreTypes.h"
