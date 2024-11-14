// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * A simple string that represents a path.
 * Works very will with jafg finder as it can easily be used to search in relative directories in the jafg folder
 * hierarchy.
 */
template <typename InTStringTy>
class LPathBase final
{
public:

    using T         = InTStringTy;
    using TStringTy = InTStringTy;
    using LStringTy = TStringTy;
    using SizeType  = typename LStringTy::SizeType;
    using LRune     = typename LStringTy::LRune;

    inline static LRune StringTerminatorRune = LStringTy::StringTerminatorRune;
    inline static LRune PathSeparator        = '/';

    FORCEINLINE  LPathBase() noexcept = default;
    FORCEINLINE  LPathBase(LNullptrTy) noexcept { }
    FORCEINLINE  LPathBase(const LPathBase<T>& Other) noexcept { this->Data = Other.Data; }
    FORCEINLINE  LPathBase(LPathBase<T>&& Other) noexcept { this->Data = std::forward<LStringTy>(Other.Data); }
    FORCEINLINE  LPathBase(const LStringTy& Other) noexcept { this->Data = Other; }
    FORCEINLINE  LPathBase(LStringTy&& Other) noexcept { this->Data = std::move(Other); }
    FORCEINLINE ~LPathBase() noexcept = default;

    FORCEINLINE auto GetSize() const noexcept -> SizeType { return this->Data.GetSize(); }
    FORCEINLINE auto IsEmpty() const noexcept -> bool     { return this->Data.IsEmpty(); }
    FORCEINLINE auto Reset(const SizeType InReserve) noexcept -> void { this->Data.Reset(InReserve); }
    FORCEINLINE auto Empty() noexcept -> void { this->Data.Empty(); }

    FORCEINLINE auto Reserve(const SizeType Size) noexcept -> void { this->Data.Reserve(Size); }

    FORCEINLINE auto operator=(const LPathBase<T>& Other) noexcept -> LPathBase<T>& = default;
    FORCEINLINE auto operator=(LPathBase<T>&& Other) noexcept -> LPathBase<T>& { this->Data = std::forward<LStringTy>(Other.Data); return *this; }
    FORCEINLINE auto operator=(const LStringTy& Other) noexcept -> LPathBase<T>& { this->Data = Other; return *this; }
    FORCEINLINE auto operator=(LStringTy&& Other) noexcept -> LPathBase<T>& { this->Data = std::move(Other); return *this; }

    FORCEINLINE auto operator==(const LPathBase<T>& Other) const noexcept -> bool { return this->Data == Other.Data; }
    FORCEINLINE auto operator!=(const LPathBase<T>& Other) const noexcept -> bool { return this->Data != Other.Data; }
    FORCEINLINE auto operator <(const LPathBase<T>& Other) const noexcept -> bool { return this->Data  < Other.Data; }
    FORCEINLINE auto operator >(const LPathBase<T>& Other) const noexcept -> bool { return this->Data  > Other.Data; }
    FORCEINLINE auto operator<=(const LPathBase<T>& Other) const noexcept -> bool { return this->Data <= Other.Data; }
    FORCEINLINE auto operator>=(const LPathBase<T>& Other) const noexcept -> bool { return this->Data >= Other.Data; }
    FORCEINLINE auto Equals(const LPathBase<T>& Other)     const noexcept -> bool { return this->Data == Other.Data; }

    FORCEINLINE auto operator==(const LStringTy& Other) const noexcept -> bool { return this->Data == Other; }
    FORCEINLINE auto operator!=(const LStringTy& Other) const noexcept -> bool { return this->Data != Other; }
    FORCEINLINE auto operator <(const LStringTy& Other) const noexcept -> bool { return this->Data  < Other; }
    FORCEINLINE auto operator >(const LStringTy& Other) const noexcept -> bool { return this->Data  > Other; }
    FORCEINLINE auto operator<=(const LStringTy& Other) const noexcept -> bool { return this->Data <= Other; }
    FORCEINLINE auto operator>=(const LStringTy& Other) const noexcept -> bool { return this->Data >= Other; }
    FORCEINLINE auto Equals(const LStringTy& Other)     const noexcept -> bool { return this->Data == Other; }

