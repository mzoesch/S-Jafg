// Copyright mzoesch. All rights reserved.

#pragma once

namespace Smart
{

template <typename T>
concept IsValidUniqueType =
       !std::is_reference_v<T>
    && !std::is_pointer_v<T>
    && !std::is_same_v<T, LNullptrTy>;

//#
//# Jafg implementation of a non-intrusive unique pointer.
//#
template <typename T> requires IsValidUniqueType<T>
struct TUniqueBase
{
    typedef T        Element;
    typedef T*       Pointer;
    typedef T const* ConstPointer;

    FORCEINLINE constexpr TUniqueBase(LNullptrTy) noexcept : Object(nullptr) { }
    template <typename U> requires (std::is_convertible_v<U, Pointer> || std::is_convertible_v<U, LNullptrTy>)
    FORCEINLINE constexpr explicit TUniqueBase(U&& u) noexcept : Object(std::forward<U>(u)) { }
    FORCEINLINE constexpr ~TUniqueBase() noexcept = default;

    //# @return Whether the pointer is null or not.
    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Object != nullptr; }

    //# Releases the pointer without deleting it.
    FORCEINLINE constexpr Pointer Release() noexcept { const Pointer Out { this->Object }; this->Object = nullptr; return Out; }

    FORCEINLINE constexpr Element&       GetValue()       noexcept { check( this->Object ) return *this->Object; }
    FORCEINLINE constexpr Element const& GetValue() const noexcept { check( this->Object ) return *this->Object; }
    FORCEINLINE constexpr Element&       GetValue(      Element& Default)       noexcept { if (this->Object) { return *this->Object; } return Default; }
    FORCEINLINE constexpr Element const& GetValue(const Element& Default) const noexcept { if (this->Object) { return *this->Object; } return Default; }

    FORCEINLINE constexpr      Pointer GetPointer()       noexcept { return this->Object; }
    FORCEINLINE constexpr ConstPointer GetPointer() const noexcept { return this->Object; }
    FORCEINLINE constexpr      Pointer GetPointerChecked()       noexcept { check( this->Object ) return this->Object; }
    FORCEINLINE constexpr ConstPointer GetPointerChecked() const noexcept { check( this->Object ) return this->Object; }
    FORCEINLINE constexpr      Pointer GetPointerAsserted()       noexcept { jassert( this->Object ) return this->Object; }
    FORCEINLINE constexpr ConstPointer GetPointerAsserted() const noexcept { jassert( this->Object ) return this->Object; }

    template <typename U> requires (std::is_integral_v<U>)
    FORCEINLINE constexpr Element&       operator[](const U u)       noexcept { check( this->Object ) return this->Object[u]; }
    template <typename U> requires (std::is_integral_v<U>)
    FORCEINLINE constexpr Element const& operator[](const U u) const noexcept { check( this->Object ) return this->Object[u]; }

    FORCEINLINE constexpr operator      Pointer()       noexcept { return this->Object; }
    FORCEINLINE constexpr operator ConstPointer() const noexcept { return this->Object; }
    FORCEINLINE constexpr      Pointer operator->()       noexcept { return this->Object; }
    FORCEINLINE constexpr ConstPointer operator->() const noexcept { return this->Object; }

    FORCEINLINE constexpr Element&       operator*()       { return this->GetValue(); }
    FORCEINLINE constexpr Element const& operator*() const { return this->GetValue(); }

    FORCEINLINE constexpr bool operator!() const noexcept { return !this->Object; }

protected:

    Pointer Object;
};

namespace Private
{

template <typename T, typename U>
concept IsValidOtherPointerForUnique =
       std::is_same_v<U, LNullptrTy>
    ||
    (
           std::is_pointer_v<U>
        && std::is_convertible_v<std::remove_pointer_t<U>*, T*>
    )
    ;

} /* ~Namespace Private */

template <typename T>
struct TUnique : public TUniqueBase<T>
{
    /* We cannot use SFINAE, as TUnique<T[]> is a specialization of this primary template. */
    static_assert(std::is_array_v<T> == false);

    typedef TUniqueBase<T>         Base;
    typedef typename Base::Element Element;
    typedef typename Base::Pointer Pointer;

