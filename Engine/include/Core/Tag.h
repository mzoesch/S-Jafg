// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

template<typename Leaf, std::unsigned_integral T, std::size_t Alignment = alignof(T)>
struct TTag
{
    template <typename TTag, typename TAllocator>
    friend struct TTagRegistry;

    typedef T value_type;
    typedef Leaf leaf;

    enum: u8 { NO_TAG = 0 };

    FORCEINLINE constexpr TTag() noexcept : Value{NO_TAG} {}
    FORCEINLINE constexpr TTag(TTag const& Other) noexcept = default;
    FORCEINLINE constexpr TTag(TTag&& Other) noexcept : Value{std::exchange(Other.Value, NO_TAG)} {}
    FORCEINLINE constexpr TTag& operator=(TTag const& Rhs) noexcept = default;
    FORCEINLINE constexpr TTag& operator=(TTag&& Rhs) noexcept { this->Value = std::exchange(Rhs.Value, NO_TAG); return *this; }
    FORCEINLINE constexpr ~TTag() noexcept = default;

    //# Convert the range of characters to a tag.
    NODISCARD static Leaf ToTag(LStringView S) noexcept
    {
        return Leaf::GetMutableRegistry().RegisterOrGet(S);
    }

    //#
    //# Get the tag version of the range of characters.
    //# @return NO_TAG if S does not map to a tag.
    //#
    NODISCARD ENGINE_API static Leaf AsTag(LStringView S)
    {
        return Leaf::GetMutableRegistry().GetTag(S);
    }
    NODISCARD FORCEINLINE static Leaf AsTagChecked(LStringView S)
    {
        Leaf Tag{Leaf::AsTag(S)};
        check(Tag.IsSet())
        return Tag;
    }

    NODISCARD LString ToString() const noexcept
    {
        return Leaf::GetMutableRegistry().GetReprSafe(static_cast<Leaf const&>(*this));
    }

    NODISCARD FORCEINLINE constexpr bool IsSet() const noexcept { return this->Value != NO_TAG; }

    NODISCARD  FORCEINLINE constexpr static bool AreEqual(TTag A, TTag B) noexcept { return A.Value == B.Value; }
    NODISCARD  FORCEINLINE constexpr bool Equals(TTag const& Other) const noexcept { return TTag::AreEqual(*this, Other); }
    NODISCARD  FORCEINLINE constexpr bool operator==(TTag const& Rhs) const noexcept { return TTag::AreEqual(*this, Rhs); }

    NODISCARD FORCEINLINE constexpr T GetUnderlyingValue() const noexcept { return this->Value; }

    ///////////////////////////////////////////////////////////////////////////////
    // Hashing only. Do not use.
    // This is meaningless as names are not deterministic.
    NODISCARD FORCEINLINE std::strong_ordering operator<=>(TTag const& Rhs) const { return this->Value <=> Rhs.Value; }
    // ~Hashing only. Do not use.
    ///////////////////////////////////////////////////////////////////////////////

private:

    FORCEINLINE explicit constexpr TTag(T V) noexcept: Value{V} {}

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
    template<typename... TArgs> requires(std::is_constructible_v<Allocator, TArgs...>)
    FORCEINLINE constexpr TTagRegistry(TArgs&&... Args) noexcept(std::is_nothrow_constructible_v<Allocator, TArgs...>)
        : Tags{std::forward<TArgs>(Args)...}
    {
        return;
    }

    FORCEINLINE auto const& GetAllocator() const noexcept { return this->Tags; }
    FORCEINLINE u64 GetTagCount() const noexcept { return this->Tags.size(); }

    FORCEINLINE TagType GetTag(LStringView InRepr) const noexcept
    {
        if (const auto It{ algo::find(this->Tags, InRepr) }; It != this->Tags.end() )
        {
            return TagType(static_cast<typename TagType::value_type>(std::distance(this->Tags.begin(), It) + 1));
        }
        return TagType{};
    }
    FORCEINLINE TagType GetTagChecked(LStringView InRepr) const noexcept
    {
        const TagType Tag {this->GetTag(InRepr)};
        check(Tag.IsSet())
        return Tag;
    }
    FORCEINLINE TagType GetTagAsserted(LStringView InRepr) const noexcept
    {
        const TagType Tag {this->GetTag(InRepr)};
        jassert(Tag.IsSet())
        return Tag;
    }

    FORCEINLINE constexpr bool IsTagRegistered(LStringView InRepr) const noexcept
    {
        return this->Tags.Contains(InRepr);
    }

    FORCEINLINE TagType RegisterOrGet(LStringView InRepr) noexcept
    {
        check(InRepr.empty() == false)

        if (const TagType Tag{this->GetTag(InRepr)}; Tag.IsSet())
        {
            return Tag;
        }

        this->Tags.emplace_back(std::forward<decltype(InRepr)>(InRepr));
        LOG_TRACE(LogTags, "Registered tag [{}].", this->Tags.back())

        return TagType{static_cast<typename TagType::value_type>(this->GetTagCount())};
    }

    template <std::size_t N>
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

        /* #IsSet checks for zero. Therefore, the result is always at least zero. And this cast is safe. */
        if (static_cast<typename TagType::value_type>(InTag.GetUnderlyingValue() - 1) < this->Tags.size())
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

} /* ~Namespace Jafg */
