// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#ifndef PRIVATE_LAL_ENSURE_STRING_INVARIANT
    #if LAL_CHECK_STRING_VALIDITY
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT() this->EnsureInvariant();
    #else /* LAL_CHECK_STRING_VALIDITY */
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    #endif /* !LAL_CHECK_STRING_VALIDITY */
#endif /* PRIVATE_LAL_ENSURE_STRING_INVARIANT */

#ifndef PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON
    #if LAL_CHECK_STRING_VALIDITY
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(What) (What).EnsureInvariant();
    #else /* LAL_CHECK_STRING_VALIDITY */
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(What)
    #endif /* !LAL_CHECK_STRING_VALIDITY */
#endif /* PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON */

namespace Lal
{

///////////////////////////////////////////////////////////////////////////////
// Concepts.
///////////////////////////////////////////////////////////////////////////////

template <typename T>
concept TStringBaseEncodingConcept = requires
{
    typename T::T;
    typename T::SizeType;

    T::Terminator;

    { T::GetStringLength(std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::GetCharacterCount(std::declval<const typename T::T*>()) }  -> std::convertible_to<typename T::SizeType>;
    { T::GetCharacterCount(std::declval<const typename T::T*>(), std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::GetCharacterSize(std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::RecoverToMostRecentValidCharacter(std::declval<typename T::T*>(), std::declval<const typename T::T*>()) } -> std::same_as<typename T::T*>;
    { T::RecoverToMostRecentValidCharacter(std::declval<const typename T::T*>(), std::declval<const typename T::T*>()) } -> std::same_as<const typename T::T*>;

    { T::GetUppercaseRune(std::declval<typename T::T>()) } -> std::same_as<typename T::T>;
    { T::GetLowercaseRune(std::declval<typename T::T>()) } -> std::same_as<typename T::T>;
};

template <typename T>
concept TStringBaseAllocatorConcept = requires(T Allocator)
{
    typename T::T;
    typename T::SizeType;
    typename T::Iterator;
    typename T::ConstIterator;
    typename T::Pointer;
    typename T::ConstPointer;
    typename T::Reference;
    typename T::ConstReference;
};

///////////////////////////////////////////////////////////////////////////////
// Implementations.
///////////////////////////////////////////////////////////////////////////////

//#
//# The base encoding for strings.
//#
template <typename TIn, std::integral TSizeType>
struct TStringBaseEncodingBase
{
    typedef TIn       T;
    typedef TSizeType SizeType;
};

//#
//# Common traits. Define your own if you need to. These are the most basic UTF-8 traits.
//#
template <typename TIn, std::integral TSizeType>
struct TStringBaseDefaultUtf8Traits : public TStringBaseEncodingBase<TIn, TSizeType>
{
    typedef TStringBaseEncodingBase<TIn, TSizeType> _Super;

    typedef typename _Super::T        T;
    typedef typename _Super::SizeType SizeType;

    inline static constexpr T Terminator { '\0' };

    UTILITY_STRUCT(TStringBaseDefaultUtf8Traits)

    //#
    //# @return The length of the string, excluding the null terminator in runes.
    //#
    template <typename T>
    FORCEINLINE static SizeType GetStringLength(const T* String) noexcept;

    //#
    //# @return The length of the string, excluding the null terminator in characters.
    //#
    //# @note This just includes basic UTF-8 characters, not any extended characters or compound shit.
    //#
    template <typename T>
    static SizeType GetCharacterCount(const T* String) noexcept;
    template <typename T>
    static SizeType GetCharacterCount(const T* Begin, const T* End) noexcept;

    //#
    //# The length of one character in runes.
    //#
    static SizeType GetCharacterSize(const T* Character) noexcept;

    //#
    //# Recover the given pointer #Character to the most recent valid UTF-8 Character.
    //# @param Guard The max the Character may go back. If this is not given, it will go back until one valid character
    //#              is found. The #Terminator also qualifies as a valid character.
    //#
    static T* RecoverToMostRecentValidCharacter(T* Character, const T* Guard = nullptr);
    static const T* RecoverToMostRecentValidCharacter(const T* Character, const T* Guard = nullptr);

    static T GetUppercaseRune(const T Rune);
    static T GetLowercaseRune(const T Rune);
};

template <typename TIn, std::integral TSizeType>
template <typename T>
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetStringLength(const T* String) noexcept
{
    check( String )

    SizeType Out { 0 };

    while (*String++ != TStringBaseDefaultUtf8Traits::Terminator)
    {
        ++Out;
    }

    return Out;
}

template <typename TIn, std::integral TSizeType>
template <typename T>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterCount(const T* String) noexcept
{
    check( String )

    SizeType Out { 0 };

    while (*String != TStringBaseDefaultUtf8Traits::Terminator)
    {
        ++Out;
        String += TStringBaseDefaultUtf8Traits::GetCharacterSize(String);

        continue;
    }

    return Out;
}

template <typename TIn, std::integral TSizeType>
template <typename T>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterCount(const T* Begin, const T* End) noexcept
{
    if (LAL_UNLIKELY(Begin == nullptr))
    {
        check( End == nullptr )
        return 0;
    }

    SizeType Out { 0 };

    while (Begin != End)
    {
        check( *Begin != TStringBaseDefaultUtf8Traits::Terminator )

        ++Out;
        Begin += TStringBaseDefaultUtf8Traits::GetCharacterSize(Begin);

        continue;
    }

    return Out;
}

template <typename TIn, std::integral TSizeType>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterSize(const T* Character) noexcept
{
    checkSlow( Character )

    /* ASCII byte. */
    if ((*Character & 0x80) == 0)
    {
        return 1;
    }

    /* 2-byte sequence. */
    if ((*Character & 0xE0) == 0xC0)
    {
        return 2;
    }

    /* 3-byte sequence. */
    if ((*Character & 0xF0) == 0xE0)
    {
        return 3;
    }

    /* 4-byte sequence. */
    if ((*Character & 0xF8) == 0xF0)
    {
        return 4;
    }

    checkNoEntry()
    /* Invalid UTF-8. */
    // Return 1; So we do not hang indefinitely.
    return 1;
}

template <typename TIn, std::integral TSizeType>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T* TStringBaseDefaultUtf8Traits<TIn, TSizeType>::RecoverToMostRecentValidCharacter(T* Character, const T* Guard /* = nullptr */)
{
    checkSlow( Character )

    while ((Guard && Character > Guard) && *Character != TStringBaseDefaultUtf8Traits::Terminator)
    {
        if
        (
               ((*Character & 0x80) == 0x00) /* ASCII byte. (0xxx xxxx) */
            || ((*Character & 0xC0) == 0xC0) /* Multi-byte. (11xx xxxx) */
        )
        {
            break;
        }

        --Character;
    }

    return Character;
}

template <typename TIn, std::integral TSizeType>
const typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T* TStringBaseDefaultUtf8Traits<TIn, TSizeType>::RecoverToMostRecentValidCharacter(const T* Character, const T* Guard /* = nullptr */)
{
    checkSlow( Character )

    while ((Guard && Character > Guard) && *Character != TStringBaseDefaultUtf8Traits::Terminator)
    {
        if
        (
               ((*Character & 0x80) == 0x00) /* ASCII byte. (0xxx xxxx) */
            || ((*Character & 0xC0) == 0xC0) /* Multi-byte. (11xx xxxx) */
        )
        {
            break;
        }

        --Character;
    }

    return Character;
}

template <typename TIn, std::integral TSizeType>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetUppercaseRune(const T Rune)
{
    if (Rune >= 'a' && Rune <= 'z')
    {
        return Rune - 32;
    }

    return Rune;
}

template <typename TIn, std::integral TSizeType>
typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetLowercaseRune(const T Rune)
{
    if (Rune >= 'A' && Rune <= 'Z')
    {
        return Rune + 32;
    }

    return Rune;
}

static_assert(TStringBaseEncodingConcept<TStringBaseDefaultUtf8Traits<LChar, LSize>>);

//#
//# Default Lal string implementation.
//#
//# Supports ASCII and UTF-8 encoded strings by default. You may add your own encodings.
//#
//# This string allows for heap, stack allocation, or a mixture, depending on the size of the string.
//#
template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TStringBase
{
public:

    typedef TEncoding<
        typename TAllocator::T,
        typename TAllocator::SizeType
        >  Encoding;

    typedef TAllocator                         Allocator;
    typedef typename TAllocator::T             T;
    typedef typename TAllocator::SizeType      SizeType;
    typedef typename Allocator::Iterator       Iterator;
    typedef typename Allocator::ConstIterator  ConstIterator;
    typedef typename Allocator::Pointer        Pointer;
    typedef typename Allocator::ConstPointer   ConstPointer;
    typedef typename Allocator::Reference      Reference;
    typedef typename Allocator::ConstReference ConstReference;

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArrayBase<TMemberField>* MemberField);

    template <template <typename, typename> typename UEncoding, TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<UEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    friend class TStringBase;

    NODISCARD
    FORCEINLINE consteval static bool IsStringView() noexcept requires(requires { Allocator::IsStringView; }) { return Allocator::IsStringView(); }
    NODISCARD
    FORCEINLINE consteval static bool IsStringView() noexcept requires(!requires { Allocator::IsStringView; } && requires { Allocator::IsStronglyAllocated; }) { return !Allocator::IsStronglyAllocated(); }
    NODISCARD
    FORCEINLINE consteval static bool IsStringView() noexcept requires(!requires { Allocator::IsStringView; } && !requires { Allocator::IsStronglyAllocated; }) { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsOwningString() noexcept { return TStringBase::IsStringView() == false; }

    FORCEINLINE constexpr TStringBase() noexcept requires(std::is_default_constructible_v<Allocator>) = default;
    FORCEINLINE constexpr TStringBase(LNullptrTy) noexcept requires(std::is_default_constructible_v<Allocator>) : Impl{} { }

    FORCEINLINE constexpr TStringBase(const TStringBase& Other) noexcept
        requires(std::is_constructible_v<Allocator, const Allocator&>);
    FORCEINLINE constexpr TStringBase& operator=(const TStringBase& Other) noexcept
        requires(std::assignable_from<Allocator&, const Allocator&>);

    FORCEINLINE constexpr TStringBase(TStringBase&& Other) noexcept
        requires(std::is_constructible_v<Allocator, Allocator&&>);
    FORCEINLINE constexpr TStringBase& operator=(TStringBase&& Other) noexcept
        requires(std::assignable_from<Allocator&, Allocator&&>);

    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE explicit constexpr TStringBase(const TStringBase<TEncoding, UAllocator>& Other) noexcept
        requires(std::is_constructible_v<Allocator, const UAllocator&>);
    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE constexpr TStringBase& operator=(const TStringBase<TEncoding, UAllocator>& Other) noexcept
        requires(std::assignable_from<Allocator&, const UAllocator&>);

    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE explicit constexpr TStringBase(TStringBase<TEncoding, UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&>);
    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE constexpr TStringBase& operator=(TStringBase<TEncoding, UAllocator>&& Other) noexcept
        requires(std::assignable_from<Allocator&, UAllocator&&>);

    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE explicit constexpr TStringBase(TStringBase<TEncoding, UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&> == false) = delete;
    template <TStringBaseAllocatorConcept UAllocator>
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
    FORCEINLINE TStringBase& operator=(TStringBase<TEncoding, UAllocator>&& Other) noexcept
        requires(std::assignable_from<Allocator&, UAllocator&&> == false) = delete;

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE constexpr TStringBase(const UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, VIterator>);
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr TStringBase(const UIterator Begin, const SizeType Length) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, UIterator>);

    FORCEINLINE constexpr TStringBase(const Pointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase(const Pointer String) noexcept
        requires(!TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase(const Pointer Begin, const Pointer End) noexcept
        requires(std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase& operator=(const Pointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase& operator=(const Pointer String) noexcept
        requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); });

    FORCEINLINE constexpr TStringBase(const ConstPointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase(const ConstPointer String) noexcept
        requires(!TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase(const ConstPointer Begin, const ConstPointer End) noexcept
        requires(std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase& operator=(const ConstPointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase& operator=(const ConstPointer String) noexcept
        requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); } && std::is_same_v<Pointer, ConstPointer> == false)
    {
        const SizeType Length { Encoding::GetStringLength(String) };

        this->Reset(Length);
        this->Impl.Append(String, String + Length + /*Terminator*/1);

        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        return *this;
    }

    FORCEINLINE constexpr ~TStringBase() noexcept = default;

    //# Equivalent to #c_str() on std::string. Always returns a c style string.
    NODISCARD
    FORCEINLINE ConstPointer ToPtr() const noexcept requires(!TStringBase::IsStringView()) { if (this->GetRuneCount() > 0) { return this->Impl.GetDataPointer(); } return &Encoding::Terminator; }
    NODISCARD
    FORCEINLINE SizeType GetRuneCount() const noexcept requires(TStringBase::IsStringView());
    NODISCARD
    FORCEINLINE SizeType GetRuneCount() const noexcept requires(!TStringBase::IsStringView());
    NODISCARD
    FORCEINLINE SizeType GetCharacterCount() const noexcept { return Encoding::GetCharacterCount(this->begin_ptr(), this->end_ptr()); }
    NODISCARD
    FORCEINLINE SizeType GetAllocatedByteSize() const noexcept { return this->Impl.GetAllocatedByteSize(); }
    NODISCARD
    FORCEINLINE bool IsEmpty() const noexcept { return this->GetRuneCount() == 0; }

    NODISCARD FORCEINLINE constexpr bool IsValidPointer(const ConstPointer Ptr) const noexcept { return Ptr >= this->begin_ptr() && Ptr < this->end_ptr(); }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsValidIterator(const TIterator& It) const noexcept { return It.Cursor >= this->begin_ptr() && It.Cursor < this->end_ptr(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires( std::is_signed_v<SizeType>) { return Index >= 0 && Index < this->GetRuneCount(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires(!std::is_signed_v<SizeType>) { return Index < this->GetRuneCount(); }
    NODISCARD FORCEINLINE constexpr bool IsValidEndPointer(const ConstPointer Ptr) const noexcept { return Ptr >= this->begin_ptr() && Ptr <= this->end_ptr(); }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsValidEndIterator(const TIterator& It) const noexcept { return It.Cursor >= this->begin_ptr() && It.Cursor <= this->end_ptr(); }

    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr typename TIterator::Reference operator[](const TIterator It) noexcept requires( TAllocator::IsContentMutable() && TStringBase::IsStringView()) { return this->Impl[It]; }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr typename TIterator::Reference operator[](const TIterator It) noexcept requires(!TAllocator::IsContentMutable() && TStringBase::IsStringView()) { check( this->IsValidIterator(It) ) return this->Impl[It]; }
    NODISCARD FORCEINLINE constexpr Reference operator[](const SizeType Index) noexcept requires( TAllocator::IsContentMutable() && TStringBase::IsStringView()) { return this->Impl[Index]; }
    NODISCARD FORCEINLINE constexpr Reference operator[](const SizeType Index) noexcept requires(!TAllocator::IsContentMutable() && TStringBase::IsStringView()) { check( this->IsValidIndex(Index) ) return this->Impl[Index]; }

    FORCEINLINE constexpr Iterator begin() noexcept { return this->Impl.begin(); }
    FORCEINLINE constexpr Iterator end() noexcept requires(TStringBase::IsStringView()) { return this->Impl.end(); }
    FORCEINLINE constexpr Iterator end() noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.end() }; return It == this->begin() ? It : --It; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return this->Impl.begin(); }
    FORCEINLINE constexpr ConstIterator end() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.end(); }
    FORCEINLINE constexpr ConstIterator end() const noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.end() }; return It == this->begin() ? It : --It; }
    FORCEINLINE constexpr ConstIterator cbegin() const noexcept { return this->Impl.cbegin(); }
    FORCEINLINE constexpr ConstIterator cend() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.cend(); }
    FORCEINLINE constexpr ConstIterator cend() const noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.cend() }; return It == this->cbegin() ? It : --It; }

    FORCEINLINE constexpr Pointer begin_ptr() noexcept { return this->Impl.begin_ptr(); }
    FORCEINLINE constexpr Pointer end_ptr() noexcept requires(TStringBase::IsStringView()) { return this->Impl.end_ptr(); }
    FORCEINLINE constexpr Pointer end_ptr() noexcept requires(!TStringBase::IsStringView()) { auto Ptr { this->Impl.end_ptr() }; return Ptr == this->begin_ptr() ? Ptr : --Ptr; }
    FORCEINLINE constexpr ConstPointer begin_ptr() const noexcept { return this->Impl.begin_ptr(); }
    FORCEINLINE constexpr ConstPointer end_ptr() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.end_ptr(); }
    FORCEINLINE constexpr ConstPointer end_ptr() const noexcept requires(!TStringBase::IsStringView()) { auto Ptr { this->Impl.end_ptr() }; return Ptr == this->begin_ptr() ? Ptr : --Ptr; }

