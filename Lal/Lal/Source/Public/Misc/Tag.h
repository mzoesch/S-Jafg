// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Lal
{

template <std::unsigned_integral T, LSize Alignment = alignof(T)>
struct TTag
{
    enum : T { NO_TAG = 0 };

    FORCEINLINE constexpr TTag() noexcept : Value(NO_TAG) { }
    FORCEINLINE constexpr TTag(const TTag& Other) noexcept = default;
    FORCEINLINE constexpr TTag(TTag&& Other) noexcept : Value(Other.Value) { Other.Value = NO_TAG; }
    FORCEINLINE constexpr TTag& operator=(const TTag& Other) noexcept = default;
    FORCEINLINE constexpr TTag& operator=(TTag&& Other) noexcept { this->Value = Other.Value; Other.Value = NO_TAG; return *this; }
    FORCEINLINE constexpr ~TTag() noexcept = default;

    FORCEINLINE constexpr static bool IsEqual(const TTag& A, const TTag& B) noexcept { return A.Value == B.Value; }
    FORCEINLINE constexpr bool Equals(const TTag& Other) const noexcept { return TTag::IsEqual(*this, Other); }
    FORCEINLINE constexpr bool operator==(const TTag& Other) const noexcept { return TTag::IsEqual(*this, Other); }
    FORCEINLINE constexpr bool operator!=(const TTag& Other) const noexcept = default;

    FORCEINLINE constexpr bool IsSet() const noexcept { return this->Value != NO_TAG; }

    ///////////////////////////////////////////////////////////////////////////////
    // Hashing only. Do not use.
    // This is meaningless as names are not deterministic.
    FORCEINLINE std::strong_ordering operator<=>(const TTag& Other) const { return this->Value <=> Other.Value; }
    // ~Hashing only. Do not use.
    ///////////////////////////////////////////////////////////////////////////////

private:

    FORCEINLINE explicit constexpr TTag(const T InValue) noexcept : Value(InValue)
    {
        return;
    }

    alignas(Alignment)
    T Value;
};

template <std::unsigned_integral T, LSize Alignment = alignof(T), typename TAllocator = TArray<TTag<T, Alignment>>>
struct TTagRegistry
{
    using TagType = TTag<T, Alignment>;
    using Allocator = TAllocator;

    FORCEINLINE constexpr TTagRegistry() noexcept = default;
    template <typename... TArgs> requires(std::is_constructible_v<Allocator, TArgs...>)
    FORCEINLINE constexpr TTagRegistry(TArgs&&... Args) noexcept(std::is_nothrow_constructible_v<Allocator, TArgs...>)
        : Tags(std::forward<TArgs>(Args)...)
    {
        return;
    }

    FORCEINLINE constexpr bool IsTagRegistered(const TagType InTag) const noexcept
    {
        return InTag.IsSet() && InTag.Value <= this->Tags.GetSize();
    }

    //# FORCEINLINE constexpr bool IsTagRegistered(CString auto&& InRepr) const noexcept
    //# {
    //#     return this->Tags.Contains(InRepr);
    //# }

    FORCEINLINE u64 GetTagCount() const noexcept { return this->Tags.GetSize(); }

    FORCEINLINE u64 Destroy() const
    {
        const u64 Count { this->Tags.GetSize() };

        LOG_VERBOSE(LogTags, "Destroying [{}] tags from the registry.", Count)
        this->Tags.Empty();

        return Count;
    }

private:

    Allocator Tags;
};

} /* ~Namespace Lal */
