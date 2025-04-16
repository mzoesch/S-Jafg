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

    template <typename TOtherAlloc>
    using TOtherPathBase = TPathBase<InTraits, TOtherAlloc>;

    friend void OnDefaultOnlyMallocMember(Self* MemberField);
    friend void OnDefaultOnlyMallocMember(TArray<Self>* MemberField);

    template <typename TOtherTraits, typename TOtherAlloc>
    friend class TPathBase;

    template <typename TOtherString>
    FORCEINLINE consteval static bool IsValidOtherString() noexcept { return Super::template IsValidOtherString<TOtherString>(); }

    FORCEINLINE consteval static bool IsDynamic()        noexcept { return Super::IsDynamic();        }
    FORCEINLINE consteval static bool IsWeakAlloc()      noexcept { return Super::IsWeakAlloc();      }
    FORCEINLINE consteval static bool IsStrongAlloc()    noexcept { return Super::IsStrongAlloc();    }
    FORCEINLINE consteval static bool IsContentConst()   noexcept { return Super::IsContentConst();   }
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return Super::IsContentMutable(); }

    FORCEINLINE  TPathBase() noexcept = default;
    FORCEINLINE  TPathBase(LNullptrTy) noexcept : Super(nullptr) { }
    FORCEINLINE  TPathBase(const Self& InOther) noexcept : Super(InOther) { }
    FORCEINLINE  TPathBase(Self&& InOther) noexcept : Super(std::move(InOther)) { }
    explicit     TPathBase(const T InRune) noexcept requires (Self::IsStrongAlloc()) : Super(InRune) { }
    explicit     TPathBase(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InRune) { }
    explicit     TPathBase(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InRune) { }
    FORCEINLINE  TPathBase(const T* InString) noexcept requires (Self::IsStrongAlloc()) : Super(InString) { }
    FORCEINLINE  TPathBase(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString) { }
    FORCEINLINE  TPathBase(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString) { }
    FORCEINLINE  TPathBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsStrongAlloc()) : Super(InString, InLength) { }
    FORCEINLINE  TPathBase(const T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString, InLength) { }
    FORCEINLINE  TPathBase(      T* InString, const SizeType InLength) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString, InLength) { }
    FORCEINLINE  TPathBase(const T* InString, const T* InEnd) noexcept requires (Self::IsStrongAlloc()) : Super(InString, InEnd) { }
    FORCEINLINE  TPathBase(const T* InString, const T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) : Super(InString, InEnd) { }
    FORCEINLINE  TPathBase(      T* InString,       T* InEnd) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) : Super(InString, InEnd) { }
    FORCEINLINE ~TPathBase() noexcept = default;
    template <typename TOtherString>
    FORCEINLINE  TPathBase(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(Other) { }
    template <typename TOtherString>
    FORCEINLINE  TPathBase(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  TPathBase(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE  TPathBase(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(std::move(InString)) { }
    template <typename TOtherString>
    FORCEINLINE  TPathBase(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) : Super(std::move(InString)) { }

    FORCEINLINE Self& operator=(LNullptrTy) noexcept { return this->Super::operator=(nullptr); }
    FORCEINLINE Self& operator=(const Self& InOther) noexcept { return this->Super::operator=(InOther); }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { return this->Super::operator=(std::move(InOther)); }
    FORCEINLINE Self& operator=(const T InRune) noexcept requires (Self::IsStrongAlloc()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(const T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(      T& InRune) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Super::operator=(InRune); }
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsStrongAlloc()) { return this->Super::operator=(InString); }
    FORCEINLINE Self& operator=(const T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst()) { return this->Super::operator=(InString); }
    FORCEINLINE Self& operator=(      T* InString) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable()) { return this->Super::operator=(InString); }
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) { return this->Super::operator=(Other); }
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(const TOtherString&& Other) noexcept requires (Self::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) = delete;
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsWeakAlloc() && TOtherString::IsWeakAlloc() && Self::template IsValidOtherString<TOtherString>()) { return this->Super::operator=(std::move(InString)); }
    template <typename TOtherString>
    FORCEINLINE Self& operator=(TOtherString&& InString) noexcept requires (Self::IsStrongAlloc() && TOtherString::IsStrongAlloc() && Self::template IsValidOtherString<TOtherString>()) { return this->Super::operator=(std::move(InString)); }

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