    FORCEINLINE auto GetPath() const noexcept -> const LStringTy& { return this->Data; }
    FORCEINLINE auto MoveOut() noexcept -> LStringTy { return std::move(this->Data); }

    /** Private iterator functions for range-based loops. Do not use these directly. */
    FORCEINLINE auto begin()       noexcept -> Iterator<typename LStringTy::LRune>       { return this->Data.begin(); }
    FORCEINLINE auto begin() const noexcept -> Iterator<const typename LStringTy::LRune> { return this->Data.begin(); }
    FORCEINLINE auto end()         noexcept -> Iterator<typename LStringTy::LRune>       { return this->Data.end();   }
    FORCEINLINE auto end()   const noexcept -> Iterator<const typename LStringTy::LRune> { return this->Data.end();   }

    template <typename ... ArgyTy>
    static auto SprintF(const char* Format, const ArgyTy& ... Args) -> LPathBase<T> { return LSimpleString::SprintF(Format, Args ...); }

    FORCEINLINE auto operator /(const LRune* Other) const noexcept -> LPathBase<T>;
    FORCEINLINE auto operator/=(const LRune* Other) noexcept -> LPathBase<T>&;
    FORCEINLINE auto operator /(const LPathBase<T>& Other) const noexcept -> LPathBase<T>;
    FORCEINLINE auto operator/=(const LPathBase<T>& Other) noexcept -> LPathBase<T>&;

    FORCEINLINE void Normalize() noexcept;
    /** @return The number of sub paths that were actually popped. */
    FORCEINLINE int32 PopSubPaths(const int32 NumberOfSubPaths);

    FORCEINLINE void AddExtension(const LStringTy& Extension);

private:

    LStringTy Data = nullptr;
};

template <typename InTStringTy>
LPathBase<InTStringTy> LPathBase<InTStringTy>::operator/(const LRune* Other) const noexcept
{
    LPathBase<T> Out;
    Out.Reserve(this->Data.GetSize());
    Out.Data.CopyFrom(this->Data);
    return Out /= Other;
}

template <typename InTStringTy>
LPathBase<InTStringTy>& LPathBase<InTStringTy>::operator/=(const LRune* Other) noexcept
{
    if (*Other == LPathBase<T>::StringTerminatorRune)
    {
        return *this;
    }

    if (*this->Data.Peek() != LPathBase<T>::PathSeparator)
    {
        this->Data.Add(&LPathBase<T>::PathSeparator);
    }

    if (*Other == LPathBase<T>::PathSeparator)
    {
        ++Other;
    }

    if (*Other == LPathBase<T>::StringTerminatorRune)
    {
        return *this;
    }

    this->Data += Other;

    return *this;
}

template <typename InTStringTy>
LPathBase<InTStringTy> LPathBase<InTStringTy>::operator/(const LPathBase<T>& Other) const noexcept
{
    return *this / Other.GetPath().ToPtr();
}

template <typename InTStringTy>
LPathBase<InTStringTy>& LPathBase<InTStringTy>::operator/=(const LPathBase<T>& Other) noexcept
{
    return *this /= Other.GetPath().ToPtr();
}

template <typename InTStringTy>
void LPathBase<InTStringTy>::Normalize() noexcept
{
    this->Data.Replace('\\', '/');

    if (*this->Data.Peek() == LPathBase<T>::PathSeparator)
    {
        this->Data.Pop();
    }

    return;
}

template <typename InTStringTy>
int32 LPathBase<InTStringTy>::PopSubPaths(const int32 NumberOfSubPaths)
{
    int32 Popped = 0;

    this->Normalize();

    for (int32 i = 0; i < NumberOfSubPaths; ++i)
    {
        if (this->Data.IsEmpty())
        {
            break;
        }

        const int32 Last = this->Data.FindLast(LPathBase<T>::PathSeparator);
        if (Last == INDEX_NONE)
        {
            this->Data.Empty();
            break;
        }

        this->Data.InlineCut(Last - 1);
        ++Popped;
    }

    return Popped;
}

template <typename InTStringTy>
void LPathBase<InTStringTy>::AddExtension(const LStringTy& Extension)
{
    this->Data.Append(Extension);
}

using LPath = LPathBase<LSimpleString>;

} /* ~Namespace Jafg */
