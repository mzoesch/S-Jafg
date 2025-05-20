// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/World.h"

namespace Jafg
{

//#
//# Wrapper around a world raw pointer to store it for a longer period of time. This is essentially a weak pointer.
//# You may check if the world pointer is still valid with #IsValid.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
struct LWorldStorage
{
    FORCEINLINE explicit constexpr LWorldStorage() noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(LNullptrTy) noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(LWorld* InWorld) noexcept : World(InWorld) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LWorldStorage)
    FORCEINLINE constexpr ~LWorldStorage() noexcept { }

    FORCEINLINE constexpr bool operator==(LNullptrTy) const noexcept { return this->World == nullptr; }
    FORCEINLINE constexpr bool operator!=(LNullptrTy) const noexcept { return this->World != nullptr; }
    FORCEINLINE constexpr bool operator==(const LWorldStorage& Other) const noexcept { return this->World == Other.World; }
    FORCEINLINE constexpr bool operator!=(const LWorldStorage& Other) const noexcept { return this->World != Other.World; }
    FORCEINLINE constexpr bool operator==(const LWorld* Other) const noexcept { return this->World == Other; }
    FORCEINLINE constexpr bool operator!=(const LWorld* Other) const noexcept { return this->World != Other; }

    //#
    //# Nulls the underlying object out.
    //#
    FORCEINLINE constexpr void Reset() noexcept { this->World = nullptr; }

    //#
    //# Checks if the pointer is null.
    //#
    FORCEINLINE constexpr bool IsNull() const noexcept { return this->World == nullptr; }

    //#
    //# Checks if the pointer is valid.
    //#
    ENGINE_API  bool IsValid() const noexcept;

    FORCEINLINE constexpr       LWorld* operator->() noexcept { return  this->World; }
    FORCEINLINE constexpr const LWorld* operator->() const noexcept { return this->World; }
    FORCEINLINE constexpr       LWorld* Get() noexcept { return this->World; }
    FORCEINLINE constexpr const LWorld* Get() const noexcept { return this->World; }
    FORCEINLINE constexpr       LWorld& operator*() noexcept { return *this->World; }
    FORCEINLINE constexpr const LWorld& operator*() const noexcept { return *this->World; }

    FORCEINLINE constexpr operator bool() const noexcept { return this->World != nullptr; }

    FORCEINLINE operator       LWorld*() noexcept { return this->World; }
    FORCEINLINE operator const LWorld*() const noexcept { return this->World; }

private:

    LWorld* World;
};

} /* ~Namespace Jafg */
