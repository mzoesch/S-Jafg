// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * Jafg interpretation of an optional value.
 * #IsValid() returns whether the value of #GetValue() is meaningful or not.
 */
template <typename T>
struct TOptional final
{
public:

    FORCEINLINE TOptional() : bValid(false) { }
    FORCEINLINE TOptional(EDefaultInit) : bValid(false) { }

    FORCEINLINE TOptional(const T& InValue) noexcept;
    FORCEINLINE TOptional(T&& InValue) noexcept;
    FORCEINLINE TOptional(const TOptional& InOther) noexcept;
    FORCEINLINE TOptional(TOptional&& InOther) noexcept;

    FORCEINLINE ~TOptional();

    FORCEINLINE TOptional& operator=(const T& InValue) noexcept;
    FORCEINLINE TOptional& operator=(T&& InValue) noexcept;
    FORCEINLINE TOptional& operator=(const TOptional& InOther) noexcept;
    FORCEINLINE TOptional& operator=(TOptional&& InOther) noexcept;

    template <typename... InTArgs>
    FORCEINLINE void Emplace(InTArgs&&... InArgs) noexcept;

    FORCEINLINE bool IsValid() const noexcept;
    FORCEINLINE explicit operator bool() const noexcept;

    FORCEINLINE void Reset() noexcept;

    FORCEINLINE       T& GetValue() noexcept;
    FORCEINLINE       T& GetValueAsserted() noexcept;
    FORCEINLINE const T& GetValue() const noexcept;
    FORCEINLINE const T& GetValueAsserted() const noexcept;
    FORCEINLINE       T& GetValue(T& InDefaultValue) noexcept;
    FORCEINLINE const T& GetValue(const T& InDefaultValue) const noexcept;

    FORCEINLINE       T& operator*() noexcept { return this->GetValue(); }
    FORCEINLINE const T& operator*() const noexcept { return this->GetValue(); }
    FORCEINLINE       T* operator->() noexcept { return this->GetStoragePtr(); }
    FORCEINLINE const T* operator->() const noexcept { return this->GetStoragePtr(); }

    FORCEINLINE bool operator==(const T& InValue) const noexcept;
    FORCEINLINE bool operator!=(const T& InValue) const noexcept { return !(*this == InValue); }
    FORCEINLINE bool operator==(const TOptional& InOther) const noexcept;
    FORCEINLINE bool operator!=(const TOptional& InOther) const noexcept { return !(*this == InOther); }

private:

#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif /* LAL_WITH_CLANG */
    typedef std::aligned_storage_t<sizeof(T), alignof(T)> LStorage;
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

          T* GetStoragePtr()       noexcept { return reinterpret_cast<      T*>(&this->Storage); }
    const T* GetStoragePtr() const noexcept { return reinterpret_cast<const T*>(&this->Storage); }

    LStorage Storage;
    bool bValid;
};

template<typename T>
FORCEINLINE TOptional<T>::TOptional(const T& InValue) noexcept : bValid(true)
{
    new(&this->Storage) T(InValue);
}

template<typename T>
FORCEINLINE TOptional<T>::TOptional(T&& InValue) noexcept : bValid(true)
{
    new(&this->Storage) T(std::move(InValue));
}

template<typename T>
FORCEINLINE TOptional<T>::TOptional(const TOptional& InOther) noexcept : bValid(InOther.bValid)
{
    if (this->bValid)
    {
        new (&this->Storage) T(*InOther.GetStoragePtr());
    }
}

template<typename T>
FORCEINLINE TOptional<T>::TOptional(TOptional&& InOther) noexcept : bValid(InOther.bValid)
{
    if (this->bValid)
    {
        new (&this->Storage) T(std::move(*InOther.GetStoragePtr()));
        InOther.Reset();
    }

    return;
}

template<typename T>
FORCEINLINE TOptional<T>::~TOptional()
{
    this->Reset();
    return;
}

