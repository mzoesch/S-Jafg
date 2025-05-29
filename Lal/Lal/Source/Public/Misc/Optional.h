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

    FORCEINLINE TOptional(const TOptional<T>& Other) noexcept : bMeaningful(Other.bMeaningful)
    {
        if (this->bMeaningful)
        {
            this->Value = Other.Value;
        }

        return;
    }

    FORCEINLINE TOptional(TOptional<T>&& Other) noexcept : bMeaningful(Other.bMeaningful)
    {
        if (this->bMeaningful)
        {
            this->Value = std::move(Other.Value);
            Other.bMeaningful = false;
        }

        return;
    }

    FORCEINLINE TOptional& operator=(const TOptional<T>& Other) noexcept
    {
        this->bMeaningful = Other.bMeaningful;
        if (this->bMeaningful)
        {
            this->Value = Other.Value;
        }

        return *this;
    }

    FORCEINLINE TOptional& operator=(TOptional<T>&& Other) noexcept
    {
        this->bMeaningful = Other.bMeaningful;
        if (this->bMeaningful)
        {
            this->Value = std::move(Other.Value);
            Other.bMeaningful = false;
        }

        return *this;
    }

    FORCEINLINE ~TOptional() { this->Reset(); }

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

    FORCEINLINE auto IsSet() const -> bool { return this->bMeaningful; }
    FORCEINLINE auto MakeMeaningful() -> void { this->bMeaningful = true; }
    FORCEINLINE void SetValue(const T& InValue) { this->Value = InValue; this->bMeaningful = true; }

    void Reset();

    FORCEINLINE auto GetValue() -> T& { check( this->bMeaningful ) return this->Value; }
    FORCEINLINE auto GetValue() const -> const T& { check( this->bMeaningful ) return this->Value; }
    FORCEINLINE auto GetValue(const T& DefaultValue) const -> T& { return this->bMeaningful ? this->Value : DefaultValue; }
    FORCEINLINE auto GetValue(T&& DefaultValue) const -> T& { return this->bMeaningful ? this->Value : std::move(DefaultValue); }
    FORCEINLINE auto GetValuePtr() -> T* { return this->bMeaningful ? &this->Value : nullptr; }
    FORCEINLINE auto GetValuePtr() const -> const T* { return this->bMeaningful ? &this->Value : nullptr; }

    FORCEINLINE auto operator *() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator *() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator->() -> T* { return this->GetValuePtr(); }
    FORCEINLINE auto operator->() const -> const T* { return this->GetValuePtr(); }
    FORCEINLINE auto operator !() const -> bool { return !this->bMeaningful; }

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

template<typename T>
void TOptional<T>::Reset()
{
    if (this->bMeaningful)
    {
        this->bMeaningful = false;
        this->Value.~T();
    }

    return;
}

} /* ~Namespace Jafg */