    FORCEINLINE constexpr SizeType end_idx() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.end_idx(); }
    FORCEINLINE constexpr SizeType end_idx() const noexcept requires(!TStringBase::IsStringView()) { return Jafg::Maths::Max<SizeType>(this->Impl.end_idx() - 1, static_cast<SizeType>(0)); }

    FORCEINLINE constexpr auto Iter() noexcept { return this->Impl.Iter(); }
    FORCEINLINE constexpr auto CIter() const noexcept { return this->Impl.CIter(); }

    FORCEINLINE constexpr void Reserve(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.Reserve(Count + /*Terminator*/1); }
    FORCEINLINE constexpr void ReserveAdditionally(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.ReserveAdditionally(Count); }
    FORCEINLINE constexpr void Reset(const SizeType Count, const bool bAllowShrinking = true) noexcept requires(TAllocator::IsStronglyAllocated());
    FORCEINLINE constexpr void Resize(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated());
    FORCEINLINE constexpr void ShrinkToFit() noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.ShrinkToFit(); }
    FORCEINLINE constexpr void Empty() noexcept { this->Impl.Empty(); }
    FORCEINLINE constexpr void SwapStrings(TStringBase* Other) noexcept requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); }) { check( Other ) this->Impl.SwapBuffers(&Other->Impl); }

    FORCEINLINE void SwapIndices(const Iterator InA, const Iterator InB) noexcept requires(Allocator::IsContentMutable()) { check( this->IsValidIterator(InA) ) check( this->IsValidIterator(InB) ) this->Impl.SwapIndices(InA, InB); }
    FORCEINLINE void SwapIndices(const SizeType InA, const SizeType InB) noexcept requires(Allocator::IsContentMutable()) { this->SwapIndices(this->begin() + InA, this->begin() + InB); }

    FORCEINLINE bool operator==(LNullptrTy) const noexcept { return this->IsEmpty(); }
    FORCEINLINE bool operator!=(LNullptrTy) const noexcept { return this->operator==(nullptr) == false; }

    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool operator==(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>);
    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool operator!=(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>);

    NODISCARD
    FORCEINLINE constexpr bool operator==(const ConstPointer String) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool operator!=(const ConstPointer String) const noexcept;

    NODISCARD
    FORCEINLINE constexpr bool operator==(const T Rune) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool operator!=(const T Rune) const noexcept { return this->operator==(Rune) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(LNullptrTy) const noexcept { return *this == nullptr; }
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(LNullptrTy) const noexcept { return this->Equals(nullptr) == false; }

    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool Equals(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>) { return this->operator==(Other); }
    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
        requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>) { return this->Equals(Other) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(const ConstPointer String) const noexcept { return this->operator==(String); }
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(const ConstPointer String) const noexcept { return this->Equals(String) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(const T Rune) const noexcept { return this->operator==(Rune); }
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(const T Rune) const noexcept { return this->Equals(Rune) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(const ConstPointer Begin, const ConstPointer End) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->Equals(Begin, End) == false; }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    NODISCARD
    FORCEINLINE constexpr bool Equals(UIterator Begin, const VIterator End) const noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(const UIterator Begin, const VIterator End) const noexcept { return this->Equals(Begin, End) == false; }

    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    constexpr std::strong_ordering operator<=>(const TStringBase<TEncoding, UAllocator>& Other) noexcept { return this->SpaceShip(Other.begin(), Other.end()); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(const ConstPointer String) const noexcept { return this->SpaceShip(ConstIterator{String}, ConstIterator{String + Encoding::GetStringLength(String)}); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(const T Rune) const noexcept { return this->SpaceShip(Rune); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering SpaceShip(const T Rune) const noexcept;
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering SpaceShip(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->SpaceShip(ConstIterator{Begin}, ConstIterator{End}); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    NODISCARD
    constexpr std::strong_ordering SpaceShip(const UIterator Begin, const VIterator End) const noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    TStringBase& Assign(const TStringBase<TEncoding, UAllocator>& Other) & noexcept { return this->Assign(Other.begin(), Other.end()); }
    TStringBase& Assign(const ConstPointer String) & noexcept { return this->Assign(String, String + Encoding::GetStringLength(String)); }
    TStringBase& Assign(const ConstPointer String, const SizeType Length) & noexcept { return this->Assign(String, String + Length); }
    TStringBase& Assign(const ConstPointer Begin, const ConstPointer End) & noexcept { return this->Assign(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    TStringBase& Assign(const UIterator Begin, const VIterator End) & noexcept requires(TStringBase::IsStringView()) { this->Impl = Allocator{ Begin.Cursor, End.Cursor }; PRIVATE_LAL_ENSURE_STRING_INVARIANT() return *this; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    TStringBase& Assign(const UIterator Begin, const VIterator End) & noexcept requires(!TStringBase::IsStringView()) { this->AssignImpl(Begin, End); return *this; }
    TStringBase& Assign(const T Rune) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->AssignImpl(&Rune, &Rune + 1); return *this; }

    template <TStringBaseAllocatorConcept UAllocator>
    TStringBase&& Assign(const TStringBase<TEncoding, UAllocator>& Other) && noexcept { return std::move(this->Assign(Other.begin(), Other.end())); }
    TStringBase&& Assign(const ConstPointer String) && noexcept { return std::move(this->Assign(String, String + Encoding::GetStringLength(String))); }
    TStringBase&& Assign(const ConstPointer String, const SizeType Length) && noexcept { return std::move(this->Assign(String, String + Length)); }
    TStringBase&& Assign(const ConstPointer Begin, const ConstPointer End) && noexcept { return std::move(this->Assign(ConstIterator{ Begin }, ConstIterator{ End })); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    TStringBase&& Assign(const UIterator Begin, const VIterator End) && noexcept requires(TStringBase::IsStringView()) { this->Impl = Allocator{ Begin.Cursor, End.Cursor }; PRIVATE_LAL_ENSURE_STRING_INVARIANT() return std::move(*this); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    TStringBase&& Assign(const UIterator Begin, const VIterator End) && noexcept requires(!TStringBase::IsStringView()) { this->AssignImpl(Begin, End); return std::move(*this); }
    TStringBase&& Assign(const T Rune) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AssignImpl(&Rune, &Rune + 1); return std::move(*this); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE void Append(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Other.begin(), Other.end()); }
    FORCEINLINE void Append(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE void Append(const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(String, String + Length); }
    FORCEINLINE void Append(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Append(const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE void Append(const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(&Rune, &Rune + 1); }

    //#
    //# Appends the specified string or rune at the given iterator position.
    //# @return The iterator to the first newly inserted rune. If the range is empty, the iterator will point to
    //#         the slack.
    //#
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, Other.begin(), Other.end()); }
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, String, String + Length); }
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It.Cursor - this->Impl.GetDataPointer(), Begin, End); }
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, &Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, Other.begin(), Other.end()); }
    FORCEINLINE Iterator AppendAt(const SizeType Index, const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE Iterator AppendAt(const SizeType Index, const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, String, String + Length); }
    FORCEINLINE Iterator AppendAt(const SizeType Index, const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE Iterator AppendAt(const SizeType Index, const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE Iterator AppendAt(const SizeType Index, const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, &Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator, TStringBaseAllocatorConcept VAllocator = Allocator> requires(TStringBase<TEncoding, VAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, VAllocator> AppendToNew(const TStringBase<TEncoding, UAllocator>& Other) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendToNew<ConstIterator, ConstIterator, VAllocator>(Other.begin(), Other.end()); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(const ConstPointer String) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendToNew<UAllocator>(String, String + Encoding::GetStringLength(String)); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(const ConstPointer String, const SizeType Length) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendToNew<UAllocator>(String, String + Length); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(const ConstPointer Begin, const ConstPointer End) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendToNew<ConstIterator, ConstIterator, UAllocator>(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator = Allocator> requires(IsValidIteratorPair<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(const UIterator Begin, const VIterator End) const noexcept requires(Allocator::IsAllowedToPushItems());
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(const T Rune) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendToNew<UAllocator>(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase operator+(const TStringBase<TEncoding, UAllocator>& Other) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendToNew(Other.begin(), Other.end()); }
    FORCEINLINE TStringBase operator+(const ConstPointer String) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendToNew(String); }
    FORCEINLINE TStringBase operator+(const T Rune) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendToNew(Rune); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase&& operator+(const TStringBase<TEncoding, UAllocator>& Other) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Other.begin(), Other.end()); return std::move(*this); }
    FORCEINLINE TStringBase&& operator+(const ConstPointer String) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(String); return std::move(*this); }
    FORCEINLINE TStringBase&& operator+(const T Rune) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Rune); return std::move(*this); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase& operator+=(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Other); return *this; }
    FORCEINLINE TStringBase& operator+=(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(String); return *this; }
    FORCEINLINE TStringBase& operator+=(const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Rune); return *this; }

    //#
    //# Appends a POSIX path to this string following basic rules. This guarantees that there is a '/' between the
    //# two strings inserted or omitted if the string has a trailing '/' already.
    //#
    //# - "a"  /  "b" -> "a/b"
    //# - "a/" /  "b" -> "a/b"
    //# - "a"  / "/b" -> "a/b"
    //# - "a/" / "/b" -> "a/b"
    //#
    //# But WATCHOUT:
    //# - "a//" / "/b"  -> "a//b"  # Will not remove double slashes if already present.
    //# - "a//" / "//b" -> "a///b"
    //#
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE void AppendPath(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other.begin(), Other.end()); }
    FORCEINLINE void AppendPath(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String, Encoding::GetStringLength(String)); }
    FORCEINLINE void AppendPath(const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String, String + Length); }
    FORCEINLINE void AppendPath(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void AppendPath(UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE void AppendPath(const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator, TStringBaseAllocatorConcept VAllocator = Allocator> requires(TStringBase<TEncoding, VAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, VAllocator> AppendPathToNew(const TStringBase<TEncoding, UAllocator>& Other) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<ConstIterator, ConstIterator, VAllocator>(Other.begin(), Other.end()); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendPathToNew(const ConstPointer String) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<UAllocator>(String, Encoding::GetStringLength(String)); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendPathToNew(const ConstPointer String, const SizeType Length) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<UAllocator>(String, String + Length); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendPathToNew(const ConstPointer Begin, const ConstPointer End) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<ConstIterator, ConstIterator, UAllocator>(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator = Allocator> requires(IsValidIteratorPair<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendPathToNew(const UIterator Begin, const VIterator End) const noexcept requires(Allocator::IsAllowedToPushItems());
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendPathToNew(const T Rune) const noexcept requires(Allocator::IsAllowedToPushItems()) { return this->template AppendPathToNew<UAllocator>(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase operator/(const TStringBase<TEncoding, UAllocator>& Other) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendPathToNew(Other.begin(), Other.end()); }
    FORCEINLINE TStringBase operator/(const ConstPointer String) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendPathToNew(String); }
    FORCEINLINE TStringBase operator/(const T Rune) & noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendPathToNew(Rune); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase&& operator/(const TStringBase<TEncoding, UAllocator>& Other) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other.begin(), Other.end()); return std::move(*this); }
    FORCEINLINE TStringBase&& operator/(const ConstPointer String) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String); return std::move(*this); }
    FORCEINLINE TStringBase&& operator/(const T Rune) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Rune); return std::move(*this); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase& operator/=(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other); return *this; }
    FORCEINLINE TStringBase& operator/=(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String); return *this; }
    FORCEINLINE TStringBase& operator/=(const T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Rune); return *this; }

    template <TIteratorConcept UIterator>
    FORCEINLINE void RemoveAt(const UIterator It) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(It, It + 1); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void RemoveAt(const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Begin.Cursor - this->Impl.GetDataPointer(), End.Cursor - this->Impl.GetDataPointer()); }
    FORCEINLINE void RemoveAt(const SizeType Index) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Index, Index + 1); }
    FORCEINLINE void RemoveAt(const SizeType Begin, const SizeType End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE void RemoveAt(const ConstPointer Ptr) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Ptr - this->Impl.GetDataPointer(), (Ptr + 1) - this->Impl.GetDataPointer()); }
    FORCEINLINE void RemoveAt(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Begin - this->Impl.GetDataPointer(), End - this->Impl.GetDataPointer()); }
    FORCEINLINE void RemoveAt(const ConstPointer Ptr, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Ptr - this->Impl.GetDataPointer(), (Ptr + Length) - this->Impl.GetDataPointer()); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType Remove(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(Other.begin(), Other.end(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Other, bAllowShrinking) }; check( Count > 0 ) return Count; }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Other, bAllowShrinking) }; jassert( Count > 0 ) return Count; }
    FORCEINLINE SizeType Remove(const ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(String, String + Encoding::GetStringLength(String), bAllowShrinking); }
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(String, bAllowShrinking) }; check( Count > 0 ) return Count; }
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(String, bAllowShrinking) }; jassert( Count > 0 ) return Count; }
    FORCEINLINE SizeType Remove(const ConstPointer String, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(String, String + Length, bAllowShrinking); }
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const ConstPointer String, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(String, Length, bAllowShrinking) }; check( Count > 0 ) return Count; }
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const ConstPointer String, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(String, Length, bAllowShrinking) }; jassert( Count > 0 ) return Count; }
    FORCEINLINE SizeType Remove(const ConstPointer Begin, const ConstPointer End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(ConstIterator{ Begin }, ConstIterator{ End }, bAllowShrinking); }
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const ConstPointer Begin, const ConstPointer End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Begin, End, bAllowShrinking) }; check( Count > 0 ) return Count; }
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const ConstPointer Begin, const ConstPointer End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Begin, End, bAllowShrinking) }; jassert( Count > 0 ) return Count; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Remove(const UIterator Begin, const VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const UIterator Begin, const VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Begin, End, bAllowShrinking) }; check( Count > 0 ) return Count; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const UIterator Begin, const VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Begin, End, bAllowShrinking) }; jassert( Count > 0 ) return Count; }
    FORCEINLINE SizeType Remove(const T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Rune, bAllowShrinking) }; check( Count > 0 ) return Count; }
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const SizeType Count { this->Remove(Rune, bAllowShrinking) }; jassert( Count > 0 ) return Count; }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnce(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(Other.begin(), Other.end()); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnceChecked(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Other) }; check( Result ) return Result; }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Other) }; jassert( Result ) return Result; }
    FORCEINLINE bool RemoveOnce(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE bool RemoveOnceChecked(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(String) }; check( Result ) return Result; }
    FORCEINLINE bool RemoveOnceAsserted(const ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(String) }; jassert( Result ) return Result; }
    FORCEINLINE bool RemoveOnce(const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(String, String + Length); }
    FORCEINLINE bool RemoveOnceChecked(const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(String, Length) }; check( Result ) return Result; }
    FORCEINLINE bool RemoveOnceAsserted(const ConstPointer String, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(String, Length) }; jassert( Result ) return Result; }
    FORCEINLINE bool RemoveOnce(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(ConstIterator{ Begin }, ConstIterator{ End }); }
    FORCEINLINE bool RemoveOnceChecked(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Begin, End) }; check( Result ) return Result; }
    FORCEINLINE bool RemoveOnceAsserted(const ConstPointer Begin, const ConstPointer End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Begin, End) }; jassert( Result ) return Result; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnce(const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnceChecked(const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Begin, End) }; check( Result ) return Result; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnceAsserted(const UIterator Begin, const VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Begin, End) }; jassert( Result ) return Result; }
    FORCEINLINE bool RemoveOnce(const T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnceChecked(const T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Rune) }; check( Result ) return Result; }
    FORCEINLINE bool RemoveOnceAsserted(const T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { const bool Result { this->RemoveOnce(Rune) }; jassert( Result ) return Result; }

    //#
    //# Replace all occurrences of the input string with the replacement string.
    //# @return The number of substrings replaced.
    //#
    FORCEINLINE SizeType Replace(const T Rune, const T Replacement) noexcept requires(Allocator::IsContentMutable());
    FORCEINLINE SizeType Replace(const ConstPointer String, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(String, Encoding::GetStringLength(String), Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }

    FORCEINLINE SizeType Replace(const ConstPointer Begin, const SizeType Length, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const SizeType Length, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ ReplacementEnd }, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Begin + Length, OtherBegin, OtherEnd, bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Begin + Length, Rune); }

    FORCEINLINE SizeType Replace(const ConstPointer Begin, const ConstPointer End, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const ConstPointer End, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ ReplacementEnd }, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd, bAllowShrinking); }
    FORCEINLINE SizeType Replace(const ConstPointer Begin, const ConstPointer End, const T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(ConstIterator{ Begin }, ConstIterator{ End }, Rune, bAllowShrinking); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const UIterator Begin, const VIterator End, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const UIterator Begin, const VIterator End, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const UIterator Begin, const VIterator End, const ConstPointer ReplacementBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(Allocator::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, ConstIterator{ ReplacementBegin }, ConstIterator{ ReplacementEnd }, bAllowShrinking, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                SizeType Replace(UIterator Begin, const VIterator End, WIterator ReplacementBegin, const XIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(const UIterator Begin, const VIterator End, const T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, &Rune, &Rune + 1, bAllowShrinking); }

    //#
    //# Substitutes a range of runes with a replacement string.
    //#
    FORCEINLINE void Substitute(const ConstPointer Begin, const SizeType Length, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const SizeType Length, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const SizeType Length, const ConstPointer ReplacementBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, ConstIterator{ ReplacementBegin }, ConstIterator{ ReplacementEnd }, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const ConstPointer Begin, const SizeType Length, const UIterator ReplacementBegin, const VIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Begin + Length, ReplacementBegin, ReplacementEnd, bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, &Rune, &Rune + 1); }

    FORCEINLINE void Substitute(const ConstPointer Begin, const ConstPointer End, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const ConstPointer End, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const ConstPointer End, const ConstPointer ReplacementBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Iterator{ Begin }, Iterator{ End }, ConstIterator{ ReplacementBegin }, ConstIterator{ ReplacementEnd }), bAllowShrinking; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const ConstPointer Begin, const ConstPointer End, const UIterator ReplacementBegin, const VIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Iterator{ Begin }, Iterator{ End }, ReplacementBegin, ReplacementEnd, bAllowShrinking); }
    FORCEINLINE void Substitute(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Iterator{ Begin }, Iterator{ End }, &Rune, &Rune + 1); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const UIterator Begin, const VIterator End, const ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const UIterator Begin, const VIterator End, const ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const UIterator Begin, const VIterator End, const ConstPointer ReplacementBegin, const ConstPointer ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, ConstIterator{ ReplacementBegin }, ConstIterator{ ReplacementEnd }, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                void Substitute(UIterator Begin, VIterator End, const WIterator ReplacementBegin, const XIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void Substitute(const UIterator Begin, const VIterator End, const T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { check( Rune != Encoding::Terminator ) this->Substitute(Begin, End, &Rune, &Rune + 1, bAllowShrinking); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool StartsWith(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->StartsWith(Other.begin(), Other.end()); }
    FORCEINLINE bool StartsWith(const ConstPointer String) const noexcept { return this->StartsWith(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE bool StartsWith(const ConstPointer String, const SizeType Length) const noexcept { return this->StartsWith(String, String + Length); }
    FORCEINLINE bool StartsWith(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->StartsWith(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool StartsWith(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::StartsWith(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool StartsWith(const T Rune) const noexcept { return *this->ToPtr() == Rune; /* Better performance. */ }

    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::StartsWith(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::StartsWith(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::StartsWith(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::StartsWith(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::StartsWith(Begin, Begin + Length, Rune); }

    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::StartsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::StartsWith(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::StartsWith(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::StartsWith(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static bool StartsWith(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::StartsWith(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::StartsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::StartsWith(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::StartsWith(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static bool StartsWith(UIterator Begin, const VIterator End, WIterator OtherBegin, const XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(const UIterator Begin, const VIterator End, const T Rune) noexcept { if (Begin != End) { return *Begin == Rune; } return false; /* Better performance. */ }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool EndsWith(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->EndsWith(Other.begin(), Other.end()); }
    FORCEINLINE bool EndsWith(const ConstPointer String) const noexcept { return this->EndsWith(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE bool EndsWith(const ConstPointer String, const SizeType Length) const noexcept { return this->EndsWith(String, String + Length); }
    FORCEINLINE bool EndsWith(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->EndsWith(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool EndsWith(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::EndsWith(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool EndsWith(const T Rune) const noexcept { return TStringBase::EndsWith(this->begin(), this->end(), Rune); }

    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::EndsWith(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::EndsWith(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::EndsWith(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::EndsWith(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::EndsWith(Begin, Begin + Length, Rune); }

    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::EndsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::EndsWith(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::EndsWith(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::EndsWith(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static bool EndsWith(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::EndsWith(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::EndsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::EndsWith(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::EndsWith(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static bool EndsWith(const UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static bool EndsWith(const UIterator Begin, const VIterator End, const T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindFirst(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindFirst(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindFirst(const ConstPointer String) const noexcept { return this->FindFirst(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindFirst(const ConstPointer String, const SizeType Length) const noexcept { return this->FindFirst(String, String + Length); }
    FORCEINLINE ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->FindFirst(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindFirst(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::FindFirst(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindFirst(const T Rune) const noexcept { return TStringBase::FindFirst(this->begin(), this->end(), Rune); }

    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindFirst(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindFirst(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindFirst(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::FindFirst(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindFirst(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindFirst(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindFirst(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindFirst(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::FindFirst(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindFirst(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindFirst(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static UIterator FindFirst(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static UIterator FindFirst(UIterator Begin, const VIterator End, const T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindSecond(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindSecond(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindSecond(const ConstPointer String) const noexcept { return this->FindSecond(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindSecond(const ConstPointer String, const SizeType Length) const noexcept { return this->FindSecond(String, String + Length); }
    FORCEINLINE ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->FindSecond(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindSecond(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::FindSecond(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindSecond(const T Rune) const noexcept { return TStringBase::FindSecond(this->begin(), this->end(), Rune); }

    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::FindSecond(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecond(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecond(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindSecond(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::FindSecond(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::FindSecond(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecond(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecond(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindSecond(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindSecond(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::FindSecond(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::FindSecond(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecond(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecond(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept { return TStringBase::FindNth(Begin, End, OtherBegin, OtherEnd, 2); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, const VIterator End, const T Rune) noexcept { return TStringBase::FindNth(Begin, End, Rune, 2); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindNth(const TStringBase<TEncoding, UAllocator>& Other, const SizeType N) const noexcept { return this->FindNth(Other.begin(), Other.end(), N); }
    FORCEINLINE ConstIterator FindNth(const ConstPointer String, const SizeType N) const noexcept { return this->FindNth(String, Encoding::GetStringLength(String), N); }
    FORCEINLINE ConstIterator FindNth(const ConstPointer String, const SizeType Length, const SizeType N) const noexcept { return this->FindNth(String, String + Length, N); }
    FORCEINLINE ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const SizeType N) const noexcept { return this->FindNth(ConstIterator{ Begin }, ConstIterator{ End }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindNth(const UIterator Begin, const VIterator End, const SizeType N) const noexcept { return TStringBase::FindNth(this->begin(), this->end(), Begin, End, N); }
    FORCEINLINE ConstIterator FindNth(const T Rune, const SizeType N) const noexcept { return TStringBase::FindNth(this->begin(), this->end(), Rune, N); }

    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Length, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Length, Other, Other + OtherLength, N); }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Begin + Length, OtherBegin, OtherEnd, N); }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const SizeType Length, const T Rune, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Begin + Length, Rune, N); }

    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, Other, Other + OtherLength), N; }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd), N; }
    FORCEINLINE static ConstIterator FindNth(const ConstPointer Begin, const ConstPointer End, const T Rune, const SizeType N) noexcept { return TStringBase::FindNth(ConstIterator{ Begin }, ConstIterator{ End }, Rune), N; }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, Other, Other + OtherLength), N; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static UIterator FindNth(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd, SizeType N) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static UIterator FindNth(UIterator Begin, const VIterator End, const T Rune, SizeType N) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindLast(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindLast(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindLast(const ConstPointer String) const noexcept { return this->FindLast(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindLast(const ConstPointer String, const SizeType Length) const noexcept { return this->FindLast(String, String + Length); }
    FORCEINLINE ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->FindLast(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindLast(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::FindLast(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindLast(const T Rune) const noexcept { return TStringBase::FindLast(this->begin(), this->end(), Rune); }

    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::FindLast(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindLast(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindLast(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindLast(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::FindLast(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::FindLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindLast(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindLast(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindLast(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::FindLast(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindLast(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::FindLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindLast(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindLast(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindLast(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static UIterator FindLast(const UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static UIterator FindLast(const UIterator Begin, const VIterator End, const T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindSecondLast(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindSecondLast(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindSecondLast(const ConstPointer String) const noexcept { return this->FindSecondLast(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindSecondLast(const ConstPointer String, const SizeType Length) const noexcept { return this->FindSecondLast(String, String + Length); }
    FORCEINLINE ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->FindSecondLast(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindSecondLast(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::FindSecondLast(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindSecondLast(const T Rune) const noexcept { return TStringBase::FindSecondLast(this->begin(), this->end(), Rune); }

    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::FindSecondLast(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecondLast(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecondLast(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindSecondLast(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::FindSecondLast(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::FindSecondLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecondLast(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecondLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::FindSecondLast(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindSecondLast(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::FindSecondLast(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecondLast(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::FindSecondLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecondLast(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecondLast(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecondLast(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::FindSecondLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
    FORCEINLINE static UIterator FindSecondLast(const UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept { return TStringBase::FindNth(Begin, End, OtherBegin, OtherEnd, 2); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static UIterator FindSecondLast(const UIterator Begin, const VIterator End, const T Rune) noexcept { return TStringBase::FindNth(Begin, End, Rune, 2); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindNthLast(const TStringBase<TEncoding, UAllocator>& Other, const SizeType N) const noexcept { return this->FindNthLast(Other.begin(), Other.end(), N); }
    FORCEINLINE ConstIterator FindNthLast(const ConstPointer String, const SizeType N) const noexcept { return this->FindNthLast(String, Encoding::GetStringLength(String), N); }
    FORCEINLINE ConstIterator FindNthLast(const ConstPointer String, const SizeType Length, const SizeType N) const noexcept { return this->FindNthLast(String, String + Length, N); }
    FORCEINLINE ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const SizeType N) const noexcept { return this->FindNthLast(ConstIterator{ Begin }, ConstIterator{ End }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindNthLast(const UIterator Begin, const VIterator End, const SizeType N) const noexcept { return TStringBase::FindNthLast(this->begin(), this->end(), Begin, End, N); }
    FORCEINLINE ConstIterator FindNthLast(const T Rune, const SizeType N) const noexcept { return TStringBase::FindNthLast(this->begin(), this->end(), Rune, N); }

    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, Other, Other + OtherLength, N); }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Begin + Length, OtherBegin, OtherEnd, N); }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const SizeType Length, const T Rune, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Begin + Length, Rune, N); }

    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, Other, Other + OtherLength), N; }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd), N; }
    FORCEINLINE static ConstIterator FindNthLast(const ConstPointer Begin, const ConstPointer End, const T Rune, const SizeType N) noexcept { return TStringBase::FindNthLast(ConstIterator{ Begin }, ConstIterator{ End }, Rune), N; }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNthLast(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNthLast(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, Other, Other + OtherLength), N; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNthLast(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static UIterator FindNthLast(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd, SizeType N) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static UIterator FindNthLast(UIterator Begin, const VIterator End, const T Rune, SizeType N) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType Count(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->Count(Other.begin(), Other.end()); }
    FORCEINLINE SizeType Count(const ConstPointer String) const noexcept { return this->Count(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE SizeType Count(const ConstPointer String, const SizeType Length) const noexcept { return this->Count(String, String + Length); }
    FORCEINLINE SizeType Count(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->Count(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE SizeType Count(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::Count(this->begin(), this->end(), Begin, End); }
    FORCEINLINE SizeType Count(const T Rune) const noexcept { return TStringBase::Count(this->begin(), this->end(), Rune); }

    FORCEINLINE static SizeType Count(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::Count(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Count(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Count(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::Count(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::Count(Begin, Begin + Length, Rune); }

    FORCEINLINE static SizeType Count(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::Count(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Count(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Count(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::Count(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static SizeType Count(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::Count(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::Count(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Count(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Count(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
                static SizeType Count(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
                static SizeType Count(UIterator Begin, const VIterator End, const T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool Contains(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->Contains(Other.begin(), Other.end()); }
    FORCEINLINE bool Contains(const ConstPointer String) const noexcept { return this->Contains(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE SizeType Contains(const ConstPointer String, const SizeType Length) const noexcept { return this->Contains(String, String + Length); }
    FORCEINLINE bool Contains(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->Contains(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool Contains(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::Contains(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool Contains(const T Rune) const noexcept { return TStringBase::Contains(this->begin(), this->end(), Rune); }

    FORCEINLINE static bool Contains(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::Contains(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Contains(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Contains(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::Contains(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::Contains(Begin, Begin + Length, Rune); }

    FORCEINLINE static bool Contains(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::Contains(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Contains(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Contains(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::Contains(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static bool Contains(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::Contains(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::Contains(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Contains(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::Contains(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept { return TStringBase::FindFirst(Begin, End, OtherBegin, OtherEnd) != End; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, const VIterator End, const T Rune) noexcept { return TStringBase::FindFirst(Begin, End, Rune) != End; }

    //#
    //# Searches the string backwards. These methods perform much worse than the forward searches with #Contains due
    //# to cache misses, failed vectorization, and wrong compiler prediction hints; and should, therefore, only be
    //# used when the item to find is expected to be at the end of the string.
    //# If you compare both forward and backward searches in performance in their worst case scenario, then the
    //# backward searches are expected to witness a 20% to 40% performance drop on larger arrays that do not fit in
    //# cache lines.
    //#
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool ContainsBackward(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->ContainsBackward(Other.begin(), Other.end()); }
    FORCEINLINE bool ContainsBackward(const ConstPointer String) const noexcept { return this->ContainsBackward(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE SizeType ContainsBackward(const ConstPointer String, const SizeType Length) const noexcept { return this->ContainsBackward(String, String + Length); }
    FORCEINLINE bool ContainsBackward(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->ContainsBackward(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE bool ContainsBackward(const UIterator Begin, const VIterator End) const noexcept { return TStringBase::ContainsBackward(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool ContainsBackward(const T Rune) const noexcept { return TStringBase::ContainsBackward(this->begin(), this->end(), Rune); }

    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const SizeType Length, const ConstPointer Other) noexcept { return TStringBase::ContainsBackward(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const SizeType Length, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::ContainsBackward(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const SizeType Length, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::ContainsBackward(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const SizeType Length, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::ContainsBackward(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const SizeType Length, const T Rune) noexcept { return TStringBase::ContainsBackward(Begin, Begin + Length, Rune); }

    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const ConstPointer End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Other + OtherLength); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const ConstPointer End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::ContainsBackward(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const ConstPointer End, const UIterator OtherBegin, const VIterator OtherEnd) noexcept { return TStringBase::ContainsBackward(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static bool ContainsBackward(const ConstPointer Begin, const ConstPointer End, const T Rune) noexcept { return TStringBase::ContainsBackward(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(const UIterator Begin, const VIterator End, const ConstPointer Other) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(const UIterator Begin, const VIterator End, const ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(const UIterator Begin, const VIterator End, const ConstPointer OtherBegin, const ConstPointer OtherEnd) noexcept { return TStringBase::ContainsBackward(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept { return TStringBase::FindLast(Begin, End, OtherBegin, OtherEnd) != End; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const VIterator End, const T Rune) noexcept { return TStringBase::FindLast(Begin, End, Rune) != End; }

    //# Cuts the content to the specified locations.
                void InlineCut(const SizeType Begin, const SizeType End, const bool bAllowShrinking = true) noexcept;
    FORCEINLINE void InlineCut(const ConstPointer Begin, const ConstPointer End, const bool bAllowShrinking = true) noexcept { this->InlineCut(Begin - this->begin_ptr(), End - this->begin_ptr(), bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void InlineCut(const UIterator Begin, const VIterator End, const bool bAllowShrinking = true) noexcept { this->InlineCut(Begin - this->begin(), End - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> Cut(const SizeType Begin, const SizeType End) const noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> Cut(const ConstPointer Begin, const ConstPointer End) const noexcept { return this->template Cut<UAllocator>(Begin - this->begin_ptr(), End - this->begin_ptr()); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator = Allocator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE TStringBase<TEncoding, UAllocator> Cut(const UIterator Begin, const VIterator End) const noexcept { return this->template Cut<UAllocator>(Begin - this->begin(), End - this->begin()); }

    //# Cuts the content at the specified exclusive location.
    FORCEINLINE void InlineLeftCut(const SizeType Index, const bool bAllowShrinking = true) noexcept;
    FORCEINLINE void InlineLeftCut(const ConstPointer Ptr, const bool bAllowShrinking = true) noexcept { this->InlineLeftCut(Ptr - this->begin_ptr(), bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void InlineLeftCut(const UIterator It, const bool bAllowShrinking = true) noexcept { this->InlineLeftCut(It - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftCut(const SizeType Index) const noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftCut(const ConstPointer Ptr) const noexcept { return this->template LeftCut<UAllocator>(Ptr - this->begin_ptr()); }
    template <TIteratorConcept UIterator, TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftCut(const UIterator It) const noexcept { return this->template LeftCut<UAllocator>(It - this->begin()); }

    //# Cuts the content at the specified inclusive location.
    FORCEINLINE void InlineRightCut(const SizeType Index, const bool bAllowShrinking = true) noexcept;
    FORCEINLINE void InlineRightCut(const ConstPointer Ptr, const bool bAllowShrinking = true) noexcept { this->InlineRightCut(Ptr - this->begin_ptr(), bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void InlineRightCut(const UIterator It, const bool bAllowShrinking = true) noexcept { this->InlineRightCut(It - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightCut(const SizeType Index) const noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightCut(const ConstPointer Ptr) const noexcept { return this->template RightCut<UAllocator>(Ptr - this->begin_ptr()); }
    template <TIteratorConcept UIterator, TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightCut(const UIterator It) const noexcept { return this->template RightCut<UAllocator>(It - this->begin()); }

    //# Chops #Count runes from the right.
    FORCEINLINE void InlineLeftChop(const SizeType Count, const bool bAllowShrinking = true) noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftChop(const SizeType Count) const noexcept;

    //# Chops #Count runes from the left.
    FORCEINLINE void InlineRightChop(const SizeType Count, const bool bAllowShrinking = true) noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightChop(const SizeType Count) const noexcept;

    FORCEINLINE void ToLower() noexcept requires(TAllocator::IsContentMutable());
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> GetLower() const noexcept;

    FORCEINLINE void ToUpper() noexcept requires(TAllocator::IsContentMutable());
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> GetUpper() const noexcept;

    FORCEINLINE SizeType GetLineNumber(const SizeType Index) const noexcept { return this->GetLineNumber(this->Impl.GetDataPointer() + Index); }
    FORCEINLINE SizeType GetLineNumber(const ConstPointer Ptr) const noexcept { return this->GetLineNumber(ConstIterator{ Ptr }); }
    FORCEINLINE SizeType GetLineNumber(const ConstIterator It) const noexcept;

    template <typename TPredicate> requires(std::invocable<TPredicate, T> && std::is_void_v<std::invoke_result_t<TPredicate, T>>)
    void ForEach(const TPredicate& Predicate) noexcept requires(TAllocator::IsContentMutable());
    template <std::predicate<T> TPredicate>
    void ForEach(const TPredicate& Predicate) const noexcept;

    NODISCARD FORCEINLINE constexpr ConstPointer Peek() noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase& Push(const TStringBase<TEncoding, UAllocator>& Other) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(Other.begin(), Other.end()); return *this; }
    FORCEINLINE TStringBase& Push(const ConstPointer String) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(String, String + Encoding::GetStringLength(String)); return *this; }
    FORCEINLINE TStringBase& Push(const ConstPointer String, const SizeType Length) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(String, String + Length); return *this; }
    FORCEINLINE TStringBase& Push(const ConstPointer Begin, const ConstPointer End) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(ConstIterator{ Begin }, ConstIterator{ End }); return *this; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE TStringBase& Push(const UIterator Begin, const VIterator End) & noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE TStringBase& Push(const T Rune) & noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(&Rune, &Rune + 1); return *this; }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase&& Push(const TStringBase<TEncoding, UAllocator>& Other) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(Other.begin(), Other.end()); return std::move(*this); }
    FORCEINLINE TStringBase&& Push(const ConstPointer String) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(String); return std::move(*this); }
    FORCEINLINE TStringBase&& Push(const ConstPointer String, const SizeType Length) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(String, Length); return std::move(*this); }
    FORCEINLINE TStringBase&& Push(const ConstPointer Begin, const ConstPointer End) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(ConstIterator{ Begin }, ConstIterator{ End }); return std::move(*this); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE TStringBase&& Push(const UIterator Begin, const VIterator End) && noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE TStringBase&& Push(const T Rune) && noexcept requires(Allocator::IsAllowedToPushItems()) { this->Push(&Rune, &Rune + 1); return std::move(*this); }

    //#
    //# Removes the last rune from this string.
    //# @return True if a rune was popped, false if the string was empty.
    //#
    FORCEINLINE bool Pop() noexcept requires(TStringBase::IsStringView());
    FORCEINLINE bool Pop() noexcept requires(!TStringBase::IsStringView());
    //# Pops #Count runes from the end of this string.
    FORCEINLINE void Pop(SizeType Count) noexcept requires(TStringBase::IsStringView());
    FORCEINLINE void Pop(SizeType Count) noexcept requires(!TStringBase::IsStringView());

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

    template <typename... TArgs>
    FORCEINLINE static TStringBase SprintF(const ConstPointer Format, const TArgs&... Args) noexcept requires(TStringBase::IsOwningString());

    FORCEINLINE const Allocator& GetAllocator() const noexcept { return this->Impl; }
    FORCEINLINE Allocator& GetMutableAllocator() noexcept { return this->Impl; }

private:

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(IsValidIteratorPair<UIterator, VIterator>)
    FORCEINLINE void AssignImpl(const UIterator Begin, const VIterator End) noexcept requires(!TStringBase::IsStringView())
    {
        check( Begin <= End )

        const SizeType Length { static_cast<SizeType>(End - Begin) };

        this->Reset(Length);
        if (Length > 0)
        {
            this->Impl.Append(Begin, End);
            this->Impl.Push(Encoding::Terminator);
        }

        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        return;
    }

    void CreateInvariant() noexcept
    {
        if constexpr (TStringBase::IsStringView())
        {
            checkCode
            (
                if (this->Impl.IsEmpty() == false)
                {
                    if (*this->Impl.Peek() == Encoding::Terminator)
                    {
                        LOG_FATAL(LogLowLevel, "String view is c style terminated, but should not be.")
                    }
                }
            )
        }
        else
        {
            if (this->Impl.IsEmpty() == false)
            {
                if (*this->Impl.Peek() != Encoding::Terminator)
                {
                    this->Impl.PushMinimalGrowth(Encoding::Terminator);
                }
            }
        }

        return;
    }

    void CreateInvariantWeak() noexcept
    {
        if constexpr (TStringBase::IsStringView())
        {
            if (this->Impl.IsEmpty() == false)
            {
                if (*this->Impl.Peek() == Encoding::Terminator)
                {
                    this->Impl.Pop();
                }
            }
        }
        else
        {
            if (this->Impl.IsEmpty() == false)
            {
                if (*this->Impl.Peek() != Encoding::Terminator)
                {
                    this->Impl.PushMinimalGrowth(Encoding::Terminator);
                }
            }
        }

        return;
    }

#if LAL_CHECK_STRING_VALIDITY
    void EnsureInvariant() const
    {
        for (const T *RESTRICT Bulk { this->Impl.GetDataPointer() }, *RESTRICT End { this->Impl.GetSlackPointer() }; Bulk != End; ++Bulk)
        {
            if constexpr (TStringBase::IsStringView())
            {
                if (*Bulk == Encoding::Terminator)
                {
                    LOG_FATAL(LogLowLevel, "String view contains c style null terminator.")
                }
            }
            else /* !Self::IsStringView() */
            {
                if (*Bulk == Encoding::Terminator && Bulk + 1 != End)
                {
                    LOG_FATAL(LogLowLevel, "Owning string contains c style null terminator in the middle of the predicted string.")
                }
            }
        }

        if constexpr (TStringBase::IsOwningString())
        {
            if (this->Impl.IsEmpty() == false)
            {
                if (*this->Impl.Peek() != Encoding::Terminator)
                {
                    LOG_FATAL(LogLowLevel, "Owning string is not c style terminated.")
                }
            }
        }

        return;
    }
#endif /* LAL_CHECK_STRING_VALIDITY */

    Allocator Impl;
};

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::GetRuneCount() const noexcept
    requires(TStringBase::IsStringView())
{
    return this->Impl.GetSize();
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::GetRuneCount() const noexcept
    requires(!TStringBase::IsStringView())
{
    const SizeType Size { this->Impl.GetSize() };
    if (Size == 0)
    {
        return 0;
    }

    return Size - 1;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const TStringBase& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const TAllocator&>)
    : Impl{Other.Impl}
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const TStringBase& Other) noexcept
    requires(std::assignable_from<TAllocator&, const TAllocator&>)
{
    this->Impl = Other.Impl;
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(TStringBase&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, TAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(TStringBase&& Other) noexcept
    requires(std::assignable_from<TAllocator&, TAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const TStringBase<TEncoding, UAllocator>& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const UAllocator&>)
    : Impl{Other.Impl}
{
    this->CreateInvariantWeak();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const TStringBase<TEncoding, UAllocator>& Other) noexcept
    requires(std::assignable_from<TAllocator&, const UAllocator&>)
{
    this->Impl = Other.Impl;
    this->CreateInvariantWeak();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T,typename UAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(TStringBase<TEncoding, UAllocator>&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, UAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(TStringBase<TEncoding, UAllocator>&& Other) noexcept
    requires(std::assignable_from<TAllocator&, UAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, VIterator>)
    : Impl{ Begin, End }
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator>
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const UIterator Begin, const SizeType Length) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, UIterator>)
    : Impl{ Begin, Begin + Length }
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const Pointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, typename TAllocator::Pointer, typename TAllocator::Pointer>)
    : Impl{ String, String + Encoding::GetStringLength(String) }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const Pointer String) noexcept
    requires(!TStringBase::
    IsStringView() && std::is_constructible_v<TAllocator, typename TAllocator::Pointer, typename TAllocator::Pointer>)
    : Impl{ String, String + Encoding::GetStringLength(String) + /*Terminator*/1 }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const Pointer Begin, const Pointer End) noexcept
    requires(std::is_constructible_v<TAllocator, typename TAllocator::Pointer, typename TAllocator::Pointer>)
    : Impl{ Begin, End }
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const Pointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, typename TAllocator::Pointer, typename TAllocator::Pointer>)
{
    const SizeType Length { Encoding::GetStringLength(String) };
    this->Impl = Allocator{ String, String + Length };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const Pointer String) noexcept
    requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); })
{
    const SizeType Length { Encoding::GetStringLength(String) };

    this->Reset(Length);
    this->Impl.Append(String, String + Length + /*Terminator*/1);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const ConstPointer String) noexcept
    requires
    (
           TStringBase::IsStringView()
        && std::is_constructible_v<TAllocator, typename TAllocator::ConstPointer, typename TAllocator::ConstPointer>
        && std::is_same_v<typename TAllocator::Pointer, typename TAllocator::ConstPointer> == false
    )
    : Impl{ String, String + Encoding::GetStringLength(String) }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const ConstPointer String) noexcept
    requires
    (
           !TStringBase::IsStringView()
        && std::is_constructible_v<TAllocator, typename TAllocator::ConstPointer, typename TAllocator::ConstPointer>
        && std::is_same_v<typename TAllocator::Pointer, typename TAllocator::ConstPointer> == false
    )
    : Impl{ String, String + Encoding::GetStringLength(String) + /*Terminator*/1 }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const ConstPointer Begin, const ConstPointer End) noexcept
    requires
    (
           std::is_constructible_v<TAllocator, typename TAllocator::ConstPointer, typename TAllocator::ConstPointer>
        && std::is_same_v<typename TAllocator::Pointer, typename TAllocator::ConstPointer> == false
    )
    : Impl{ Begin, End }
{
    if constexpr (TStringBase::IsOwningString())
    {
        this->CreateInvariant();
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const ConstPointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, typename TAllocator::ConstPointer, typename TAllocator::ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false)
{
    const SizeType Length { Encoding::GetStringLength(String) };
    this->Impl = Allocator{ String, String + Length };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr void TStringBase<TEncoding, TAllocator>::Reset(const SizeType Count, const bool bAllowShrinking) noexcept
    requires(TAllocator::IsStronglyAllocated())
{
    if (Count > 0)
    {
        this->Impl.Reset(Count + /*Terminator*/1, bAllowShrinking);
    }
    else
    {
        this->Impl.Reset(Count, bAllowShrinking);
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr void TStringBase<TEncoding, TAllocator>::Resize(const SizeType Count) noexcept
    requires(TAllocator::IsStronglyAllocated())
{
    if (Count > 0)
    {
        this->Impl.Resize(Count + /*Terminator*/1);
    }
    else
    {
        this->Impl.Resize(Count);
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
{
    return std::equal(this->begin(), this->end(), Other.begin(), Other.end());
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
    template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator!=(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
{
    return !(*this == Other);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(const ConstPointer String) const noexcept
{
    const SizeType Length { Encoding::GetStringLength(String) };

    if (this->GetRuneCount() != Length)
    {
        return false;
    }

    return std::memcmp(this->begin_ptr(), String, Length) == 0;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator!=(const ConstPointer String) const noexcept
{
    return !(*this == String);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(const T Rune) const noexcept
{
    return this->GetRuneCount() <= 1 && *this->Peek() == Rune;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::Equals(const ConstPointer Begin, const ConstPointer End) const noexcept
{
    return (End - Begin) == this->GetRuneCount() && std::memcmp(this->begin_ptr(), Begin, End - Begin) == 0;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::Equals(UIterator Begin, const VIterator End) const noexcept
{
    check( Begin <= End )

    const SizeType RuneCount { this->GetRuneCount() };

    if (RuneCount != static_cast<SizeType>(End - Begin))
    {
        return false;
    }

    if (RuneCount == 0)
    {
        return true;
    }

    ConstIterator It { this->begin() };
    for (; It != this->end() && Begin != End; ++It, ++Begin)
    {
        if (*It != *Begin)
        {
            return false;
        }

        continue;
    }

    return It == this->end() && Begin == End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr std::strong_ordering TStringBase<TEncoding, TAllocator>::SpaceShip(const T Rune) const noexcept
{
    const SizeType RuneCount { this->GetRuneCount() };

    if (RuneCount == 0)
    {
        return Rune == Encoding::Terminator ? std::strong_ordering::equal : std::strong_ordering::less;
    }

    T LhsLow { Encoding::GetLowercaseRune(*this->begin_ptr()) };
    T RhsLow { Encoding::GetLowercaseRune(Rune) };

    if (LhsLow < RhsLow)
    {
        return std::strong_ordering::less;
    }

    if (LhsLow > RhsLow)
    {
        return std::strong_ordering::greater;
    }

    if (*this->begin_ptr() < Rune)
    {
        return std::strong_ordering::less;
    }

    if (*this->begin_ptr() > Rune)
    {
        return std::strong_ordering::greater;
    }

    if (RuneCount == 1)
    {
        return std::strong_ordering::equal;
    }

    return std::strong_ordering::greater;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
constexpr std::strong_ordering TStringBase<TEncoding, TAllocator>::SpaceShip(const UIterator Begin, const VIterator End) const noexcept
{
    SizeType Size1 { this->Impl.GetSlackPointer() - this->Impl.GetDataPointer() };
    SizeType Size2 { End - Begin };
    auto MinSize { Jafg::Maths::Min(Size1, Size2) };

    struct LMyComp final
    {
        FORCEINLINE std::strong_ordering operator()(T Lhs, T Rhs) const noexcept
        {
            T LhsLow { Encoding::GetLowercaseRune(Lhs) };
            T RhsLow { Encoding::GetLowercaseRune(Rhs) };

            if (LhsLow < RhsLow)
            {
                return std::strong_ordering::less;
            }
            if (LhsLow > RhsLow)
            {
                return std::strong_ordering::greater;
            }

            if (Lhs < Rhs)
            {
                return std::strong_ordering::less;
            }
            if (Lhs > Rhs)
            {
                return std::strong_ordering::greater;
            }

            return std::strong_ordering::equal;
        }
    };

    auto Result
    {
        std::lexicographical_compare_three_way
        (
            this->Impl.GetDataPointer(), this->Impl.GetDataPointer() + MinSize,
            Begin.Cursor, End.Cursor + MinSize,
            LMyComp{}
        )
    };

    if (Result != 0)
    {
        return Result;
    }

    return Size1 <=> Size2;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Append(const UIterator Begin, const VIterator End) noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( Begin <= End )

    if (const SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        this->Impl.Pop(); /* Terminator */
        this->ReserveAdditionally(static_cast<SizeType>(Length) + /*Terminator*/1);
        this->Impl.Append(Begin, End);
        this->Impl.Push(Encoding::Terminator);
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::Iterator TStringBase<TEncoding, TAllocator>::AppendAt(const SizeType Index, const UIterator Begin, const VIterator End) noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( this->IsValidIndex(Index) || this->GetRuneCount() == Index )
    check( Begin <= End )

    if (const SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        this->Impl.Pop(); /* Terminator */
        this->ReserveAdditionally(static_cast<SizeType>(Length) + /*Terminator*/1);
        auto Out { this->Impl.AppendAt(Index, Begin, End) };
        this->Impl.Push(Encoding::Terminator);
        PRIVATE_LAL_ENSURE_STRING_INVARIANT()

        return Out;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return this->end();
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator>
    requires(IsValidIteratorPair<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::AppendToNew(const UIterator Begin, const VIterator End) const noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( Begin <= End )

    if (const SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        TStringBase<TEncoding, UAllocator> Out;
        Out.Reserve(this->GetRuneCount() + static_cast<SizeType>(Length));

        Out.Append(this->begin(), this->end());
        Out.Append(Begin, End);

        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(Out)
        return Out;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return TStringBase<TEncoding, UAllocator>{ this->begin(), this->end() };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::AppendPath(UIterator Begin, const VIterator End) noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( Begin <= End )

    if (SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        this->Impl.Pop(); /* Terminator */

        if (const ConstPointer Last { this->Impl.Peek() }; Last)
        {
            if (*Last == '/')
            {
                if (*Begin == '/')
                {
                    ++Begin;
                    --Length;

                    if (LAL_UNLIKELY(Length == 0))
                    {
                        this->Impl.Push(Encoding::Terminator);
                        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
                        return;
                    }
                }

                this->ReserveAdditionally(static_cast<SizeType>(Length) + /*Terminator*/1);
            }
            else
            {
                if (*Begin == '/')
                {
                    this->ReserveAdditionally(static_cast<SizeType>(Length) + /*Terminator*/1);
                }
                else
                {
                    this->ReserveAdditionally(static_cast<SizeType>(Length) + /*Terminator*/1 + /*Leading Slash*/1);
                    this->Impl.Push('/');
                }
            }
        }
        else
        {
            check( this->GetRuneCount() == 0 )
        }

        this->Impl.Append(Begin, End);
        this->Impl.Push(Encoding::Terminator);
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator>
    requires(IsValidIteratorPair<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::AppendPathToNew(const UIterator Begin, const VIterator End) const noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( Begin <= End )

    if (const SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        TStringBase<TEncoding, UAllocator> Out;
        Out.Reserve(this->GetRuneCount() + static_cast<SizeType>(Length) +
            /*Slash*/1 /* This might be one character to many, but we don't really give a shit. */
            );
        Out.Append(this->begin(), this->end());
        Out.AppendPath(Begin, End);

        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(Out)
        return Out;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return TStringBase<TEncoding, UAllocator>{ this->begin(), this->end() };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::RemoveAt(const SizeType Begin, const SizeType End) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    check( this->IsValidIndex(Begin) && Begin <= End && (Begin == End || this->IsValidIndex(End - 1)) )
    this->Impl.RemoveAt(Begin, End);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Remove(const UIterator Begin, const VIterator End, const bool bAllowShrinking /* = true */) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Length { static_cast<SizeType>(End - Begin) };
    if (LAL_UNLIKELY(Length == 0))
    {
        return 0;
    }
    if (Length == 1)
    {
        return this->Remove(*Begin);
    }

    SizeType Out { 0 };

    ConstPointer Cursor { this->begin_ptr() };
    while (Cursor != this->end_ptr())
    {
        const ConstIterator It { TStringBase::FindFirst(Cursor, this->end_ptr(), Begin, End) };
        if (It == this->end())
        {
            break;
        }

        this->RemoveAt(&*It, End - Begin);

        Cursor = &*It;

        ++Out;
        continue;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Remove(const T Rune, const bool bAllowShrinking /* = false */) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    check( Rune != Encoding::Terminator )
    const SizeType Out { this->Impl.Remove(Rune) };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnce(const UIterator Begin, const VIterator End) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Length { static_cast<SizeType>(End - Begin) };
    if (LAL_UNLIKELY(Length == 0))
    {
        return false;
    }
    if (Length == 1)
    {
        return this->RemoveOnce(*Begin);
    }

    const ConstIterator It { TStringBase::FindFirst(this->begin(), this->end(), Begin, End) };
    if (It == this->end())
    {
        return false;
    }

    this->RemoveAt(&*It, End - Begin);

    return true;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnce(const T Rune) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    check( Rune != Encoding::Terminator )
    const SizeType Out { this->Impl.RemoveOnce(Rune) };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Replace(const T Rune, const T Replacement) noexcept
    requires(Allocator::IsContentMutable())
{
    check( Rune != Encoding::Terminator )
    check( Replacement != Encoding::Terminator )

    const SizeType Out { this->Impl.Replace(Rune, Replacement) };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Replace(const UIterator Begin, const VIterator End, WIterator ReplacementBegin, const XIterator ReplacementEnd, const bool bAllowShrinking /* = true */) noexcept
    requires(TStringBase::IsOwningString() && Allocator::IsContentMutable())
{
    check( this->IsValidIterator(Begin) == false && this->IsValidEndIterator(End) == false && Begin <= End )
    check( this->IsValidIterator(ReplacementBegin) == false && this->IsValidEndIterator(ReplacementEnd) == false && ReplacementBegin <= ReplacementEnd )

    const SizeType Length { static_cast<SizeType>(End - Begin) };
    const SizeType ReplacementLength { static_cast<SizeType>(ReplacementEnd - ReplacementBegin) };

    SizeType Out { 0 };

    Pointer Cursor { this->begin_ptr() };
    while (Cursor != this->end_ptr())
    {
        const ConstIterator It { TStringBase::FindFirst(Cursor, this->end_ptr(), Begin, End) };
        if (It == this->end())
        {
            break;
        }

        const SizeType Index { static_cast<SizeType>(It - this->begin()) };

        this->Substitute(this->begin() + Index, this->begin() + Index + Length, ReplacementBegin, ReplacementEnd, false);

        Cursor = this->begin_ptr() + Index + ReplacementLength;

        ++Out;
        continue;
    }

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
void TStringBase<TEncoding, TAllocator>::Substitute(UIterator Begin, VIterator End, const WIterator ReplacementBegin, const XIterator ReplacementEnd, const bool bAllowShrinking /* = true */) noexcept
    requires(TStringBase::IsOwningString() && Allocator::IsContentMutable())
{
    check( this->IsValidIterator(Begin) && this->IsValidEndIterator(End) && Begin <= End )
    check( ReplacementBegin <= ReplacementEnd )

    const i64 Delta { static_cast<i64>(ReplacementEnd - ReplacementBegin) - static_cast<i64>(End - Begin) };
    if (Delta == 0)
    {
        std::memcpy(Begin.Data(), ReplacementBegin.Data(), ReplacementEnd - ReplacementBegin);
        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        return;
    }

    const SizeType BeginIdx { static_cast<SizeType>(Begin - this->begin()) };
    const SizeType EndIdx { static_cast<SizeType>(End - this->begin()) };

    if (Delta > 0)
    {
        this->Impl.ReserveAdditionally(static_cast<SizeType>(Delta));
    }

    std::memmove(this->begin_ptr() + EndIdx + Delta, this->begin_ptr() + EndIdx, this->Impl.GetSize() - EndIdx);
    std::memcpy(this->begin_ptr() + BeginIdx, ReplacementBegin.Data(), ReplacementEnd - ReplacementBegin);

    this->Impl.GetMutableAllocator().Slack += Delta;

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
bool TStringBase<TEncoding, TAllocator>::StartsWith(UIterator Begin, const VIterator End, WIterator OtherBegin, const XIterator OtherEnd) noexcept
{
    while (Begin != End && OtherBegin != OtherEnd)
    {
        check( *Begin != Encoding::Terminator )
        check( *OtherBegin != Encoding::Terminator )

        if (*Begin != *OtherBegin)
        {
            return false;
        }

        ++Begin;
        ++OtherBegin;

        continue;
    }

    return OtherBegin == OtherEnd;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
bool TStringBase<TEncoding, TAllocator>::EndsWith(const UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept
{
    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    if (OtherLength > static_cast<SizeType>(End - Begin))
    {
        return false;
    }

    return TStringBase::StartsWith(End - OtherLength, End, OtherBegin, OtherEnd);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
bool TStringBase<TEncoding, TAllocator>::EndsWith(const UIterator Begin, const VIterator End, const T Rune) noexcept
{
    check( Begin <= End )

    if (Begin == End)
    {
        return false;
    }

    return *(End - 1) == Rune;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindFirst(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };
    while (Begin != End)
    {
        check( *Begin != Encoding::Terminator )

        if (static_cast<SizeType>(End - Begin) < OtherLength)
        {
            return End;
        }

        WIterator Cursor { OtherBegin };
        while (Cursor != OtherEnd)
        {
            check( *Cursor != Encoding::Terminator )

            if (*(Begin + (Cursor - OtherBegin)) != *Cursor)
            {
                break;
            }

            ++Cursor;
            continue;
        }

        if (Cursor == OtherEnd)
        {
            check( Begin < End )
            return Begin;
        }

        ++Begin;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE UIterator TStringBase<TEncoding, TAllocator>::FindFirst(UIterator Begin, const VIterator End, const T Rune) noexcept
{
    check( Begin <= End )

    while (Begin != End)
    {
        check( *Begin != Encoding::Terminator )

        if (*Begin == Rune)
        {
            return Begin;
        }

        ++Begin;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNth(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd, SizeType N) noexcept
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };
    while (Begin != End && N > 0)
    {
        check( *Begin != Encoding::Terminator )

        if (static_cast<SizeType>(End - Begin) < OtherLength)
        {
            return End;
        }

        WIterator Cursor { OtherBegin };
        while (Cursor != OtherEnd)
        {
            check( *Cursor != Encoding::Terminator )

            if (*(Begin + (Cursor - OtherBegin)) != *Cursor)
            {
                break;
            }

            ++Cursor;
            continue;
        }

        if (Cursor == OtherEnd)
        {
            if (--N == 0)
            {
                check( Begin < End )
                return Begin;
            }
        }

        ++Begin;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNth(UIterator Begin, const VIterator End, const T Rune, SizeType N) noexcept
{
    check( Begin <= End )

    while (Begin != End && N > 0)
    {
        check( *Begin != Encoding::Terminator )

        if (*Begin == Rune)
        {
            if (--N == 0)
            {
                check( Begin < End )
                return Begin;
            }
        }

        ++Begin;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindLast(const UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    VIterator Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (static_cast<SizeType>(End - Cursor) < OtherLength)
        {
            --Cursor;
            continue;
        }

        WIterator InnerCursor { OtherBegin };
        while (InnerCursor != OtherEnd)
        {
            check( *InnerCursor != Encoding::Terminator )

            if (*(Cursor + (InnerCursor - OtherBegin)) != *InnerCursor)
            {
                break;
            }

            ++InnerCursor;
            continue;
        }

        if (InnerCursor == OtherEnd)
        {
            check( Cursor < End )
            return Cursor;
        }

        --Cursor;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindLast(const UIterator Begin, const VIterator End, const T Rune) noexcept
{
    check( Begin <= End )

    VIterator Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (*Cursor == Rune)
        {
            check( Cursor < End )
            return Cursor;
        }

        --Cursor;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNthLast(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd, SizeType N) noexcept
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    VIterator Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (static_cast<SizeType>(End - Cursor) < OtherLength)
        {
            --Cursor;
            continue;
        }

        WIterator InnerCursor { OtherBegin };
        while (InnerCursor != OtherEnd)
        {
            check( *InnerCursor != Encoding::Terminator )

            if (*(Cursor + (InnerCursor - OtherBegin)) != *InnerCursor)
            {
                break;
            }

            ++InnerCursor;
            continue;
        }

        if (InnerCursor == OtherEnd)
        {
            if (--N == 0)
            {
                check( Cursor < End )
                return Cursor;
            }
        }

        --Cursor;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNthLast(UIterator Begin, const VIterator End, const T Rune, SizeType N) noexcept
{
    check( Begin <= End )

    VIterator Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (*Cursor == Rune)
        {
            if (--N == 0)
            {
                check( Cursor < End )
                return Cursor;
            }
        }

        --Cursor;
        continue;
    }

    return End;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(IsValidIteratorPair<UIterator, VIterator> && IsValidIteratorPair<WIterator, XIterator> && IsValidCrossIteratorPair<UIterator, WIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Count(UIterator Begin, const VIterator End, const WIterator OtherBegin, const XIterator OtherEnd) noexcept
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )

    SizeType Out { 0 };

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };
    while (Begin != End)
    {
        check( *Begin != Encoding::Terminator )

        if (static_cast<SizeType>(End - Begin) < OtherLength)
        {
            break;
        }

        WIterator Cursor { OtherBegin };
        while (Cursor != OtherEnd)
        {
            check( *Cursor != Encoding::Terminator )

            if (*(Begin + (Cursor - OtherBegin)) != *Cursor)
            {
                break;
            }

            ++Cursor;
            continue;
        }

        if (Cursor == OtherEnd)
        {
            ++Out;
            Begin += OtherLength;
            check( Begin <= End )
        }
        else
        {
            ++Begin;
        }

        continue;
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Count(UIterator Begin, const VIterator End, const T Rune) noexcept
{
    check( Begin <= End )

    SizeType Out { 0 };

    while (Begin != End)
    {
        check( *Begin != Encoding::Terminator )

        if (*Begin == Rune)
        {
            ++Out;
        }

        ++Begin;
        continue;
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
void TStringBase<TEncoding, TAllocator>::InlineCut(const SizeType Begin, const SizeType End, const bool bAllowShrinking /* = true */) noexcept
{
    check( Begin <= End )

    if constexpr (!TStringBase::IsStringView())
    {
        this->Impl.Pop(); /* Terminator */
    }

    this->Impl.Drop(Begin);
    check( this->Impl.GetSize() >= End - Begin )
    this->Impl.Pop(this->Impl.GetSize() - (End - Begin));

    if constexpr (!TStringBase::IsStringView())
    {
        if (this->Impl.GetSize() > 0)
        {
            this->Impl.Push(Encoding::Terminator);
        }
        else
        {
            this->Impl.Empty();
        }
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::Cut(const SizeType Begin, const SizeType End) const noexcept
{
    check( Begin <= End )
    return TStringBase<TEncoding, UAllocator>{ this->begin() + Begin, this->begin() + End };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineLeftCut(const SizeType Index, const bool bAllowShrinking /* = true */) noexcept
{
    this->InlineCut(0ul, Index, bAllowShrinking);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::LeftCut(const SizeType Index) const noexcept
{
    return this->template Cut<UAllocator>(0ul, Index);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineRightCut(const SizeType Index, const bool bAllowShrinking /* = true */) noexcept
{
    this->InlineCut(Index, this->end_idx(), bAllowShrinking);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::RightCut(const SizeType Index) const noexcept
{
    return this->template Cut<UAllocator>(Index, this->end_idx());
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineLeftChop(const SizeType Count, const bool bAllowShrinking /* = true */) noexcept
{
    if constexpr (!TStringBase::IsStringView())
    {
        this->Impl.Pop(); /* Terminator */
    }

    this->Impl.Pop(Count);

    if constexpr (!TStringBase::IsStringView())
    {
        if (this->Impl.GetSize() > 0)
        {
            this->Impl.Push(Encoding::Terminator);
        }
        else
        {
            this->Impl.Empty();
        }
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::LeftChop(const SizeType Count) const noexcept
{
    check( Count <= this->GetRuneCount() )
    return TStringBase<TEncoding, UAllocator>{ this->begin(), this->end() - Count };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineRightChop(const SizeType Count, const bool bAllowShrinking /* = true */) noexcept
{
    this->Impl.Drop(Count);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::RightChop(const SizeType Count) const noexcept
{
    check( Count <= this->GetRuneCount() )
    return TStringBase<TEncoding, UAllocator>{ this->begin() + Count, this->end() };
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::ToLower() noexcept
    requires(TAllocator::IsContentMutable())
{
    for (T& Rune : *this)
    {
        Rune = Encoding::GetLowercaseRune(Rune);
        continue;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::GetLower() const noexcept
{
    TStringBase<TEncoding, UAllocator> Out;
    Out.Reserve(this->GetRuneCount());

    for (const T& Rune : *this)
    {
        Out.GetMutableAllocator().Push(Encoding::GetLowercaseRune(Rune));
        continue;
    }

    Out.GetMutableAllocator().Push(Encoding::Terminator);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(Out)

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::ToUpper() noexcept
    requires(TAllocator::IsContentMutable())
{
    for (T& Rune : *this)
    {
        Rune = Encoding::GetUppercaseRune(Rune);
        continue;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
    requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::GetUpper() const noexcept
{
    TStringBase<TEncoding, UAllocator> Out;
    Out.Reserve(this->GetRuneCount());

    for (const T& Rune : *this)
    {
        Out.GetMutableAllocator().Push(Encoding::GetUppercaseRune(Rune));
        continue;
    }

    Out.GetMutableAllocator().Push(Encoding::Terminator);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(Out)

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::GetLineNumber(const ConstIterator It) const noexcept
{
    SizeType Out;

    for (ConstIterator Cursor { this->begin() }; Cursor != It; ++Cursor)
    {
        check( *Cursor != Encoding::Terminator )

        if (*Cursor == '\n')
        {
            ++Out;
        }

        continue;
    }

    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <typename TPredicate>
    requires(std::invocable<TPredicate, typename TAllocator::T> && std::is_void_v<std::invoke_result_t<TPredicate, typename TAllocator::T>>)
void TStringBase<TEncoding, TAllocator>::ForEach(const TPredicate& Predicate) noexcept
    requires(TAllocator::IsContentMutable())
{
    for (const Reference Rune : *this)
    {
        check( Rune != Encoding::Terminator )
        Predicate(Rune);

        continue;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <std::predicate<typename TAllocator::T> TPredicate>
void TStringBase<TEncoding, TAllocator>::ForEach(const TPredicate& Predicate) const noexcept
{
    for (const ConstReference Rune : *this)
    {
        check( Rune != Encoding::Terminator )
        Predicate(Rune);
        continue;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::Push(const UIterator Begin, const VIterator End) & noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    this->Append(Begin, End);
    return *this;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(IsValidIteratorPair<UIterator, VIterator>)
FORCEINLINE TStringBase<TEncoding, TAllocator>&& TStringBase<TEncoding, TAllocator>::Push(const UIterator Begin, const VIterator End) && noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    this->Append(Begin, End);
    return std::move(*this);
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
bool TStringBase<TEncoding, TAllocator>::Pop() noexcept
    requires(TStringBase::IsStringView())
{
    const bool bOut { this->Impl.Pop() };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return bOut;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
bool TStringBase<TEncoding, TAllocator>::Pop() noexcept
    requires(!TStringBase::IsStringView())
{
    const bool bEmpty { this->IsEmpty() };

    this->Impl.Pop(1 + 1/* Terminator */);

    if (this->Impl.GetSize() > 0)
    {
        this->Impl.Push(Encoding::Terminator);
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return bEmpty == false;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Pop(const SizeType Count) noexcept
    requires(TStringBase::IsStringView())
{
    this->Impl.Pop(Count);
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Pop(const SizeType Count) noexcept
    requires(!TStringBase::IsStringView())
{
    this->Impl.Pop(Count + 1/* Terminator */);

    if (this->Impl.GetSize() > 0)
    {
        this->Impl.Push(Encoding::Terminator);
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::Drop() noexcept
{
    const bool bOut { this->Impl.Drop() };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return bOut;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Drop(const SizeType Count) noexcept
{
    SizeType Out { this->Impl.Drop(Count) };
    if constexpr (TStringBase::IsOwningString())
    {
        if (this->Impl.GetSize() == 0 && Out > 0)
        {
            /* Terminator */
            --Out;
        }
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return Out;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::ConstPointer TStringBase<TEncoding, TAllocator>::Peek() noexcept
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    if (this->Impl.GetSize() > 0)
    {
        return this->Impl.GetDataPointer() + this->GetRuneCount();
    }

    return &Encoding::Terminator;
}

template <template <typename, typename> class TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <typename... TArgs>
FORCEINLINE TStringBase<TEncoding, TAllocator> TStringBase<TEncoding, TAllocator>::SprintF(const ConstPointer Format, const TArgs&... Args) noexcept
    requires(TStringBase::IsOwningString())
{
    //
    // Super stupid solution. But who cares right now?
    // Later we write our own implementation with type safety etc.
    //
    // S will be stack-allocated if the str is small enough. So it's not that bad.
    // But still one unnecessary heap allocation by this class...
    //
    const LStringLegacy S { std::vformat(Format, std::make_format_args(Args...)) };
    return { S.begin().base(), S.end().base() };
}

template <template <typename, typename> typename TEncoding, typename T>
using THeapString = TStringBase<TEncoding, TArray<T>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TStringCapacity>
using TSmallString = TStringBase<TEncoding, TStackArray<T, TStringCapacity>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TStringCapacity>
using TOptimizedString = TStringBase<TEncoding, TStackOptimizedArray<T, TStringCapacity>>;

template <template <typename, typename> typename TEncoding, typename T>
using TStringView = TStringBase<TEncoding, TArrayView<T>>;
template <template <typename, typename> typename TEncoding, typename T>
using TMutableStringView = TStringBase<TEncoding, TMutableArrayView<T>>;

template <typename T>
using THeapStringUtf8 = THeapString<TStringBaseDefaultUtf8Traits, T>;
template <typename T, LSize TStringCapacity>
using TSmallStringUtf8 = TSmallString<TStringBaseDefaultUtf8Traits, T, TStringCapacity>;
template <typename T, LSize TStringCapacity>
using TOptimizedStringUtf8 = TOptimizedString<TStringBaseDefaultUtf8Traits, T, TStringCapacity>;

template <typename T>
using TStringViewUtf8 = TStringView<TStringBaseDefaultUtf8Traits, T>;
template <typename T>
using TMutableStringViewUtf8 = TMutableStringView<TStringBaseDefaultUtf8Traits, T>;

} /* Namespace Lal */

//#
//# The default string for Lal with UTF-8 encoding.
//# This string is a good all-rounder. It is optimized for small strings but uses different allocation strategies
//# when the string gets larger.
//#
typedef Lal::TOptimizedStringUtf8<LJafgChar, 8> LString;

//#
//# This string is optimized for large strings.
//#
typedef Lal::THeapStringUtf8<LJafgChar> LBigString;

//#
//# This string is optimized for small strings. This string uses stack allocation and is meant if the string's max
//# sizes are well-defined and more or less small.
//#
//# @tparam The capacity to reserve on the stack. This is the maximum size of the string.
//#
template <LSize TCapacity>
using LSmallString = Lal::TSmallStringUtf8<LJafgChar, TCapacity>;

//#
//# The default string view for Lal with UTF-8 encoding.
//#
typedef Lal::TStringViewUtf8<LJafgChar> LStringView;
typedef Lal::TMutableStringViewUtf8<LJafgChar> LMutableStringView;

//#
//# Guaranteed to be the same size as std::string.
//#
static_assert(sizeof(std::string) == sizeof(LString));