    FORCEINLINE constexpr TUnique() noexcept : Base(nullptr) { }
    FORCEINLINE constexpr TUnique(LNullptrTy) noexcept : Base(nullptr) { }
    PROHIBIT_COPY(TUnique)
    FORCEINLINE constexpr TUnique(TUnique&& Other) noexcept : Base(std::move(Other.Object)) { Other.Object = nullptr; }
    FORCEINLINE constexpr TUnique& operator=(TUnique&& Other) noexcept { delete this->Object; this->Object = Other.Object; Other.Object = nullptr; return *this; }
    template <typename U> requires (Private::IsValidOtherPointerForUnique<T, U>)
    FORCEINLINE constexpr explicit TUnique(U u) noexcept : Base(static_cast<Pointer>(u)) { }
    template <typename U> requires (std::is_convertible_v<U*, T*>)
    FORCEINLINE constexpr TUnique(TUnique<U>&& u) noexcept : Base(static_cast<Pointer>(u.GetPointer())) { u.Release(); }
    FORCEINLINE constexpr ~TUnique() noexcept { this->Reset(); }

    FORCEINLINE constexpr void Set(Pointer P) noexcept { delete this->Object; this->Object = P; }
    FORCEINLINE constexpr void Reset() noexcept { delete this->Object; this->Object = nullptr; }

    FORCEINLINE constexpr bool operator== (const TUnique& Other) const noexcept { return this->Object == Other.Object; }
    FORCEINLINE constexpr bool operator<=>(const TUnique& Other) const noexcept { return this->Object <=> Other.Object; }

    template <typename U>
    FORCEINLINE constexpr bool operator== (const U& Other) const noexcept { return this->Object == Other; }
    template <typename U>
    FORCEINLINE constexpr bool operator<=>(const U& Other) const noexcept { return this->Object <=> Other; }
};

template <typename T>
struct TUnique<T[]> : public TUniqueBase<T>
{
    typedef TUniqueBase<T>         Base;
    typedef typename Base::Element Element;
    typedef typename Base::Pointer Pointer;

    FORCEINLINE constexpr TUnique() noexcept : Base(nullptr) { }
    FORCEINLINE constexpr TUnique(LNullptrTy) noexcept : Base(nullptr) { }
    PROHIBIT_COPY(TUnique)
    FORCEINLINE constexpr TUnique(TUnique&& Other) noexcept : Base(std::move(Other.Object)) { Other.Object = nullptr; }
    FORCEINLINE constexpr TUnique& operator=(TUnique&& Other) noexcept { delete[] this->Object; this->Object = Other.Object; Other.Object = nullptr; return *this; }
    template <typename U> requires
    (
           std::is_same_v<U, Pointer>
        || std::is_same_v<U, LNullptrTy>
        ||
        (
               std::is_pointer_v<U>
            && std::is_same_v<U, T*>
            && std::is_convertible_v<std::remove_pointer_t<U>(*)[], Element(*)[]>
        )
    )
    FORCEINLINE constexpr explicit TUnique(U u) noexcept : Base(static_cast<Pointer>(u)) { }
    FORCEINLINE constexpr ~TUnique() noexcept { this->Reset(); }

    FORCEINLINE constexpr void Set(Pointer P) noexcept { delete[] this->Object; this->Object = P; }
    FORCEINLINE constexpr void Reset() noexcept { delete[] this->Object; this->Object = nullptr; }

    FORCEINLINE constexpr bool operator== (const TUnique& Other) const noexcept { return this->Object == Other.Object; }
    FORCEINLINE constexpr bool operator<=>(const TUnique& Other) const noexcept { return this->Object <=> Other.Object; }

    template <typename U>
    FORCEINLINE constexpr bool operator== (const U& Other) const noexcept { return this->Object == Other; }
    template <typename U>
    FORCEINLINE constexpr bool operator<=>(const U& Other) const noexcept { return this->Object <=> Other; }
};

//# Emplace a new object of type T with the arguments InArgs.
template <typename T, typename... TArgs>
FORCEINLINE TUnique<T> EmplaceUnique(TArgs&&... Args) noexcept
{
    return TUnique<T>{ new T(std::forward<TArgs>(Args)...) };
}

//# Construct a new object of type U that creates a unique pointer of type T with the arguments InArgs.
template <typename T, typename U, typename... TArgs>
FORCEINLINE TUnique<T> EmplaceUniqueOfType(TArgs&&... Args) noexcept
{
    return TUnique<T>{ new U(std::forward<TArgs>(Args)...) };
}

} /* ~Namespace Smart */
