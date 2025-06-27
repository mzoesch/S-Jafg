// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"

namespace Jafg
{

//#
//# This struct is meant as a wrapper class for storing object pointers for a longer time.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
template <typename T>
struct TObjectStorage final
{
    static_assert(std::is_base_of_v<JObjectBase, T>);

    FORCEINLINE constexpr TObjectStorage() noexcept;
    FORCEINLINE constexpr TObjectStorage(LNullptrTy) noexcept;
    FORCEINLINE constexpr TObjectStorage(T* InPointer) noexcept;
    FORCEINLINE constexpr TObjectStorage(LObjectContext* InContextPointer, JObjectBase* InPointer) noexcept;
    FORCEINLINE constexpr TObjectStorage(const TObjectStorage& InOther) noexcept;
    FORCEINLINE constexpr TObjectStorage(TObjectStorage&& InOther) noexcept;
    FORCEINLINE constexpr TObjectStorage& operator=(LNullptrTy) noexcept;
    FORCEINLINE constexpr TObjectStorage& operator=(T* InPointer) noexcept;
    FORCEINLINE constexpr TObjectStorage& operator=(const TObjectStorage& InOther) noexcept;
    FORCEINLINE constexpr TObjectStorage& operator=(TObjectStorage&& InOther) noexcept;

    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage(TOther* InPointer) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage(const TObjectStorage<TOther>& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage(TObjectStorage<TOther>&& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage& operator=(TOther* InPointer) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage& operator=(const TObjectStorage<TOther>& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr TObjectStorage& operator=(TObjectStorage<TOther>&& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);

    //# Nulls the underlying object out.
    FORCEINLINE constexpr void Reset() noexcept;

    FORCEINLINE constexpr bool operator==(LNullptrTy) const noexcept;
    FORCEINLINE constexpr bool operator!=(LNullptrTy) const noexcept;
    FORCEINLINE constexpr bool operator==(T* InOther) const noexcept;
    FORCEINLINE constexpr bool operator!=(T* InOther) const noexcept;
    template <typename TOther>
    FORCEINLINE constexpr bool operator==(TOther* InOther) const noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    template <typename TOther>
    FORCEINLINE constexpr bool operator!=(TOther* InOther) const noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>);
    FORCEINLINE constexpr bool operator==(const TObjectStorage& InOther) const noexcept;
    FORCEINLINE constexpr bool operator!=(const TObjectStorage& InOther) const noexcept;

    //#
    //# Checks whether the underlying object pointer is null.
    //# Equivalent to if (Pointer) { ... }
    //#
    FORCEINLINE constexpr bool IsNull() const noexcept;
    FORCEINLINE constexpr bool IsNotNull() const noexcept { return this->IsNull() == false; }

    //#
    //# A very fast check if the #Pointer is still valid.
    //# Requires that the context is still valid.
    //#
    //# @see #IsValidFast function in Engine/ObjectBaseUtility.h
    //#
    FORCEINLINE bool IsValid() const noexcept;

    //#
    //# A check if the #Pointer is still valid. This check will also check if the context has gotten out of scope.
    //# Use this only in cases where the caller is outside the context, where the pointed object is home.
    //#
    //# @see #IsValidSlow function in Engine/ObjectBaseUtility.h
    //#
    FORCEINLINE bool IsValidDeep() const noexcept;

    FORCEINLINE       T* operator->() noexcept;
    FORCEINLINE const T* operator->() const noexcept;
    FORCEINLINE       T* Get() noexcept;
    FORCEINLINE const T* Get() const noexcept;
    FORCEINLINE       T& operator*() noexcept;
    FORCEINLINE const T& operator*() const noexcept;

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsNotNull(); }

    FORCEINLINE operator       T*() noexcept { return this->Get(); }
    FORCEINLINE operator const T*() const noexcept { return this->Get(); }

private:

#if LAL_DO_CHECKS
    FORCEINLINE constexpr bool IsValidState() const;
#endif /* LAL_DO_CHECKS */

    LObjectContext* Context;
    T* Pointer;
};

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage() noexcept
    : Context(nullptr), Pointer(nullptr)
{
    check( this->IsValidState() )
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(LNullptrTy) noexcept
    : Context(nullptr), Pointer(nullptr)
{
    check( this->IsValidState() )
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(T* InPointer) noexcept
{
    if (InPointer)
    {
        this->Context = InPointer->GetOuter();
        this->Pointer = InPointer;
    }
    else
    {
        this->Context = nullptr;
        this->Pointer = nullptr;
    }

    check( this->IsValidState() )

    return;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(LObjectContext* InContextPointer, JObjectBase* InPointer) noexcept
    : Context(InContextPointer), Pointer(InPointer)
{
    check( this->IsValidState() )
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(const TObjectStorage& InOther) noexcept
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;

    check( this->IsValidState() )

    return;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(TObjectStorage&& InOther) noexcept
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;
    InOther.Context = nullptr;
    InOther.Pointer = nullptr;

    check( this->IsValidState() )

    return;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(LNullptrTy) noexcept
{
    this->Context = nullptr;
    this->Pointer = nullptr;

    check( this->IsValidState() )

    return *this;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(T* InPointer) noexcept
{
    if (InPointer)
    {
        this->Context = InPointer->GetOuter();
        this->Pointer = InPointer;
    }
    else
    {
        this->Context = nullptr;
        this->Pointer = nullptr;
    }

    check( this->IsValidState() )

    return *this;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(const TObjectStorage& InOther) noexcept
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;

    check( this->IsValidState() )

    return *this;
}

template<typename T>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(TObjectStorage&& InOther) noexcept
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;
    InOther.Context = nullptr;
    InOther.Pointer = nullptr;

    check( this->IsValidState() )

    return *this;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(TOther* InPointer) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    if (InPointer)
    {
        this->Context = InPointer->GetOuter();
        this->Pointer = InPointer;
    }
    else
    {
        this->Context = nullptr;
        this->Pointer = nullptr;
    }

    check( this->IsValidState() )

    return;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(const TObjectStorage<TOther>& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;

    check( this->IsValidState() )

    return;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>::TObjectStorage(TObjectStorage<TOther>&& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;
    InOther.Context = nullptr;
    InOther.Pointer = nullptr;

    check( this->IsValidState() )

    return;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(TOther* InPointer) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    if (InPointer)
    {
        this->Context = InPointer->GetOuter();
        this->Pointer = InPointer;
    }
    else
    {
        this->Context = nullptr;
        this->Pointer = nullptr;
    }

    check( this->IsValidState() )

    return *this;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(const TObjectStorage<TOther>& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;

    check( this->IsValidState() )

    return *this;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr TObjectStorage<T>& TObjectStorage<T>::operator=(TObjectStorage<TOther>&& InOther) noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    this->Context = InOther.Context;
    this->Pointer = InOther.Pointer;
    InOther.Context = nullptr;
    InOther.Pointer = nullptr;

    check( this->IsValidState() )

    return *this;
}

template<typename T>
FORCEINLINE constexpr void TObjectStorage<T>::Reset() noexcept
{
    this->Context = nullptr;
    this->Pointer = nullptr;

    check( this->IsValidState() )

    return;
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator==(LNullptrTy) const noexcept
{
    return this->Pointer == nullptr;
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator!=(LNullptrTy) const noexcept
{
    return !(*this == nullptr);
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator==(T* InOther) const noexcept
{
    return this->Pointer == InOther;
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator!=(T* InOther) const noexcept
{
    return !(*this == InOther);
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr bool TObjectStorage<T>::operator==(TOther* InOther) const noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    return this->Pointer == InOther;
}

template<typename T>
template<typename TOther>
FORCEINLINE constexpr bool TObjectStorage<T>::operator!=(TOther* InOther) const noexcept requires ((std::is_same_v<T, TOther> == false) && std::is_base_of_v<T, TOther>)
{
    return !(*this == InOther);
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator==(const TObjectStorage& InOther) const noexcept
{
    return this->Context == InOther.Context && this->Pointer == InOther.Pointer;
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::operator!=(const TObjectStorage& InOther) const noexcept
{
    return !(*this == InOther);
}

template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::IsNull() const noexcept
{
    return this->Pointer == nullptr;
}

template<typename T>
FORCEINLINE bool TObjectStorage<T>::IsValid() const noexcept
{
    return IsValidFast(this->Context, this->Pointer);
}

template<typename T>
FORCEINLINE bool TObjectStorage<T>::IsValidDeep() const noexcept
{
    return IsValidSlow(this->Context, this->Pointer);
}

template<typename T>
FORCEINLINE T* TObjectStorage<T>::operator->() noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    return this->Pointer;
}

template<typename T>
FORCEINLINE const T* TObjectStorage<T>::operator->() const noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    return this->Pointer;
}

template<typename T>
FORCEINLINE T* TObjectStorage<T>::Get() noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    return this->Pointer;
}

template<typename T>
FORCEINLINE const T* TObjectStorage<T>::Get() const noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    return this->Pointer;
}

template<typename T>
FORCEINLINE T& TObjectStorage<T>::operator*() noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    if (this->Pointer == nullptr)
    {
        panic( "Object is invalid." )
    }

    return *this->Pointer;
}

template<typename T>
FORCEINLINE const T& TObjectStorage<T>::operator*() const noexcept
{
    checkCode
    (
        if (this->Pointer)
        {
            check( IsValidFast(this->Context, this->Pointer) )
        }
    )

    if (this->Pointer == nullptr)
    {
        panic( "Object is invalid." )
    }

    return *this->Pointer;
}

#if LAL_DO_CHECKS
template<typename T>
FORCEINLINE constexpr bool TObjectStorage<T>::IsValidState() const
{
    if (this->Pointer)
    {
        check( this->Context )
    }
    else
    {
        check( this->Context == nullptr )
    }

    return true;
}
#endif /* LAL_DO_CHECKS */

} /* ~Namespace Jafg */
