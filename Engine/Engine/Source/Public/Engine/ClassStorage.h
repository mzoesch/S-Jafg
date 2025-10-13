// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClassUtlity.h"

namespace Jafg
{

//#
//# This struct is meant as a wrapper class for storing object pointers for a longer time.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
template<typename TObj>
struct TClassStorage final
{
    static_assert(std::is_base_of_v<JCxxClass, TObj>);

    FORCEINLINE constexpr TClassStorage() noexcept : Outer(nullptr), Pointer(nullptr) { checkCode( this->CheckValidState() ) }
    FORCEINLINE constexpr TClassStorage(LNullptrTy) noexcept : Outer(nullptr), Pointer(nullptr) { checkCode( this->CheckValidState() ) }
    FORCEINLINE constexpr TClassStorage(TObj* InPointer) noexcept : Outer(nullptr), Pointer(InPointer)
    {
        if (InPointer)
        {
            this->Outer = InPointer->GetOuter();
        }

        checkCode( this->CheckValidState() )

        return;
    }
    FORCEINLINE constexpr TClassStorage(LClassOuter* InOuter, TObj* InPointer) noexcept : Outer(InOuter), Pointer(InPointer) { checkCode( this->CheckValidState() ) }
    FORCEINLINE constexpr TClassStorage(TClassStorage const& InOther) noexcept = default;
    FORCEINLINE constexpr TClassStorage(TClassStorage&& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;
        InOther.Outer = nullptr;
        InOther.Pointer = nullptr;

        checkCode( this->CheckValidState(); InOther.CheckValidState() )

        return;
    }

    FORCEINLINE constexpr TClassStorage& operator=(LNullptrTy) noexcept { this->Outer = nullptr; this->Pointer = nullptr; checkCode( this->CheckValidState() ) return *this; }
    FORCEINLINE constexpr TClassStorage& operator=(TObj* InPointer) noexcept
    {
        if (InPointer)
        {
            this->Outer = InPointer->GetOuter();
            this->Pointer = InPointer;
        }
        else
        {
            this->Outer = nullptr;
            this->Pointer = nullptr;
        }

        checkCode( this->CheckValidState() )

        return *this;
    }
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage const& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;

        checkCode( this->CheckValidState() )

        return *this;
    }
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage&& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;
        InOther.Outer = nullptr;
        InOther.Pointer = nullptr;

        checkCode( this->CheckValidState(); InOther.CheckValidState() )

        return *this;
    }

    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage(TClassStorage<UObj> const& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;

        checkCode( this->CheckValidState() )

        return;
    }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage(TClassStorage<UObj>&& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;
        InOther.Outer = nullptr;
        InOther.Pointer = nullptr;

        checkCode( this->CheckValidState() && InOther.CheckValidState() )

        return;
    }

    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage<UObj> const& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;

        checkCode( this->CheckValidState() )

        return *this;
    }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage<UObj>&& InOther)
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;
        InOther.Outer = nullptr;
        InOther.Pointer = nullptr;

        checkCode( this->CheckValidState() && InOther.CheckValidState() )

        return *this;
    }

    //# Nulls the underlying object out.
    FORCEINLINE constexpr void Reset() noexcept { this->Outer = nullptr; this->Pointer = nullptr; checkCode( this->CheckValidState() ) return; }

    FORCEINLINE constexpr bool operator==(LNullptrTy) const noexcept { return this->Pointer == nullptr; }
    FORCEINLINE constexpr bool operator==(TObj* InOther) const noexcept { return this->Pointer == InOther; }
    FORCEINLINE constexpr bool operator==(LClassOuter* InOuter) const noexcept { return this->Outer == InOuter; }

    FORCEINLINE constexpr bool operator==(TClassStorage const& InOther) const noexcept { return this->Outer == InOther.Outer && this->Pointer == InOther.Pointer; }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr bool operator==(TClassStorage<UObj> const& InOther) const noexcept { return this->Outer == InOther.Outer && this->Pointer == InOther.Pointer; }

    //#
    //# Checks whether the underlying object pointer is null.
    //# Equivalent to if (Pointer) { ... }
    //#
    FORCEINLINE constexpr bool IsNull() const noexcept { return this->Pointer == nullptr; }
    FORCEINLINE constexpr bool IsNotNull() const noexcept { return this->IsNull() == false; }

    //#
    //# A very fast check if the #Pointer is still valid.
    //# Requires that the context is still valid.
    //#
    //# @see #IsValidFast function in Engine/CxxClassUtility.h
    //#
    FORCEINLINE bool IsValid() const noexcept { return IsValidFast(this->Outer, this->Pointer); }

    //#
    //# A check if the #Pointer is still valid. This check will also check if the context has gotten out of scope.
    //# Use this only in cases where the caller is outside the context, where the pointed object is home.
    //#
    //# @see #IsValidSlow function in Engine/CxxClassUtility.h
    //#
    FORCEINLINE bool IsValidDeep() const noexcept { return IsValidSlow(this->Outer, this->Pointer); }

    FORCEINLINE TObj* Get() noexcept { check( this->IsValid() ) return this->Pointer; }
    FORCEINLINE TObj const* Get() const noexcept { check( this->IsValid() ) return this->Pointer; }

    FORCEINLINE TObj* operator->() noexcept { return this->Get(); }
    FORCEINLINE TObj const* operator->() const noexcept { return this->Get(); }

    FORCEINLINE TObj& operator*() noexcept { return *this->Get(); }
    FORCEINLINE TObj const& operator*() const noexcept { return *this->Get(); }

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsNotNull(); }

    FORCEINLINE operator TObj*() noexcept { return this->Get(); }
    FORCEINLINE operator TObj const*() const noexcept { return this->Get(); }

private:

#if LAL_DO_CHECKS
    FORCEINLINE constexpr void CheckValidState() const
    {
        if (this->Pointer)
        {
            jassert( this->Outer )
        }
        else
        {
            jassert( this->Outer == nullptr )
        }

        return;
    }
#endif /* LAL_DO_CHECKS */

    LClassOuter* Outer;
    TObj* Pointer;
};

} /* ~Namespace Jafg */
