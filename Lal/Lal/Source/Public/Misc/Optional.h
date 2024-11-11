// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * Jafg interpretation of an optional value.
 * #IsSet() returns whether the value of #GetValue() is meaningful or not.
 */
template <typename T>
struct TOptional final
{
    TOptional() : bMeaningful(false) { }
    FORCEINLINE TOptional(LNullptrTy) : bMeaningful(false) { }
    FORCEINLINE TOptional(const T& InValue) : Value(InValue), bMeaningful(true) { }
    FORCEINLINE TOptional(T&& InValue) : Value(std::move(InValue)), bMeaningful(true) { }

    template <typename  ... ArgsTy>
    FORCEINLINE T& Emplace(ArgsTy&& ... Args)
    {
        this->Value = T(std::forward<ArgsTy>(Args)...);
        this->bMeaningful = true;
        return this->Value;
    }

    FORCEINLINE auto operator=(LNullptrTy) -> TOptional& { this->bMeaningful = false; return *this; }
    FORCEINLINE auto operator=(const T& InValue) -> TOptional& { this->Value = InValue; this->bMeaningful = true; return *this; }
    FORCEINLINE auto operator=(T&& InValue) -> TOptional& { this->Value = std::move(InValue); this->bMeaningful = true; return *this; }

    FORCEINLINE auto IsSet() const -> bool { return bMeaningful; }
    FORCEINLINE auto MakeMeaningful() -> void { this->bMeaningful = true; }
    FORCEINLINE void SetValue(const T& InValue) { this->Value = InValue; this->bMeaningful = true; }
    FORCEINLINE void Reset() { this->bMeaningful = false; }

    FORCEINLINE auto GetValue() -> T& { check( this->bMeaningful ) return this->Value; }
    FORCEINLINE auto GetValue() const -> const T& { check( this->bMeaningful ) return this->Value; }
    FORCEINLINE auto GetValue(const T& DefaultValue) const -> T& { return this->bMeaningful ? this->Value : DefaultValue; }
    FORCEINLINE auto GetValue(T&& DefaultValue) const -> T& { return this->bMeaningful ? this->Value : std::move(DefaultValue); }
    FORCEINLINE auto GetValuePtr() -> T* { return this->bMeaningful ? &this->Value : nullptr; }

    FORCEINLINE auto operator +() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator +() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator -() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator -() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator *() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator *() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator->() -> T* { return this->GetValuePtr(); }
    FORCEINLINE auto operator->() const -> const T* { return this->GetValuePtr(); }
    FORCEINLINE auto operator !() const -> bool { return !this->bMeaningful; }
    FORCEINLINE      operator bool() const { return this->bMeaningful; }

    FORCEINLINE auto operator!=(const TOptional& Other) const -> bool { return !(*this == Other); }
    FORCEINLINE auto operator==(const TOptional& Other) const -> bool
    {
        return this->bMeaningful == Other.bMeaningful && (!this->bMeaningful || this->Value == Other.Value);
    }
    FORCEINLINE auto operator!=(const T& Other) const -> bool { return !(*this == Other); }
    FORCEINLINE auto operator==(const bool bOther) const -> bool { return this->bMeaningful == bOther; }

private:

    T    Value;
    bool bMeaningful;
};

} /* ~Namespace Jafg. */
