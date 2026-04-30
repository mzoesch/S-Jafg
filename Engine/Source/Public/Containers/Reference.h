// Copyright mzoesch. All rights reserved.

#pragma once

//# A type that behaves like a reference but is copyable and assignable.
template<typename T>
struct TReference final
{
private:

    //# For l vals.
    inline static constexpr T* GetAddressOfLValue(T& l) noexcept { return std::addressof(l); }
    //# For r vals.
    inline static constexpr void GetAddressOfLValue(T&& r) noexcept = delete;

public:

    typedef T type;

    template<typename U
        , typename = std::enable_if_t<std::is_same_v<TReference, std::remove_cvref_t<U>> == false>
        , typename = decltype(TReference::GetAddressOfLValue(std::declval<U>()))
        >
    constexpr TReference(U&& u)
        noexcept(noexcept(TReference::GetAddressOfLValue(std::forward<U>(u))))
        : What(TReference::GetAddressOfLValue(std::forward<U>(u)))
    {
    }

    constexpr TReference(TReference const&) noexcept = default;
    constexpr TReference& operator=(TReference const&) noexcept = default;

    inline constexpr T& get() const noexcept { return *this->What; }
    inline constexpr operator T&() const noexcept { return this->get(); }
    inline constexpr T& operator*() const noexcept { return this->get(); }
    inline constexpr T* operator->() const noexcept { return this->What; }
    inline constexpr T* operator&() const noexcept { return &this->get(); }

    template<typename... TArgs>
    constexpr std::invoke_result_t<T&, TArgs...> operator()(TArgs&&... Args) const noexcept(std::is_nothrow_invocable_v<T&, TArgs...>)
    {
        if constexpr (std::is_object_v<T>)
        {
            static_assert(algo::detail::is_complete_type_v<T>);
        }

        return std::invoke(this->get(), std::forward<TArgs>(Args)...);
    }

    NODISCARD friend constexpr bool operator==(TReference Lhs, TReference Rhs)
        requires requires {{Lhs.get() == Rhs.get()} -> std::convertible_to<bool>; }
    {
        return Lhs.get() == Rhs.get();
    }
    NODISCARD friend constexpr bool operator==(TReference Lhs, T const& Rhs)
        requires requires {{Lhs.get() == Rhs} -> std::convertible_to<bool>; }
    {
        return Lhs.get() == Rhs;
    }

    NODISCARD friend constexpr bool operator==(TReference Lhs, TReference<T const> Rhs)
        requires (!std::is_const_v<T>) && requires {{Lhs.get() == Rhs.get()} -> std::convertible_to<bool>; }
    {
        return Lhs.get() == Rhs.get();
    }

    NODISCARD friend constexpr auto operator<=>(TReference Lhs, TReference Rhs)
        requires requires (T const t){{ t < t } -> std::convertible_to<bool>;}
    {
        return Lhs.get() <= Rhs.get();
    }

    NODISCARD friend constexpr auto operator<=>(TReference Lhs, T const& Rhs)
        requires requires {{Rhs < Rhs} -> std::convertible_to<bool>;}
    {
        return Lhs.get() <=> Rhs;
    }

    NODISCARD friend constexpr auto operator<=>(TReference Lhs, TReference<T const> Rhs)
        requires (!std::is_const_v<T>) && requires (T const t){{ t < t } -> std::convertible_to<bool>;}
    {
        return Lhs.get() <= Rhs.get();
    }

private:

    T* What;
};