template<typename T>
FORCEINLINE TOptional<T>& TOptional<T>::operator=(const T& InValue) noexcept
{
    if (this->IsValid())
    {
        *this->GetStoragePtr() = InValue;
    }
    else
    {
        this->bValid = true;
        new (&this->Storage) T(InValue);
    }

    return *this;
}

template<typename T>
FORCEINLINE TOptional<T>& TOptional<T>::operator=(T&& InValue) noexcept
{
    if (this->IsValid())
    {
        *this->GetStoragePtr() = std::move(InValue);
    }
    else
    {
        this->bValid = true;
        new (&this->Storage) T(std::move(InValue));
    }

    return *this;
}

template<typename T>
FORCEINLINE TOptional<T>& TOptional<T>::operator=(const TOptional& InOther) noexcept
{
    if (LAL_UNLIKELY(this != &InOther))
    {
        if (this->IsValid())
        {
            if (InOther.IsValid())
            {
                *this->GetStoragePtr() = *InOther.GetStoragePtr();
            }
            else
            {
                this->Reset();
            }
        }
        else if (InOther.IsValid())
        {
            this->bValid = true;
            new (&this->Storage) T(*InOther.GetStoragePtr());
        }
    }

    return *this;
}

template<typename T>
FORCEINLINE TOptional<T>& TOptional<T>::operator=(TOptional&& InOther) noexcept
{
    if (LAL_UNLIKELY(this != &InOther))
    {
        if (this->IsValid())
        {
            if (InOther.IsValid())
            {
                *this->GetStoragePtr() = std::move(*InOther.GetStoragePtr());
            }
            else
            {
                this->Reset();
            }
        }
        else if (InOther.IsValid())
        {
            this->bValid = true;
            new (&this->Storage) T(std::move(*InOther.GetStoragePtr()));
        }
    }

    return *this;
}

template<typename T>
template<typename... InTArgs>
FORCEINLINE void TOptional<T>::Emplace(InTArgs&&... InArgs) noexcept
{
    if (this->IsValid())
    {
        this->Reset();
    }

    this->bValid = true;
    new (&this->Storage) T(std::forward<InTArgs>(InArgs)...);

    return;
}

template<typename T>
FORCEINLINE bool TOptional<T>::IsValid() const noexcept
{
    return this->bValid;
}

template<typename T>
FORCEINLINE TOptional<T>::operator bool() const noexcept
{
    return this->IsValid();
}

template<typename T>
FORCEINLINE void TOptional<T>::Reset() noexcept
{
    if (this->IsValid())
    {
        this->GetStoragePtr()->~T();
        this->bValid = false;
    }

    return;
}

template<typename T>
FORCEINLINE T& TOptional<T>::GetValue() noexcept
{
    check( this->IsValid() )
    return *this->GetStoragePtr();
}

template<typename T>
FORCEINLINE T& TOptional<T>::GetValueAsserted() noexcept
{
    jassert( this->IsValid() )
    return *this->GetStoragePtr();
}

template<typename T>
FORCEINLINE const T& TOptional<T>::GetValue() const noexcept
{
    check( this->IsValid() )
    return *this->GetStoragePtr();
}

template<typename T>
FORCEINLINE const T& TOptional<T>::GetValueAsserted() const noexcept
{
    jassert( this->IsValid() )
    return *this->GetStoragePtr();
}

template<typename T>
FORCEINLINE T& TOptional<T>::GetValue(T& InDefaultValue) noexcept
{
    return this->IsValid() ? this->GetValue() : InDefaultValue;
}

template<typename T>
FORCEINLINE const T& TOptional<T>::GetValue(const T& InDefaultValue) const noexcept
{
    return this->IsValid() ? this->GetValue() : InDefaultValue;
}

template<typename T>
FORCEINLINE bool TOptional<T>::operator==(const T& InValue) const noexcept
{
    return this->IsValid() && this->GetValue() == InValue;
}

template<typename T>
FORCEINLINE bool TOptional<T>::operator==(const TOptional& InOther) const noexcept
{
    return this->IsValid() == InOther.IsValid() && (!this->IsValid() || this->GetValue() == InOther.GetValue());
}

} /* ~Namespace Jafg */
