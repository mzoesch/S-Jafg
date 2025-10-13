// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class LWorld;

//#
//# Wrapper around a world raw pointer to store it for a longer period of time. This is essentially a weak pointer.
//# You may check if the world pointer is still valid with #IsValid.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
struct LWorldStorage final
{
    FORCEINLINE explicit constexpr LWorldStorage() noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(LNullptrTy) noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(LWorld* InWorld) noexcept : World(InWorld) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LWorldStorage)
    FORCEINLINE constexpr ~LWorldStorage() noexcept { }

    FORCEINLINE constexpr bool operator==(LNullptrTy) const noexcept { return this->World == nullptr; }
    FORCEINLINE constexpr bool operator==(LWorldStorage const& Other) const noexcept { return this->World == Other.World; }
    FORCEINLINE constexpr bool operator==(LWorld const* Other) const noexcept { return this->World == Other; }

    FORCEINLINE constexpr void Reset() noexcept { this->World = nullptr; }
    FORCEINLINE constexpr bool IsNull() const noexcept { return this->World == nullptr; }
    FORCEINLINE constexpr bool IsNotNull() const noexcept { return this->IsNull() == false; }

    ENGINE_API  bool IsValid() const noexcept;

    FORCEINLINE constexpr LWorld* Get() noexcept { return this->World; }
    FORCEINLINE constexpr LWorld const* Get() const noexcept { return this->World; }

    FORCEINLINE constexpr LWorld* operator->() noexcept { return this->Get(); }
    FORCEINLINE constexpr LWorld const* operator->() const noexcept { return this->Get(); }

    FORCEINLINE constexpr LWorld& operator*() noexcept { return *this->Get(); }
    FORCEINLINE constexpr LWorld const& operator*() const noexcept { return *this->Get(); }

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsNotNull(); }

    FORCEINLINE operator LWorld*() noexcept { return this->Get(); }
    FORCEINLINE operator LWorld const*() const noexcept { return this->Get(); }

private:

    LWorld* World;
};

} /* ~Namespace Jafg */
