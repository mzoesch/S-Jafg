// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Lal
{

template<std::unsigned_integral T, LSize Alignment = alignof(T)>
struct TTag
{
    template <typename TTag, typename TAllocator>
    friend struct TTagRegistry;

    using SizeType = T;

    enum : SizeType { NO_TAG = 0 };

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

    FORCEINLINE constexpr T GetUnderlyingValue() const noexcept { return this->Value; }

private:

    FORCEINLINE explicit constexpr TTag(const T InValue) noexcept : Value(InValue)
    {
        return;
    }

    alignas(Alignment)
    T Value;
};

template<typename TTag, typename TAllocator = TArray<LString>>
struct TTagRegistry
{
    using TagType   = TTag;
    using Allocator = TAllocator;
    using ReprType  = typename Allocator::value_type;

    FORCEINLINE constexpr TTagRegistry() noexcept requires(std::is_default_constructible_v<Allocator>) = default;
    template <typename... TArgs> requires(std::is_constructible_v<Allocator, TArgs...>)
    FORCEINLINE constexpr TTagRegistry(TArgs&&... Args) noexcept(std::is_nothrow_constructible_v<Allocator, TArgs...>)
        : Tags(std::forward<TArgs>(Args)...)
    {
        return;
    }

    FORCEINLINE u64 GetTagCount() const noexcept { return this->Tags.size(); }

    FORCEINLINE TagType GetTag(LStringView InRepr) const noexcept
    {
        if (const auto It{ algo::find(this->Tags, InRepr) }; It != this->Tags.end() )
        {
            return TagType(static_cast<typename TagType::SizeType>(std::distance(this->Tags.begin(), It) + 1));
        }
        return TagType{};
    }
    FORCEINLINE TagType GetTagChecked(LStringView InRepr) const noexcept
    {
        const TagType Tag {this->GetTag(InRepr)};
        check( Tag.IsSet() )
        return Tag;
    }
    FORCEINLINE TagType GetTagAsserted(LStringView InRepr) const noexcept
    {
        const TagType Tag {this->GetTag(InRepr)};
        jassert( Tag.IsSet() )
        return Tag;
    }

    FORCEINLINE constexpr bool IsTagRegistered(LStringView InRepr) const noexcept
    {
        return this->Tags.Contains(InRepr);
    }

    FORCEINLINE TagType RegisterOrGet(LStringView InRepr) noexcept
    {
        check( InRepr.empty() == false )

        if (const TagType Tag {this->GetTag(InRepr)}; Tag.IsSet())
        {
            return Tag;
        }

        this->Tags.emplace_back(std::forward<decltype(InRepr)>(InRepr));
        LOG_TRACE(LogTags, "Registered tag [{}].", this->Tags.back())

        return TagType{static_cast<typename TagType::SizeType>(this->GetTagCount())};
    }

    template <LSize N>
    FORCEINLINE TagType RegisterOrGet(const char(&InRepr)[N]) noexcept
    {
        return this->RegisterOrGet(LStringView{InRepr, N - 1});
    }

    FORCEINLINE const ReprType& GetReprFast(const TagType InTag) const { check( InTag.IsSet() ) return this->Tags[InTag.GetUnderlyingValue() - 1]; }
    FORCEINLINE ReprType GetReprSafe(const TagType InTag) const
    {
        if (InTag.IsSet() == false)
        {
            return "<NotSet>";
        }

        if (InTag.GetUnderlyingValue() - 1 < this->Tags.size())
        {
            return this->GetReprFast(InTag);
        }

        static const ReprType Unknown { "<Unknown>" };
        return Unknown;
    }

    FORCEINLINE u64 Destroy()
    {
        const u64 Count { this->Tags.size() };

        LOG_VERBOSE(LogTags, "Destroying [{}] tags from the registry.", Count)
        algo::orphan(&this->Tags);

        return Count;
    }

private:

    Allocator Tags;
};

} /* ~Namespace Lal */
