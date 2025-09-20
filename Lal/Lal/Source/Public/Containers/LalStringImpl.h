// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const TStringBase& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const TAllocator&>)
    : Impl{Other.Impl}
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const TStringBase& Other) noexcept
    requires(Lal::AssignableFrom<TAllocator&, const TAllocator&>)
{
    this->Impl = Other.Impl;
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(TStringBase&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, TAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(TStringBase&& Other) noexcept
    requires(Lal::AssignableFrom<TAllocator&, TAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TDerived>
    requires(std::is_same_v<typename TDerived::Encoding, typename TStringBase<TEncoding, TAllocator>::Encoding>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const TDerived& Other) noexcept
    requires
           (std::is_constructible_v<TAllocator, const typename TDerived::Allocator&>
        && !(std::is_same_v<TStringBase, typename TDerived::_WeakRepr> || std::is_same_v<TStringBase, typename TDerived::_WeakMutableRepr>)
        )
    : Impl{Other.Impl}
{
    this->CreateInvariantWeak();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TDerived>
    requires(std::is_same_v<typename TDerived::Encoding, typename TStringBase<TEncoding, TAllocator>::Encoding>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(const TDerived& Other) noexcept
    requires(
           std::is_constructible_v<Allocator, const typename TDerived::Allocator&>
        && (std::is_same_v<TStringBase, typename TDerived::_WeakRepr> || std::is_same_v<TStringBase, typename TDerived::_WeakMutableRepr>)
        )
    : Impl{Other.Impl}
{
    this->CreateInvariantWeak();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TDerived>
    requires(std::is_same_v<typename TDerived::Encoding, typename TStringBase<TEncoding, TAllocator>::Encoding>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(const TDerived& Other) noexcept
    requires(Lal::AssignableFromWeak<TAllocator&, const typename TDerived::Allocator&>)
{
    this->Impl = Other.Impl;
    this->CreateInvariantWeak();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TDerived>
    requires(std::is_same_v<typename TDerived::Encoding, typename TStringBase<TEncoding, TAllocator>::Encoding>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(TDerived&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, typename TDerived::Allocator&&>)
    : Impl{std::move(Other.Impl)}
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseConcept TDerived>
    requires(std::is_same_v<typename TDerived::Encoding, typename TStringBase<TEncoding, TAllocator>::Encoding>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(TDerived&& Other) noexcept
    requires(Lal::AssignableFromWeak<TAllocator&, typename TDerived::Allocator&&>)
{
    this->Impl = std::move(Other.Impl);
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(UIterator Begin, VIterator End) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, VIterator>)
    : Impl{ Begin, End }
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator>
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(UIterator Begin, const SizeType Length) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, UIterator>)
    : Impl{ Begin, Begin + Length }
{
    this->CreateInvariant();
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(Pointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, Pointer, Pointer>)
    : Impl{ String, String + Encoding::GetStringLength(String) }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(Pointer String) noexcept
    requires(!TStringBase::
    IsStringView() && std::is_constructible_v<TAllocator, Pointer, Pointer>)
    : Impl{ String, String + Encoding::GetStringLength(String) + /*Terminator*/1 }
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(Pointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, Pointer, Pointer>)
{
    const SizeType Length { Encoding::GetStringLength(String) };
    this->Impl = Allocator{ String, String + Length };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>& TStringBase<TEncoding, TAllocator>::operator=(Pointer String) noexcept
    requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); })
{
    const SizeType Length { Encoding::GetStringLength(String) };

    this->Reset(Length);
    this->Impl.Append(String, String + Length + /*Terminator*/1);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(ConstPointer String) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>::TStringBase(ConstPointer String) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr TStringBase<TEncoding, TAllocator>&
TStringBase<TEncoding, TAllocator>::operator=(ConstPointer String) noexcept
    requires(TStringBase::IsStringView() && std::is_constructible_v<TAllocator, typename TAllocator::ConstPointer, typename TAllocator::ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false)
{
    const SizeType Length { Encoding::GetStringLength(String) };
    this->Impl = Allocator{ String, String + Length };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return *this;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::GetRuneCount() const noexcept
    requires(TStringBase::IsStringView())
{
    return this->Impl.GetSize();
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::GetRuneCount() const noexcept
    requires(!TStringBase::IsStringView())
{
    const SizeType Size { this->Impl.GetSize() };
    if (Size == 0)
    {
        return 0;
    }

    return Size - 1;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::GetCharacterLengthAt(ITERATOR It) const noexcept
{
    check( this->IsValidIterator(It) )
    return Encoding::GetCharacterSize(std::to_address(It));
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T,typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::Reference
TStringBase<TEncoding, TAllocator>::operator[](const SizeType Index) noexcept
    requires(TAllocator::IsContentMutable())
{
    check( this->IsValidIndex(Index) )
    return this->Impl[Index];
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T,typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::Reference
TStringBase<TEncoding, TAllocator>::operator[](const SizeType Index) noexcept
    requires(!TAllocator::IsContentMutable())
{
    check( this->IsValidIndex(Index) )
    return this->Impl[Index];
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T,typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::ConstReference
TStringBase<TEncoding, TAllocator>::operator[](const SizeType Index) const noexcept
{
    check( this->IsValidIndex(Index) )
    return this->Impl[Index];
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::end_idx() const noexcept
    requires(TStringBase::IsStringView())
{
    return this->Impl.end_idx();
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::end_idx() const noexcept
    requires(!TStringBase::IsStringView())
{
    SizeType ImplIndex { this->Impl.end_idx() };
    if (ImplIndex == 0)
    {
        return 0;
    }

    return ImplIndex - 1;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::ToIndex(ITERATOR It) const noexcept
{
    check( this->IsValidIterator(It) )
    return this->Impl.ToIndex(It);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr i64 TStringBase<TEncoding, TAllocator>::ToWeakIndex(ITERATOR It) const noexcept
{
    if (this->IsValidIterator(It))
    {
        return static_cast<i64>(this->Impl.ToIndex(It));
    }

    return INDEX_NONE;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T,typename TAllocator::SizeType>>)
FORCEINLINE constexpr void TStringBase<TEncoding, TAllocator>::SwapStrings(TStringBase* Other) noexcept
    requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); })
{
    check( Other )
    this->Impl.SwapBuffers(&Other->Impl);

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::SwapIndices(UIterator InA, VIterator InB) noexcept
    requires(Allocator::IsContentMutable())
{
    check( this->IsValidIterator(InA) )
    check( this->IsValidIterator(InB) )

    this->Impl.SwapIndices(InA, InB);

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
void TStringBase<TEncoding, TAllocator>::SwapIndices(const SizeType InA, const SizeType InB) noexcept
    requires(Allocator::IsContentMutable())
{
    this->SwapIndices(this->begin() + InA, this->begin() + InB);
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
{
    return std::equal(this->begin(), this->end(), Other.begin(), Other.end());
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
    template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator!=(const TStringBase<TEncoding, UAllocator>& Other) const noexcept
    requires(TStringBaseEncodingConcept<TEncoding<typename UAllocator::T, typename UAllocator::SizeType>>)
{
    return !(*this == Other);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(ConstPointer String) const noexcept
{
    const SizeType Length { Encoding::GetStringLength(String) };

    if (this->GetRuneCount() != Length)
    {
        return false;
    }

    return std::memcmp(this->begin_ptr(), String, Length) == 0;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator!=(ConstPointer String) const noexcept
{
    return !(*this == String);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::operator==(T Rune) const noexcept
{
    return this->GetRuneCount() <= 1 && *this->Peek() == Rune;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::Equals(ConstPointer Begin, ConstPointer End) const noexcept
{
    return static_cast<SizeType>(End - Begin) == this->GetRuneCount() && std::memcmp(this->begin_ptr(), Begin, End - Begin) == 0;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr bool TStringBase<TEncoding, TAllocator>::Equals(UIterator Begin, VIterator End) const noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr std::strong_ordering TStringBase<TEncoding, TAllocator>::SpaceShip(T Rune) const noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
constexpr std::strong_ordering TStringBase<TEncoding, TAllocator>::SpaceShip(UIterator Begin, VIterator End) const noexcept
{
    SizeType Size1 { this->Impl.GetSlackPointer() - this->Impl.GetDataPointer() };
    SizeType Size2 { End - Begin };
    auto MinSize { Maths::Min(Size1, Size2) };

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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Append(UIterator Begin, VIterator End) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::Iterator TStringBase<TEncoding, TAllocator>::AppendAt(const SizeType Index, UIterator Begin, VIterator End) noexcept
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

        check( this->IsValidIterator(Out) )

        return Out;
    }

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return this->end();
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::AppendToNew(UIterator Begin, VIterator End) const noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::AppendPath(UIterator Begin, VIterator End) noexcept
    requires(Allocator::IsAllowedToPushItems())
{
    check( Begin <= End )

    if (SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        this->Impl.Pop(); /* Terminator */

        if (ConstPointer Last { this->Impl.Peek() }; Last)
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseConcept TOther>
    requires(TIteratorPairConcept<UIterator, VIterator> && TValidOtherStringConcept<TStringBase<TEncoding, TAllocator>, TOther> && TOther::IsOwningString())
FORCEINLINE TOther TStringBase<TEncoding, TAllocator>::AppendPathToNew(UIterator Begin, VIterator End) const noexcept
{
    check( Begin <= End )

    if (const SizeType Length { static_cast<SizeType>(End - Begin) }; LAL_LIKELY(Length > 0))
    {
        TOther Out;
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
    return TOther{ this->begin(), this->end() };
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::RemoveAt(const SizeType Begin, const SizeType End) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    check( this->IsValidIndex(Begin) && Begin <= End && (Begin == End || this->IsValidIndex(End - 1)) )
    this->Impl.RemoveAt(Begin, End);

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceChecked(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Other, bAllowShrinking) };
    check( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator> requires(
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Other, bAllowShrinking) };
    jassert( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator> requires(
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceChecked(ConstPointer String, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(String, bAllowShrinking) };
    check( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceAsserted(ConstPointer String, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(String, bAllowShrinking) };
    jassert( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Remove(UIterator Begin, VIterator End, const bool bAllowShrinking /* = true */) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceChecked(UIterator Begin, VIterator End, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Begin, End, bAllowShrinking) };
    check( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceAsserted(UIterator Begin, VIterator End, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Begin, End, bAllowShrinking) };
    jassert( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Remove(T Rune, const bool bAllowShrinking /* = false */) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceChecked(T Rune, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Rune, bAllowShrinking) };
    check( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType
TStringBase<TEncoding, TAllocator>::RemoveAtLeastOnceAsserted(T Rune, const bool bAllowShrinking) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(Rune, bAllowShrinking) };
    jassert( Count > 0 )
    return Count;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator> requires(
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceChecked(const TStringBase<TEncoding, UAllocator>& Other) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Other) };
    check( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Other) };
    jassert( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceChecked(ConstPointer String) noexcept requires(Allocator::
    IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(String) };
    check( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceAsserted(ConstPointer String) noexcept requires(Allocator::
    IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(String) };
    jassert( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnce(UIterator Begin, VIterator End) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceChecked(UIterator Begin, VIterator End) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Begin, End) };
    check( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceAsserted(UIterator Begin, VIterator End) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Begin, End) };
    jassert( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnce(T Rune) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    check( Rune != Encoding::Terminator )
    const SizeType Out { this->Impl.RemoveOnce(Rune) };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return Out;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceChecked(T Rune) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Rune) };
    check( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::RemoveOnceAsserted(T Rune) noexcept
    requires(Allocator::IsAllowedToPopItemsInBetween())
{
    const bool Result { this->RemoveOnce(Rune) };
    jassert( Result )
    return Result;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Replace(T Rune, const T Replacement) noexcept
    requires(Allocator::IsContentMutable())
{
    check( Rune != Encoding::Terminator )
    check( Replacement != Encoding::Terminator )

    const SizeType Out { this->Impl.Replace(Rune, Replacement) };

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    return Out;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Replace(UIterator Begin, VIterator End, WIterator ReplacementBegin, XIterator ReplacementEnd, const bool bAllowShrinking /* = true */) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
void TStringBase<TEncoding, TAllocator>::Substitute(UIterator Begin, VIterator End, WIterator ReplacementBegin, XIterator ReplacementEnd, const bool bAllowShrinking /* = true */) noexcept
    requires(TStringBase::IsOwningString() && Allocator::IsContentMutable())
{
    check( this->IsValidIterator(Begin) && this->IsValidEndIterator(End) && Begin <= End )
    check( ReplacementBegin <= ReplacementEnd )

    static_assert(TIterator_IsContiguous_v<UIterator>);
    static_assert(TIterator_IsContiguous_v<WIterator>);

    const i64 Delta { static_cast<i64>(ReplacementEnd - ReplacementBegin) - static_cast<i64>(End - Begin) };
    if (Delta == 0)
    {
        std::memcpy(static_cast<void*>(std::to_address(Begin)), std::to_address(ReplacementBegin), ReplacementEnd - ReplacementBegin);
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
    std::memcpy(this->begin_ptr() + BeginIdx, std::to_address(ReplacementBegin), ReplacementEnd - ReplacementBegin);

    this->Impl.GetMutableAllocator().Slack += Delta;

    PRIVATE_LAL_ENSURE_STRING_INVARIANT()

    if (bAllowShrinking)
    {
        this->ShrinkToFit();
    }

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator> requires(
    TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Substitute(UIterator Begin, VIterator End, T Rune,
    const bool bAllowShrinking) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable())
{
    check( Rune != Encoding::Terminator )
    this->Substitute(Begin, End, &Rune, &Rune + 1, bAllowShrinking);

    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
bool TStringBase<TEncoding, TAllocator>::StartsWith(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
bool TStringBase<TEncoding, TAllocator>::EndsWith(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept
{
    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    if (OtherLength > static_cast<SizeType>(End - Begin))
    {
        return false;
    }

    return TStringBase::StartsWith(End - OtherLength, End, OtherBegin, OtherEnd);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
bool TStringBase<TEncoding, TAllocator>::EndsWith(UIterator Begin, VIterator End, T Rune) noexcept
{
    check( Begin <= End )

    if (Begin == End)
    {
        return false;
    }

    return *(End - 1) == Rune;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindFirst(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE UIterator TStringBase<TEncoding, TAllocator>::FindFirst(UIterator Begin, VIterator End, T Rune) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNth(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd, SizeType N) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
UIterator TStringBase<TEncoding, TAllocator>::FindNth(UIterator Begin, VIterator End, T Rune, SizeType N) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::ConstIterator
TStringBase<TEncoding, TAllocator>::FindLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept
    requires(ITERATOR_PAIR(Begin, End) && ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, OtherBegin))
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    decltype(End) Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (static_cast<SizeType>(End - Cursor) < OtherLength)
        {
            --Cursor;
            continue;
        }

        decltype(OtherBegin) InnerCursor { OtherBegin };
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::ConstIterator
TStringBase<TEncoding, TAllocator>::FindLast(ITERATOR Begin, ITERATOR End, T Rune) noexcept
    requires(ITERATOR_PAIR(Begin, End))
{
    check( Begin <= End )

    decltype(End) Cursor { End - 1 };
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::ConstIterator
TStringBase<TEncoding, TAllocator>::FindNthLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd, SizeType N) noexcept
    requires(ITERATOR_PAIR(Begin, End) && ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, OtherBegin))
{
    check( Begin <= End )
    check( OtherBegin <= OtherEnd )
    check( OtherBegin < Begin || OtherBegin > End )
    check( OtherEnd < Begin || OtherEnd > End )

    const SizeType OtherLength { static_cast<SizeType>(OtherEnd - OtherBegin) };

    decltype(End) Cursor { End - 1 };
    while (Cursor >= Begin)
    {
        check( *Cursor != Encoding::Terminator )

        if (static_cast<SizeType>(End - Cursor) < OtherLength)
        {
            --Cursor;
            continue;
        }

        decltype(OtherBegin) InnerCursor { OtherBegin };
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::ConstIterator
TStringBase<TEncoding, TAllocator>::FindNthLast(ITERATOR Begin, ITERATOR End, T Rune, SizeType N) noexcept
    requires(ITERATOR_PAIR(Begin, End))
{
    check( Begin <= End )

    decltype(End) Cursor { End - 1 };
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator>
    requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Count(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::Count(UIterator Begin, VIterator End, T Rune) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::Cut(const SizeType Begin, const SizeType End) const noexcept
{
    check( Begin <= End )
    return TStringBase<TEncoding, UAllocator>{ this->begin() + Begin, this->begin() + End };
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineLeftCut(const SizeType Index, const bool bAllowShrinking /* = true */) noexcept
{
    this->InlineCut(0ul, Index, bAllowShrinking);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::LeftCut(const SizeType Index) const noexcept
{
    return this->template Cut<UAllocator>(0ul, Index);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::InlineRightCut(const SizeType Index, const bool bAllowShrinking /* = true */) noexcept
{
    this->InlineCut(Index, this->end_idx(), bAllowShrinking);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::RightCut(const SizeType Index) const noexcept
{
    return this->template Cut<UAllocator>(Index, this->end_idx());
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::LeftChop(const SizeType Count) const noexcept
{
    check( Count <= this->GetRuneCount() )
    return TStringBase<TEncoding, UAllocator>{ this->begin(), this->end() - Count };
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TStringBaseAllocatorConcept UAllocator>
FORCEINLINE TStringBase<TEncoding, UAllocator> TStringBase<TEncoding, TAllocator>::RightChop(const SizeType Count) const noexcept
{
    check( Count <= this->GetRuneCount() )
    return TStringBase<TEncoding, UAllocator>{ this->begin() + Count, this->end() };
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
typename TStringBase<TEncoding, TAllocator>::SizeType TStringBase<TEncoding, TAllocator>::GetLineNumber(ITERATOR It) const noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE auto TStringBase<TEncoding, TAllocator>::Push(this auto&& Self, UIterator Begin, VIterator End) noexcept -> decltype(Self)
    requires(Allocator::IsAllowedToPushItems())
{
    std::forward<Self>(Self).Push(Begin, End);
    return std::forward<Self>(Self);
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::Pop() noexcept
    requires(TStringBase::IsStringView())
{
    const bool bOut { this->Impl.Pop() };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return bOut;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE void TStringBase<TEncoding, TAllocator>::Pop(const SizeType Count) noexcept
    requires(TStringBase::IsStringView())
{
    this->Impl.Pop(Count);
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE bool TStringBase<TEncoding, TAllocator>::Drop() noexcept
{
    const bool bOut { this->Impl.Drop() };
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    return bOut;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
FORCEINLINE constexpr typename TStringBase<TEncoding, TAllocator>::ConstPointer TStringBase<TEncoding, TAllocator>::Peek() const noexcept
{
    PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    if (this->Impl.GetSize() > 0)
    {
        return this->Impl.GetDataPointer() + this->GetRuneCount() - 1;
    }

    return &Encoding::Terminator;
}

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <typename... TArgs>
FORCEINLINE TStringBase<TEncoding, TAllocator> TStringBase<TEncoding, TAllocator>::SprintF(ConstPointer Format, const TArgs&... Args) noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
void TStringBase<TEncoding, TAllocator>::AssignImpl(UIterator Begin, VIterator End) noexcept
    requires(!TStringBase::IsStringView())
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
void TStringBase<TEncoding, TAllocator>::CreateInvariant() noexcept
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

template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
void TStringBase<TEncoding, TAllocator>::CreateInvariantWeak() noexcept
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
template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T,typename TAllocator::SizeType>>)
void TStringBase<TEncoding, TAllocator>::EnsureInvariant() const
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

} /* Namespace Lal */

//#
//# Guaranteed to be the same size as std::string.
//#
static_assert(sizeof(std::string) == sizeof(LString));

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<int>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LString> == false);

template <>
struct std::formatter<LString> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LString& String,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(String.begin_ptr(), String.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LBigString> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LBigString& String,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(String.begin_ptr(), String.end_ptr()), InContext);
    }
};

template <LSize TCapacity>
struct std::formatter<LSmallString<TCapacity>> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LSmallString<TCapacity>& String,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(String.begin_ptr(), String.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LStringView> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LStringView& String,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(String.begin_ptr(), String.end_ptr()), InContext);
    }
};

template <>
struct std::formatter<LMutableStringView> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LMutableStringView& String,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view(String.begin_ptr(), String.end_ptr()), InContext);
    }
};
