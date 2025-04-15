// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace Private
{

LAL_API bool TPathBase_DoesExist(const char* InPath);
LAL_API bool TPathBase_IsFile(const char* InPath);
LAL_API bool TPathBase_IsDir(const char* InPath);

} /* ~Namespace Private */

//#
//# The base class for all paths.
//#
template <typename InTraits, typename InAlloc>
class TPathBase;

template <typename InT>
using TPathBasic = TPathBase<TStringTraits<InT>, TArray<InT>>;

typedef TPathBasic<char> LPath;

template <typename InTraits, typename InAlloc>
class TPathBase : public TStringBase<TPathBase<InTraits, InAlloc>, InTraits, InAlloc>
{
public:

    using Super = TStringBase<TPathBase<InTraits, InAlloc>, InTraits, InAlloc>;

    using Traits   = typename Super::Traits;
    using T        = typename Super::T;
    using Alloc    = typename Super::Alloc;
    using Self     = typename Super::Self;
    using SizeType = typename Super::SizeType;

    FORCEINLINE TPathBase() noexcept = default;
    FORCEINLINE TPathBase(LNullptrTy) noexcept : Super(nullptr) { }
    FORCEINLINE TPathBase(const Self& InOther) noexcept : Super(InOther) { }
    FORCEINLINE TPathBase(Self&& InOther) noexcept : Super(std::move(InOther)) { }
    explicit    TPathBase(const T InRune) : Super(InRune) { }
    FORCEINLINE TPathBase(const T* InString) : Super(InString) { }
    FORCEINLINE TPathBase(const T* InString, const SizeType InLength) : Super(InString, InLength) { }
    FORCEINLINE ~TPathBase() noexcept = default;

    FORCEINLINE Self& operator =(LNullptrTy) noexcept { return this->Super::operator =(nullptr); }
    FORCEINLINE Self& operator =(const Self& InOther) noexcept { return this->Super::operator=(InOther); }
    FORCEINLINE Self& operator =(Self&& InOther) noexcept { return this->Super::operator=(std::move(InOther)); }
    FORCEINLINE Self& operator =(const T InRune) noexcept { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator =(const T* InString) noexcept { return this->Super::operator=(InString); }

    //# The amount of directory separators in the path that were normalized.
    FORCEINLINE SizeType Normalize() noexcept;

    //# Try to pop the last sub-path.
    FORCEINLINE bool PopSubPath() noexcept;
    FORCEINLINE bool PopSubPath(const bool bNormalize) noexcept;
    //# Try to pop the last sub-paths and return the number of the actually popped sub-paths.
    FORCEINLINE SizeType PopSubPaths(const SizeType NumberOfSubPaths) noexcept;
    FORCEINLINE SizeType PopSubPaths(const SizeType NumberOfSubPaths, const bool bNormalize) noexcept;

    FORCEINLINE void AddExtension(const Self& InExtension) noexcept;

    FORCEINLINE bool DoesExist() const noexcept { return Private::TPathBase_DoesExist(static_cast<const char*>(this->ToPtr())); }
    FORCEINLINE bool IsFile() const noexcept { return Private::TPathBase_IsFile(static_cast<const char*>(this->ToPtr())); }
    FORCEINLINE bool IsDir() const noexcept { return Private::TPathBase_IsDir(static_cast<const char*>(this->ToPtr())); }

    FORCEINLINE Self GetBase() const noexcept;
};

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TPathBase<InTraits, InAlloc>::SizeType TPathBase<InTraits, InAlloc>::Normalize() noexcept
{
    return this->Replace('\\', '/');
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TPathBase<InTraits, InAlloc>::PopSubPath() noexcept
{
    if (const SizeType Last = this->FindLast('/'); Last != INDEX_NONE)
    {
        this->InlineLeftChop(Last);
        return true;
    }

    if (this->IsEmpty())
    {
        return false;
    }

    this->Empty();
    return true;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TPathBase<InTraits, InAlloc>::PopSubPath(const bool bNormalize) noexcept
{
    if (bNormalize)
    {
        this->Normalize();
    }

    return this->PopSubPath();
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TPathBase<InTraits, InAlloc>::SizeType TPathBase<InTraits, InAlloc>::PopSubPaths(const SizeType NumberOfSubPaths) noexcept
{
    SizeType Out = 0;
    while (Out < NumberOfSubPaths)
    {
        if (this->PopSubPath() == false)
        {
            return Out;
        }

        ++Out;
        continue;
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TPathBase<InTraits, InAlloc>::SizeType TPathBase<InTraits, InAlloc>::PopSubPaths(const SizeType NumberOfSubPaths, const bool bNormalize) noexcept
{
    if (bNormalize)
    {
        this->Normalize();
    }

    return this->PopSubPaths(NumberOfSubPaths);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TPathBase<InTraits, InAlloc>::AddExtension(const Self& InExtension) noexcept
{
    this->Append(InExtension);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TPathBase<InTraits, InAlloc>::Self TPathBase<InTraits, InAlloc>::GetBase() const noexcept
{
    if (SizeType Last = this->FindLast('/'); Last != INDEX_NONE)
    {
        return this->RightChop(Last + 1);
    }

    return this->GetSelf();
}

} /* ~Namespace Jafg */

template <>
struct std::formatter<::Jafg::LPath> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::LPath& InPath,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<const char*>::format(InPath.ToPtr(), InContext);
    }
};
