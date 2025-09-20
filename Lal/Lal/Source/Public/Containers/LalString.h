// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

//# @see Containers/ContainerForward.h
template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TStringBase : public Private::LStringBase
{
public:

    typedef TEncoding<
        typename TAllocator::T,
        typename TAllocator::SizeType
        >  Encoding;

    typedef TAllocator                         Allocator;
    typedef typename Allocator::T              T;
    typedef typename Allocator::SizeType       SizeType;
    typedef typename Allocator::Iterator       Iterator;
    typedef typename Allocator::ConstIterator  ConstIterator;
    typedef typename Allocator::Pointer        Pointer;
    typedef typename Allocator::ConstPointer   ConstPointer;
    typedef typename Allocator::Reference      Reference;
    typedef typename Allocator::ConstReference ConstReference;

    typedef TStringBase<TEncoding, TArrayView<T>> _WeakRepr;
    typedef TStringBase<TEncoding, TMutableArrayView<T>> _WeakMutableRepr;

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
    static_assert(TStringBase::IsStringView() != TStringBase::IsOwningString());

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return Allocator::IsContentConst(); }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return Allocator::IsContentMutable(); }
    static_assert(TStringBase::IsContentConst() != TStringBase::IsContentMutable());

    FORCEINLINE constexpr TStringBase() noexcept requires(std::is_default_constructible_v<Allocator>) = default;
    FORCEINLINE constexpr TStringBase(LNullptrTy) noexcept requires(std::is_default_constructible_v<Allocator>) : Impl{} { return; }

    FORCEINLINE constexpr TStringBase(const TStringBase& Other) noexcept
        requires(std::is_constructible_v<Allocator, const Allocator&>);
    FORCEINLINE constexpr TStringBase& operator=(const TStringBase& Other) noexcept
        requires(Lal::AssignableFrom<Allocator&, const Allocator&>);

    FORCEINLINE constexpr TStringBase(TStringBase&& Other) noexcept
        requires(std::is_constructible_v<Allocator, Allocator&&>);
    FORCEINLINE constexpr TStringBase& operator=(TStringBase&& Other) noexcept
        requires(Lal::AssignableFrom<Allocator&, Allocator&&>);

    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE explicit constexpr TStringBase(const TDerived& Other) noexcept
        requires(std::is_constructible_v<Allocator, const typename TDerived::Allocator&> && !(std::is_same_v<TStringBase, typename TDerived::_WeakRepr> || std::is_same_v<TStringBase, typename TDerived::_WeakMutableRepr>));
    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE constexpr TStringBase(const TDerived& Other) noexcept
        requires(std::is_constructible_v<Allocator, const typename TDerived::Allocator&> && (std::is_same_v<TStringBase, typename TDerived::_WeakRepr> || std::is_same_v<TStringBase, typename TDerived::_WeakMutableRepr>));
    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE constexpr TStringBase& operator=(const TDerived& Other) noexcept
        requires(Lal::AssignableFromWeak<Allocator&, const typename TDerived::Allocator&>);

    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE explicit constexpr TStringBase(TDerived&& Other) noexcept
        requires(std::is_constructible_v<Allocator, typename TDerived::Allocator&&>);
    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE constexpr TStringBase& operator=(TDerived&& Other) noexcept
        requires(Lal::AssignableFromWeak<Allocator&, typename TDerived::Allocator&&>);

    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE explicit constexpr TStringBase(TDerived&& Other) noexcept
        requires(std::is_constructible_v<Allocator, typename TDerived::Allocator&&> == false) = delete;
    template <TStringBaseConcept TDerived> requires(std::is_same_v<typename TDerived::Encoding, Encoding>)
    FORCEINLINE TStringBase& operator=(TDerived&& Other) noexcept
        requires(Lal::AssignableFromWeak<Allocator&, typename TDerived::Allocator&&> == false) = delete;

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TStringBase(UIterator Begin, VIterator End) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, VIterator>);
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr TStringBase(UIterator Begin, const SizeType Length) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, UIterator>);

    FORCEINLINE constexpr TStringBase(Pointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase(Pointer String) noexcept
        requires(!TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase& operator=(Pointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, Pointer, Pointer>);
    FORCEINLINE constexpr TStringBase& operator=(Pointer String) noexcept
        requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); });

    FORCEINLINE constexpr TStringBase(ConstPointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase(ConstPointer String) noexcept
        requires(!TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);

    FORCEINLINE constexpr TStringBase& operator=(ConstPointer String) noexcept
        requires(TStringBase::IsStringView() && std::is_constructible_v<Allocator, ConstPointer, ConstPointer> && std::is_same_v<Pointer, ConstPointer> == false);
    FORCEINLINE constexpr TStringBase& operator=(ConstPointer String) noexcept
        requires(!TStringBase::IsStringView() && requires(Allocator _Allocator) { _Allocator.Append(String, String); } && std::is_same_v<Pointer, ConstPointer> == false)
    {
        const SizeType Length { Encoding::GetStringLength(String) };

        this->Reset(Length);
        this->Impl.Append(String, String + Length + /*Terminator*/1);

        PRIVATE_LAL_ENSURE_STRING_INVARIANT()
        return *this;
    }

    FORCEINLINE constexpr TStringBase(T Rune) noexcept
        requires(!TStringBase::IsStringView() && std::is_default_constructible_v<Allocator> && TIsChar_v<T>)
        : Impl{}
    {
        this->Assign(Rune);
        return;
    }
    FORCEINLINE constexpr TStringBase(std::integral auto Rune) noexcept
        requires(!TStringBase::IsStringView() && std::is_default_constructible_v<Allocator>) = delete;
    FORCEINLINE constexpr TStringBase& operator=(T Rune) noexcept
        requires(!TStringBase::IsStringView() && TIsChar_v<T>)
    {
        this->Assign(Rune);
        return *this;
    }
    FORCEINLINE constexpr TStringBase& operator=(std::integral auto Rune) noexcept
        requires(!TStringBase::IsStringView()) = delete;

    FORCEINLINE constexpr ~TStringBase() noexcept = default;

    //# Equivalent to #c_str() on std::string. Always returns a c style string.
    NODISCARD
    FORCEINLINE ConstPointer ToPtr() const noexcept requires(!TStringBase::IsStringView()) { if (this->GetRuneCount() > 0) { return this->Impl.GetDataPointer(); } return &Encoding::Terminator; }
    NODISCARD
    FORCEINLINE SizeType GetRuneCount() const noexcept requires(TStringBase::IsStringView());
    NODISCARD
    FORCEINLINE SizeType GetRuneCount() const noexcept requires(!TStringBase::IsStringView());
    NODISCARD
    FORCEINLINE SizeType GetSizeInBytes() const noexcept { return this->Impl.GetSizeInBytes(); }
    NODISCARD
    FORCEINLINE SizeType GetCharacterCount() const noexcept { return Encoding::GetCharacterCount(this->begin_ptr(), this->end_ptr()); }
    NODISCARD
    FORCEINLINE SizeType GetAllocatedByteSize() const noexcept { return this->Impl.GetAllocatedByteSize(); }
    NODISCARD
    FORCEINLINE bool IsEmpty() const noexcept { return this->GetRuneCount() == 0; }

    NODISCARD
    FORCEINLINE SizeType GetCharacterLengthAt(const SizeType Index) const noexcept { return this->GetCharacterLengthAt(this->begin() + Index); }
    NODISCARD
    FORCEINLINE SizeType GetCharacterLengthAt(ITERATOR It) const noexcept;
    NODISCARD
    FORCEINLINE static SizeType s_GetCharacterLengthAt(ITERATOR It) noexcept { return Encoding::GetCharacterSize(std::to_address(It)); }

    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsValidIterator(const TIterator& It) const noexcept { return std::to_address(It) >= this->begin_ptr() && std::to_address(It) < this->end_ptr(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires( std::is_signed_v<SizeType>) { return Index >= 0 && Index < this->GetRuneCount(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires(!std::is_signed_v<SizeType>) { return Index < this->GetRuneCount(); }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsValidEndIterator(const TIterator& It) const noexcept { return std::to_address(It) >= this->begin_ptr() && std::to_address(It) <= this->end_ptr(); }

    NODISCARD FORCEINLINE constexpr Reference operator[](const SizeType Index) noexcept requires( TAllocator::IsContentMutable());
    NODISCARD FORCEINLINE constexpr Reference operator[](const SizeType Index) noexcept requires(!TAllocator::IsContentMutable());
    NODISCARD FORCEINLINE constexpr ConstReference operator[](const SizeType Index) const noexcept;

    FORCEINLINE constexpr Iterator begin() noexcept { return this->Impl.begin(); }
    FORCEINLINE constexpr Iterator end() noexcept requires(TStringBase::IsStringView()) { return this->Impl.end(); }
    FORCEINLINE constexpr Iterator end() noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.end() }; if (It == this->begin()) { return It; } return --It; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return this->Impl.begin(); }
    FORCEINLINE constexpr ConstIterator end() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.end(); }
    FORCEINLINE constexpr ConstIterator end() const noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.end() }; if (It == this->begin()) { return It; } return --It; }
    FORCEINLINE constexpr ConstIterator cbegin() const noexcept { return this->Impl.cbegin(); }
    FORCEINLINE constexpr ConstIterator cend() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.cend(); }
    FORCEINLINE constexpr ConstIterator cend() const noexcept requires(!TStringBase::IsStringView()) { auto It { this->Impl.cend() }; if (It == this->cbegin()) { return It; } return --It; }

    FORCEINLINE constexpr Pointer begin_ptr() noexcept { return this->Impl.begin_ptr(); }
    FORCEINLINE constexpr Pointer end_ptr() noexcept requires(TStringBase::IsStringView()) { return this->Impl.end_ptr(); }
    FORCEINLINE constexpr Pointer end_ptr() noexcept requires(!TStringBase::IsStringView()) { auto Ptr { this->Impl.end_ptr() }; if (Ptr == this->begin_ptr()) { return Ptr; } return --Ptr; }
    FORCEINLINE constexpr ConstPointer begin_ptr() const noexcept { return this->Impl.begin_ptr(); }
    FORCEINLINE constexpr ConstPointer end_ptr() const noexcept requires(TStringBase::IsStringView()) { return this->Impl.end_ptr(); }
    FORCEINLINE constexpr ConstPointer end_ptr() const noexcept requires(!TStringBase::IsStringView()) { auto Ptr { this->Impl.end_ptr() }; if (Ptr == this->begin_ptr()) { return Ptr; } return --Ptr; }

    FORCEINLINE constexpr SizeType end_idx() const noexcept requires( TStringBase::IsStringView());
    FORCEINLINE constexpr SizeType end_idx() const noexcept requires(!TStringBase::IsStringView());

    FORCEINLINE constexpr auto Iter() noexcept { return this->Impl.Iter(); }
    FORCEINLINE constexpr auto CIter() const noexcept { return this->Impl.CIter(); }

    FORCEINLINE constexpr SizeType ToIndex(ITERATOR It) const noexcept;
    FORCEINLINE constexpr i64 ToWeakIndex(ITERATOR It) const noexcept;

    FORCEINLINE constexpr void Reserve(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.Reserve(Count + /*Terminator*/1); }
    FORCEINLINE constexpr void ReserveAdditionally(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.ReserveAdditionally(Count); }
    FORCEINLINE constexpr void Reset(const SizeType Count, const bool bAllowShrinking = true) noexcept requires(TAllocator::IsStronglyAllocated());
    FORCEINLINE constexpr void Resize(const SizeType Count) noexcept requires(TAllocator::IsStronglyAllocated());
    FORCEINLINE constexpr void ShrinkToFit() noexcept requires(TAllocator::IsStronglyAllocated()) { this->Impl.ShrinkToFit(); }
    FORCEINLINE constexpr void Empty() noexcept { this->Impl.Empty(); }

    FORCEINLINE constexpr void SwapStrings(TStringBase* Other) noexcept requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); });

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void SwapIndices(UIterator InA, VIterator InB) noexcept requires(Allocator::IsContentMutable());
    FORCEINLINE void SwapIndices(const SizeType  InA, const SizeType  InB) noexcept requires(Allocator::IsContentMutable());

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
    FORCEINLINE constexpr bool operator==(ConstPointer String) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool operator!=(ConstPointer String) const noexcept;

    NODISCARD
    FORCEINLINE constexpr bool operator==(T Rune) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool operator!=(T Rune) const noexcept { return this->operator==(Rune) == false; }

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
    FORCEINLINE constexpr bool Equals(ConstPointer String) const noexcept { return this->operator==(String); }
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(ConstPointer String) const noexcept { return this->Equals(String) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(T Rune) const noexcept { return this->operator==(Rune); }
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(T Rune) const noexcept { return this->Equals(Rune) == false; }

    NODISCARD
    FORCEINLINE constexpr bool Equals(ConstPointer Begin, ConstPointer End) const noexcept;
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(ConstPointer Begin, ConstPointer End) const noexcept { return this->Equals(Begin, End) == false; }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    NODISCARD
    FORCEINLINE constexpr bool Equals(UIterator Begin, VIterator End) const noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    NODISCARD
    FORCEINLINE constexpr bool UnEquals(UIterator Begin, VIterator End) const noexcept { return this->Equals(Begin, End) == false; }

    template <TStringBaseAllocatorConcept UAllocator>
    NODISCARD
    constexpr std::strong_ordering operator<=>(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->SpaceShip(Other.begin(), Other.end()); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(ConstPointer String) const noexcept { return this->SpaceShip(ConstIterator{String}, ConstIterator{String + Encoding::GetStringLength(String)}); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(T Rune) const noexcept { return this->SpaceShip(Rune); }
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering SpaceShip(T Rune) const noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    NODISCARD
    constexpr std::strong_ordering SpaceShip(UIterator Begin, VIterator End) const noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    auto Assign(this auto&& Self, const TStringBase<TEncoding, UAllocator>& Other) noexcept -> decltype(Self) { return Self.Assign(Other.begin(), Other.end()); }
    auto Assign(this auto&& Self, ConstPointer String) noexcept -> decltype(Self) { return Self.Assign(String, String + Encoding::GetStringLength(String)); }
    auto Assign(this auto&& Self, ITERATOR Begin, ITERATOR End) noexcept -> decltype(Self) requires(ITERATOR_PAIR(Begin, End) && TStringBase::IsStringView()) { Self.Impl.operator=(Allocator{ Begin, End }); PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(Self) return std::forward<decltype(Self)>(Self); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    auto Assign(this auto&& Self, UIterator Begin, VIterator End) noexcept -> decltype(Self) requires(!TStringBase::IsStringView()) { Self.AssignImpl(Begin, End); return std::forward<decltype(Self)>(Self); }
    auto Assign(this auto&& Self, T Rune) noexcept -> decltype(Self) { return Self.Assign(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE void Append(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Other.begin(), Other.end()); }
    FORCEINLINE void Append(ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void Append(UIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(Begin, Begin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void Append(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE void Append(T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->Append(&Rune, &Rune + 1); }

    //#
    //# Appends the specified string or rune at the given iterator position.
    //# @return The iterator to the first newly inserted rune. If the range is empty, the iterator will point to
    //#         the slack.
    //#
    template <TIteratorConcept UIterator, TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE Iterator AppendAt(UIterator It, const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, Other.begin(), Other.end()); }
    template <TIteratorConcept UIterator>
    FORCEINLINE Iterator AppendAt(UIterator It, ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    FORCEINLINE Iterator AppendAt(UIterator It, VIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, Begin, Begin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator>)
    FORCEINLINE Iterator AppendAt(UIterator It, VIterator Begin, WIterator End) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(std::to_address(It) - this->Impl.GetDataPointer(), Begin, End); }
    template <TIteratorConcept UIterator>
    FORCEINLINE Iterator AppendAt(UIterator It, T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(It, &Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, Other.begin(), Other.end()); }
    FORCEINLINE Iterator AppendAt(const SizeType Index, ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE Iterator AppendAt(const SizeType Index, UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE Iterator AppendAt(const SizeType Index, T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { return this->AppendAt(Index, &Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator, TStringBaseAllocatorConcept VAllocator = Allocator> requires(TStringBase<TEncoding, VAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, VAllocator> AppendToNew(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->template AppendToNew<ConstIterator, ConstIterator, VAllocator>(Other.begin(), Other.end()); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(ConstPointer String) const noexcept { return this->template AppendToNew<decltype(String), decltype(String), UAllocator>(String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator = Allocator> requires(TIteratorPairConcept<UIterator, VIterator> && TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(UIterator Begin, VIterator End) const noexcept;
    template <TStringBaseAllocatorConcept UAllocator = Allocator> requires(TStringBase<TEncoding, UAllocator>::IsOwningString())
    FORCEINLINE TStringBase<TEncoding, UAllocator> AppendToNew(T Rune) const noexcept { return this->template AppendToNew<decltype(&Rune), decltype(&Rune), UAllocator>(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE auto operator+(this auto&& Self, const TStringBase<TEncoding, UAllocator>& Other) noexcept -> decltype(auto) requires(TStringBase::IsOwningString()) { return Self.AppendToNew(Other.begin(), Other.end()); }
    FORCEINLINE auto operator+(this auto&& Self, ConstPointer String) noexcept -> decltype(auto) requires(TStringBase::IsOwningString()) { return Self.AppendToNew(String); }
    FORCEINLINE auto operator+(this auto&& Self, T Rune) noexcept -> decltype(auto) requires(TStringBase::IsOwningString()) { return Self.AppendToNew(Rune); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE auto operator+=(this auto&& Self, const TStringBase<TEncoding, UAllocator>& Other) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { Self.Append(Other); return std::forward<decltype(Self)>(Self); }
    FORCEINLINE auto operator+=(this auto&& Self, ConstPointer String) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { Self.Append(String); return std::forward<decltype(Self)>(Self); }
    FORCEINLINE auto operator+=(this auto&& Self, T Rune) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { Self.Append(Rune); return std::forward<decltype(Self)>(Self); }

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
    FORCEINLINE void AppendPath(ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String, Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void AppendPath(UIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Begin, Begin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void AppendPath(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE void AppendPath(T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator, TStringBaseConcept TOther = TStringBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TStringBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(const TStringBase<TEncoding, UAllocator>& Other) const noexcept{ return this->template AppendPathToNew<ConstIterator, ConstIterator, TOther>(Other.begin(), Other.end()); }
    template <TStringBaseConcept TOther = TStringBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TStringBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(ConstPointer String) const noexcept { return this->template AppendPathToNew<decltype(String), TOther>(String, static_cast<SizeType>(Encoding::GetStringLength(String))); }
    template <TIteratorConcept UIterator, TStringBaseConcept TOther = TStringBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TStringBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(UIterator String, const SizeType Length) const noexcept { return this->template AppendPathToNew<UIterator, UIterator, TOther>(String, String + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseConcept TOther = TStringBase> requires(TIteratorPairConcept<UIterator, VIterator> && TValidOtherStringConcept<TStringBase, TOther> && TOther::IsOwningString())
    FORCEINLINE TOther AppendPathToNew(UIterator Begin, VIterator End) const noexcept;
    template <TStringBaseConcept TOther = TStringBase> requires(TOther::IsOwningString() && TValidOtherStringConcept<TStringBase, TOther>)
    FORCEINLINE TOther AppendPathToNew(T Rune) const noexcept { return this->template AppendPathToNew<decltype(&Rune), decltype(&Rune), TOther>(&Rune, &Rune + 1); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE auto operator/(this auto&& Self, const TStringBase<TEncoding, UAllocator>& Other) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { return Self.AppendPathToNew(Other.begin(), Other.end()); }
    FORCEINLINE auto operator/(this auto&& Self, ConstPointer String) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { return Self.AppendPathToNew(String); }
    FORCEINLINE auto operator/(this auto&& Self, T Rune) noexcept -> decltype(auto) requires(Allocator::IsAllowedToPushItems()) { return Self.AppendPathToNew(Rune); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE TStringBase& operator/=(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Other); return *this; }
    FORCEINLINE TStringBase& operator/=(ConstPointer String) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(String); return *this; }
    FORCEINLINE TStringBase& operator/=(T Rune) noexcept requires(Allocator::IsAllowedToPushItems()) { this->AppendPath(Rune); return *this; }

    template <TIteratorConcept UIterator>
    FORCEINLINE void RemoveAt(UIterator It) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(It, It + 1); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void RemoveAt(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(std::to_address(Begin) - this->Impl.GetDataPointer(), std::to_address(End) - this->Impl.GetDataPointer()); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void RemoveAt(UIterator It, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(It, It + Length); }
    FORCEINLINE void RemoveAt(const SizeType Index) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { this->RemoveAt(Index, Index + 1); }
    FORCEINLINE void RemoveAt(const SizeType Begin, const SizeType End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType Remove(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(Other.begin(), Other.end(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType Remove(ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->Remove(String, String + Encoding::GetStringLength(String), bAllowShrinking); }
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(ConstPointer String, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType Remove(UIterator Begin, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(UIterator Begin, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(UIterator Begin, const SizeType Length, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType Remove(UIterator Begin, VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(UIterator Begin, VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(UIterator Begin, VIterator End, const bool bAllowShrinking = true) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType Remove(T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(T Rune, const bool bAllowShrinking = false) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnce(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(Other.begin(), Other.end()); }
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnceChecked(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool RemoveOnceAsserted(const TStringBase<TEncoding, UAllocator>& Other) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnce(ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween()) { return this->RemoveOnce(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE bool RemoveOnceChecked(ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnceAsserted(ConstPointer String) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE bool RemoveOnce(UIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE bool RemoveOnceChecked(UIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator>
    FORCEINLINE bool RemoveOnceAsserted(UIterator Begin, const SizeType Length) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnce(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnceChecked(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool RemoveOnceAsserted(UIterator Begin, VIterator End) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnce(T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnceChecked(T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());
    FORCEINLINE bool RemoveOnceAsserted(T Rune) noexcept requires(Allocator::IsAllowedToPopItemsInBetween());

    //#
    //# Replace all occurrences of the input string with the replacement string.
    //# @return The number of substrings replaced.
    //#
    FORCEINLINE SizeType Replace(T Rune, const T Replacement) noexcept requires(Allocator::IsContentMutable());
    FORCEINLINE SizeType Replace(ConstPointer String, ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(String, Encoding::GetStringLength(String), Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }

    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType Replace(UIterator Begin, const SizeType Length, ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType Replace(UIterator Begin, const SizeType Length, ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE SizeType Replace(UIterator Begin, const SizeType Length, WIterator ReplacementBegin, XIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Begin + Length, ReplacementBegin, ReplacementEnd, bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE SizeType Replace(UIterator Begin, const SizeType Length, T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, Length, &Rune, &Rune + 1, bAllowShrinking); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(UIterator Begin, VIterator End, ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(UIterator Begin, VIterator End, ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                SizeType Replace(UIterator Begin, VIterator End, WIterator ReplacementBegin, XIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType Replace(UIterator Begin, VIterator End, T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { return this->Replace(Begin, End, &Rune, &Rune + 1, bAllowShrinking); }

    //#
    //# Substitutes a range of runes with a replacement string.
    //#
    template <TIteratorConcept UIterator>
    FORCEINLINE void Substitute(UIterator Begin, const SizeType Length, ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void Substitute(UIterator Begin, const SizeType Length, ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE void Substitute(UIterator Begin, const SizeType Length, VIterator ReplacementBegin, WIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Begin + Length, ReplacementBegin, ReplacementEnd, bAllowShrinking); }
    template <TIteratorConcept UIterator>
    FORCEINLINE void Substitute(UIterator Begin, const SizeType Length, T Rune) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, Length, &Rune, &Rune + 1); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void Substitute(UIterator Begin, VIterator End, ConstPointer Replacement, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Encoding::GetStringLength(Replacement), bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void Substitute(UIterator Begin, VIterator End, ConstPointer Replacement, const SizeType ReplacementLength, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable()) { this->Substitute(Begin, End, Replacement, Replacement + ReplacementLength, bAllowShrinking); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                void Substitute(UIterator Begin, VIterator End, WIterator ReplacementBegin, XIterator ReplacementEnd, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable());
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void Substitute(UIterator Begin, VIterator End, T Rune, const bool bAllowShrinking = true) noexcept requires(TStringBase::IsOwningString() && Allocator::IsContentMutable());

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool StartsWith(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->StartsWith(Other.begin(), Other.end()); }
    FORCEINLINE bool StartsWith(ConstPointer String) const noexcept { return this->StartsWith(String, String + Encoding::GetStringLength(String)); }
    FORCEINLINE bool StartsWith(ConstPointer String, const SizeType Length) const noexcept { return this->StartsWith(String, String + Length); }
    FORCEINLINE bool StartsWith(ConstPointer Begin, ConstPointer End) const noexcept { return this->StartsWith(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool StartsWith(UIterator Begin, VIterator End) const noexcept { return TStringBase::StartsWith(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool StartsWith(T Rune) const noexcept { return *this->ToPtr() == Rune; /* Better performance. */ }

    template <TIteratorConcept UIterator>
    FORCEINLINE static bool StartsWith(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::StartsWith(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool StartsWith(UIterator Begin, const SizeType Length, ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::StartsWith(Begin, Length, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::StartsWith(Begin, Begin + Length, OtherBegin, OtherEnd); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool StartsWith(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::StartsWith(Begin, Begin + Length, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::StartsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(UIterator Begin, VIterator End, ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::StartsWith(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                static bool StartsWith(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool StartsWith(UIterator Begin, VIterator End, T Rune) noexcept { if (Begin != End) { return *Begin == Rune; } return false; /* Better performance. */ }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool EndsWith(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->EndsWith(Other.begin(), Other.end()); }
    FORCEINLINE bool EndsWith(ConstPointer String) const noexcept { return this->EndsWith(String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE bool EndsWith(UIterator Begin, const SizeType Length) const noexcept { return this->EndsWith(Begin, Begin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool EndsWith(UIterator Begin, VIterator End) const noexcept { return TStringBase::EndsWith(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool EndsWith(T Rune) const noexcept { return TStringBase::EndsWith(this->begin(), this->end(), Rune); }

    template <TIteratorConcept UIterator>
    FORCEINLINE static bool EndsWith(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::EndsWith(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool EndsWith(UIterator Begin, const SizeType Length, ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::EndsWith(Begin, Length, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::EndsWith(Begin, Begin + Length, OtherBegin, OtherEnd); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool EndsWith(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::EndsWith(Begin, Begin + Length, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::EndsWith(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool EndsWith(UIterator Begin, VIterator End, ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::EndsWith(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                static bool EndsWith(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
                static bool EndsWith(UIterator Begin, VIterator End, T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindFirst(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindFirst(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindFirst(ConstPointer String) const noexcept { return this->FindFirst(String, Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE ConstIterator FindFirst(UIterator String, const SizeType Length) const noexcept { return this->FindFirst(String, String + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindFirst(UIterator Begin, VIterator End) const noexcept { return TStringBase::FindFirst(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindFirst(T Rune) const noexcept { return TStringBase::FindFirst(this->begin(), this->end(), Rune); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE i64 FindFirstIndex(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { if (auto It { this->FindFirst(Other, Other) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    FORCEINLINE i64 FindFirstIndex(ConstPointer String) const noexcept { if (auto It { this->FindFirst(String) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    template <TIteratorConcept UIterator>
    FORCEINLINE i64 FindFirstIndex(UIterator String, const SizeType Length) const noexcept { if (auto It { this->FindFirst(String, Length) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE i64 FindFirstIndex(UIterator Begin, VIterator End) const noexcept { if (auto It { this->FindFirst(Begin, End) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    FORCEINLINE i64 FindFirstIndex(T Rune) const noexcept { if (auto It { this->FindFirst(Rune) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }

    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, const SizeType Length, UIterator OtherBegin, VIterator OtherEnd) noexcept { return TStringBase::FindFirst(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, const SizeType Length, T Rune) noexcept { return TStringBase::FindFirst(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, ConstPointer End, ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, ConstPointer End, UIterator OtherBegin, VIterator OtherEnd) noexcept { return TStringBase::FindFirst(ConstIterator{ Begin }, ConstIterator{ End }, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindFirst(ConstPointer Begin, ConstPointer End, T Rune) noexcept { return TStringBase::FindFirst(ConstIterator{ Begin }, ConstIterator{ End }, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindFirst(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::FindFirst(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static UIterator FindFirst(UIterator Begin, VIterator End, WIterator OtherBegin, const SizeType Length) noexcept { return TStringBase::FindFirst(Begin, End, OtherBegin, OtherBegin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                static UIterator FindFirst(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
                static UIterator FindFirst(UIterator Begin, VIterator End, T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindSecond(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindSecond(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindSecond(ConstPointer String) const noexcept { return this->FindSecond(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindSecond(ConstPointer String, const SizeType Length) const noexcept { return this->FindSecond(String, String + Length); }
    FORCEINLINE ConstIterator FindSecond(ConstPointer Begin, ConstPointer End) const noexcept { return this->FindSecond(ConstIterator{ Begin }, ConstIterator{ End }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindSecond(UIterator Begin, VIterator End) const noexcept { return TStringBase::FindSecond(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindSecond(T Rune) const noexcept { return TStringBase::FindSecond(this->begin(), this->end(), Rune); }

    template <TIteratorConcept UIterator>
    FORCEINLINE static ConstIterator FindSecond(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::FindSecond(Begin, Begin + Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, const SizeType Length, VIterator OtherBegin, const SizeType OtherLength) noexcept { return TStringBase::FindNth(Begin, Begin + Length, OtherBegin, OtherBegin + OtherLength, 2); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::FindNth(Begin, Begin + Length, OtherBegin, OtherEnd, 2); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static UIterator FindSecond(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::FindNth(Begin, Begin + Length, Rune, 2); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::FindSecond(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindSecond(UIterator Begin, VIterator End, WIterator Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecond(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept { return TStringBase::FindNth(Begin, End, OtherBegin, OtherEnd, 2); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static UIterator FindSecond(UIterator Begin, VIterator End, T Rune) noexcept { return TStringBase::FindNth(Begin, End, Rune, 2); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindNth(const TStringBase<TEncoding, UAllocator>& Other, const SizeType N) const noexcept { return this->FindNth(Other.begin(), Other.end(), N); }
    FORCEINLINE ConstIterator FindNth(ConstPointer String, const SizeType N) const noexcept { return this->FindNth(String, Encoding::GetStringLength(String), N); }
    FORCEINLINE ConstIterator FindNth(ConstPointer String, const SizeType Length, const SizeType N) const noexcept { return this->FindNth(String, String + Length, N); }
    FORCEINLINE ConstIterator FindNth(ConstPointer Begin, ConstPointer End, const SizeType N) const noexcept { return this->FindNth(ConstIterator{ Begin }, ConstIterator{ End }, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindNth(UIterator Begin, VIterator End, const SizeType N) const noexcept { return TStringBase::FindNth(this->begin(), this->end(), Begin, End, N); }
    FORCEINLINE ConstIterator FindNth(T Rune, const SizeType N) const noexcept { return TStringBase::FindNth(this->begin(), this->end(), Rune, N); }

    template <TIteratorConcept UIterator>
    FORCEINLINE static ConstIterator FindNth(UIterator Begin, const SizeType Length, ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Length, Other, Encoding::GetStringLength(Other), N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(UIterator Begin, const SizeType Length, VIterator OtherBegin, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Length, OtherBegin, OtherBegin + OtherLength, N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator>)
    FORCEINLINE static ConstIterator FindNth(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Begin + Length, OtherBegin, OtherEnd, N); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static ConstIterator FindNth(UIterator Begin, const SizeType Length, T Rune, const SizeType N) noexcept { return TStringBase::FindNth(Begin, Begin + Length, Rune, N); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static ConstIterator FindNth(UIterator Begin, VIterator End, ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNth(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                static UIterator FindNth(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd, SizeType N) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
                static UIterator FindNth(UIterator Begin, VIterator End, T Rune, SizeType N) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindLast(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindLast(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindLast(ConstPointer String) const noexcept { return this->FindLast(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindLast(TIteratorConcept auto String, const SizeType Length) const noexcept { return this->FindLast(String, String + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE ConstIterator FindLast(UIterator Begin, VIterator End) const noexcept { return TStringBase::FindLast(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindLast(T Rune) const noexcept { return TStringBase::FindLast(this->begin(), this->end(), Rune); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE i64 FindLastIndex(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { if (auto It { this->FindLast(Other) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    FORCEINLINE i64 FindLastIndex(ConstPointer String) const noexcept { if (auto It { this->FindLast(String) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    FORCEINLINE i64 FindLastIndex(TIteratorConcept auto String, const SizeType Length) const noexcept { if (auto It { this->FindLast(String, Length) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE i64 FindLastIndex(UIterator Begin, VIterator End) const noexcept { if (auto It { this->FindLast(Begin, End) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }
    FORCEINLINE i64 FindLastIndex(T Rune) const noexcept { if (auto It { this->FindLast(Rune) }; It == this->end()) { return INDEX_NONE; } else { return static_cast<i64>(this->ToIndex(It)); } }

    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::FindLast(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, const SizeType Length, ITERATOR OtherBegin, const SizeType OtherLength) noexcept requires(ITERATOR_CROSS(Begin, OtherBegin)) { return TStringBase::FindLast(Begin, Length, OtherBegin, OtherBegin + OtherLength); }
    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, const SizeType Length, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept requires(ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, Length)) { return TStringBase::FindLast(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, const SizeType Length, T Rune) noexcept { return TStringBase::FindLast(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, ITERATOR End, ConstPointer Other) noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindLast(ITERATOR Begin, ITERATOR End, ITERATOR Other, const SizeType OtherLength) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_CROSS(Begin, Other)) { return TStringBase::FindLast(Begin, End, Other, Other + OtherLength); }
                static ConstIterator FindLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, OtherBegin));
                static ConstIterator FindLast(ITERATOR Begin, ITERATOR End, T Rune) noexcept requires(ITERATOR_PAIR(Begin, End));

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindSecondLast(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->FindSecondLast(Other.begin(), Other.end()); }
    FORCEINLINE ConstIterator FindSecondLast(ConstPointer String) const noexcept { return this->FindSecondLast(String, Encoding::GetStringLength(String)); }
    FORCEINLINE ConstIterator FindSecondLast(ITERATOR String, const SizeType Length) const noexcept { return this->FindSecondLast(String, String + Length); }
    FORCEINLINE ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End) const noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindSecondLast(this->begin(), this->end(), Begin, End); }
    FORCEINLINE ConstIterator FindSecondLast(T Rune) const noexcept { return TStringBase::FindSecondLast(this->begin(), this->end(), Rune); }

    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::FindSecondLast(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, const SizeType Length, ITERATOR Other, const SizeType OtherLength) noexcept { return TStringBase::FindSecondLast(Begin, Length, Other, Other + OtherLength); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, const SizeType Length, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept { return TStringBase::FindSecondLast(Begin, Begin + Length, OtherBegin, OtherEnd); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, const SizeType Length, T Rune) noexcept { return TStringBase::FindSecondLast(Begin, Begin + Length, Rune); }

    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End, ConstPointer Other) noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindSecondLast(Begin, End, Other, Encoding::GetStringLength(Other)); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, const SizeType OtherLength) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_CROSS(Begin, OtherBegin)) { return TStringBase::FindSecondLast(Begin, End, OtherBegin, OtherBegin + OtherLength); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindSecondLast(Begin, End, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, OtherBegin)) { return TStringBase::FindNth(Begin, End, OtherBegin, OtherEnd, 2); }
    FORCEINLINE static ConstIterator FindSecondLast(ITERATOR Begin, ITERATOR End, T Rune) noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindNth(Begin, End, Rune, 2); }

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE ConstIterator FindNthLast(const TStringBase<TEncoding, UAllocator>& Other, const SizeType N) const noexcept { return this->FindNthLast(Other.begin(), Other.end(), N); }
    FORCEINLINE ConstIterator FindNthLast(ConstPointer String, const SizeType N) const noexcept { return this->FindNthLast(String, Encoding::GetStringLength(String), N); }
    FORCEINLINE ConstIterator FindNthLast(ITERATOR String, const SizeType Length, const SizeType N) const noexcept { return this->FindNthLast(String, String + Length, N); }
    FORCEINLINE ConstIterator FindNthLast(ITERATOR Begin, ITERATOR End, const SizeType N) const noexcept requires(ITERATOR_PAIR(Begin, End)) { return TStringBase::FindNthLast(this->begin(), this->end(), Begin, End, N); }
    FORCEINLINE ConstIterator FindNthLast(T Rune, const SizeType N) const noexcept { return TStringBase::FindNthLast(this->begin(), this->end(), Rune, N); }

    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, const SizeType Length, ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, const SizeType Length, ConstPointer Other, const SizeType OtherLength, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, Other, Other + OtherLength, N); }
    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, const SizeType Length, ConstPointer OtherBegin, ConstPointer OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }, N); }
    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, const SizeType Length, ITERATOR OtherBegin, ITERATOR OtherEnd, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Begin + Length, OtherBegin, OtherEnd, N); }
    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, const SizeType Length, T Rune, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, Begin + Length, Rune, N); }

    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, ITERATOR End, ConstPointer Other, const SizeType N) noexcept { return TStringBase::FindNthLast(Begin, End, Other, Encoding::GetStringLength(Other), N); }
    FORCEINLINE static ConstIterator FindNthLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, const SizeType OtherLength, const SizeType N) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_CROSS(Begin, OtherBegin)) { return TStringBase::FindNthLast(Begin, End, OtherBegin, OtherBegin + OtherLength), N; }
                static ConstIterator FindNthLast(ITERATOR Begin, ITERATOR End, ITERATOR OtherBegin, ITERATOR OtherEnd, SizeType N) noexcept requires(ITERATOR_PAIR(Begin, End) && ITERATOR_PAIR(OtherBegin, OtherEnd) && ITERATOR_CROSS(Begin, OtherBegin));
                static ConstIterator FindNthLast(ITERATOR Begin, ITERATOR End, T Rune, SizeType N) noexcept requires(ITERATOR_PAIR(Begin, End));

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE SizeType Count(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->Count(Other.begin(), Other.end()); }
    FORCEINLINE SizeType Count(ConstPointer String) const noexcept { return this->Count(String, String + Encoding::GetStringLength(String)); }
    template <typename UIterator>
    FORCEINLINE SizeType Count(UIterator String, const SizeType Length) const noexcept { return this->Count(String, String + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE SizeType Count(UIterator Begin, VIterator End) const noexcept { return TStringBase::Count(this->begin(), this->end(), Begin, End); }
    FORCEINLINE SizeType Count(T Rune) const noexcept { return TStringBase::Count(this->begin(), this->end(), Rune); }

    template <typename UIterator>
    FORCEINLINE static SizeType Count(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::Count(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <typename UIterator>
    FORCEINLINE static SizeType Count(UIterator Begin, const SizeType Length, ConstPointer Other, const SizeType OtherLength) noexcept { return TStringBase::Count(Begin, Length, Other, Other + OtherLength); }
    template <typename UIterator>
    FORCEINLINE static SizeType Count(UIterator Begin, const SizeType Length, ConstPointer OtherBegin, ConstPointer OtherEnd) noexcept { return TStringBase::Count(Begin, Length, ConstIterator{ OtherBegin }, ConstIterator{ OtherEnd }); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::Count(Begin, Begin + Length, OtherBegin, OtherEnd); }
    template <typename UIterator>
    FORCEINLINE static SizeType Count(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::Count(Begin, Begin + Length, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static SizeType Count(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::Count(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, VIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static SizeType Count(UIterator Begin, VIterator End, WIterator Other, const SizeType OtherLength) noexcept { return TStringBase::Count(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
                static SizeType Count(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
                static SizeType Count(UIterator Begin, VIterator End, T Rune) noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE bool Contains(const TStringBase<TEncoding, UAllocator>& Other) const noexcept { return this->Contains(Other.begin(), Other.end()); }
    FORCEINLINE bool Contains(ConstPointer String) const noexcept { return this->Contains(String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool Contains(UIterator Begin, VIterator End) const noexcept { return TStringBase::Contains(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool Contains(T Rune) const noexcept { return TStringBase::Contains(this->begin(), this->end(), Rune); }

    template <TIteratorConcept UIterator>
    FORCEINLINE static bool Contains(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::Contains(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, const SizeType Length, VIterator Other, const SizeType OtherLength) noexcept { return TStringBase::Contains(Begin, Length, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::Contains(Begin, Begin + Length, OtherBegin, OtherEnd); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool Contains(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::Contains(Begin, Begin + Length, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::Contains(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept { return TStringBase::FindFirst(Begin, End, OtherBegin, OtherEnd) != End; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool Contains(UIterator Begin, VIterator End, T Rune) noexcept { return TStringBase::FindFirst(Begin, End, Rune) != End; }

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
    FORCEINLINE bool ContainsBackward(ConstPointer String) const noexcept { return this->ContainsBackward(String, String + Encoding::GetStringLength(String)); }
    template <TIteratorConcept UIterator>
    FORCEINLINE bool ContainsBackward(UIterator Begin, const SizeType Length) const noexcept { return TStringBase::ContainsBackward(this->begin(), this->end(), Begin, Begin + Length); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE bool ContainsBackward(UIterator Begin, VIterator End) const noexcept { return TStringBase::ContainsBackward(this->begin(), this->end(), Begin, End); }
    FORCEINLINE bool ContainsBackward(T Rune) const noexcept { return TStringBase::ContainsBackward(this->begin(), this->end(), Rune); }

    template <TIteratorConcept UIterator>
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const SizeType Length, ConstPointer Other) noexcept { return TStringBase::ContainsBackward(Begin, Length, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const SizeType Length, VIterator Other, const SizeType OtherLength) noexcept { return TStringBase::ContainsBackward(Begin, Length, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator> && TCrossIteratorConcept<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const SizeType Length, VIterator OtherBegin, WIterator OtherEnd) noexcept { return TStringBase::ContainsBackward(Begin, Begin + Length, OtherBegin, OtherEnd); }
    template <TIteratorConcept UIterator>
    FORCEINLINE static bool ContainsBackward(UIterator Begin, const SizeType Length, T Rune) noexcept { return TStringBase::ContainsBackward(Begin, Begin + Length, Rune); }

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, VIterator End, ConstPointer Other) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Encoding::GetStringLength(Other)); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, VIterator End, WIterator Other, const SizeType OtherLength) noexcept { return TStringBase::ContainsBackward(Begin, End, Other, Other + OtherLength); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator, TIteratorConcept XIterator> requires(TIteratorPairConcept<UIterator, VIterator> && TIteratorPairConcept<WIterator, XIterator> && TCrossIteratorConcept<UIterator, WIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, VIterator End, WIterator OtherBegin, XIterator OtherEnd) noexcept { return TStringBase::FindLast(Begin, End, OtherBegin, OtherEnd) != End; }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE static bool ContainsBackward(UIterator Begin, VIterator End, T Rune) noexcept { return TStringBase::FindLast(Begin, End, Rune) != End; }

    //# Cuts the content to the specified locations.
                void InlineCut(const SizeType Begin, const SizeType End, const bool bAllowShrinking = true) noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void InlineCut(UIterator Begin, VIterator End, const bool bAllowShrinking = true) noexcept { this->InlineCut(Begin - this->begin(), End - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> Cut(const SizeType Begin, const SizeType End) const noexcept;
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TStringBaseAllocatorConcept UAllocator = Allocator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE TStringBase<TEncoding, UAllocator> Cut(UIterator Begin, VIterator End) const noexcept { return this->template Cut<UAllocator>(Begin - this->begin(), End - this->begin()); }

    //# Cuts the content at the specified exclusive location.
    FORCEINLINE void InlineLeftCut(const SizeType Index, const bool bAllowShrinking = true) noexcept;
    template <TIteratorConcept UIterator>
    FORCEINLINE void InlineLeftCut(UIterator It, const bool bAllowShrinking = true) noexcept { this->InlineLeftCut(It - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftCut(const SizeType Index) const noexcept;
    template <TIteratorConcept UIterator, TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> LeftCut(UIterator It) const noexcept { return this->template LeftCut<UAllocator>(It - this->begin()); }

    //# Cuts the content at the specified inclusive location.
    FORCEINLINE void InlineRightCut(const SizeType Index, const bool bAllowShrinking = true) noexcept;
    template <TIteratorConcept UIterator>
    FORCEINLINE void InlineRightCut(UIterator It, const bool bAllowShrinking = true) noexcept { this->InlineRightCut(It - this->begin(), bAllowShrinking); }
    template <TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightCut(const SizeType Index) const noexcept;
    template <TIteratorConcept UIterator, TStringBaseAllocatorConcept UAllocator = Allocator>
    FORCEINLINE TStringBase<TEncoding, UAllocator> RightCut(UIterator It) const noexcept { return this->template RightCut<UAllocator>(It - this->begin()); }

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
    FORCEINLINE SizeType GetLineNumber(ITERATOR It) const noexcept;

    template <typename TPredicate> requires(std::invocable<TPredicate, T> && std::is_void_v<std::invoke_result_t<TPredicate, T>>)
    void ForEach(const TPredicate& Predicate) noexcept requires(TAllocator::IsContentMutable());
    template <std::predicate<T> TPredicate>
    void ForEach(const TPredicate& Predicate) const noexcept;

    NODISCARD FORCEINLINE constexpr ConstPointer Peek() const noexcept;

    template <TStringBaseAllocatorConcept UAllocator>
    FORCEINLINE auto Push(this auto&& Self, const TStringBase<TEncoding, UAllocator>& Other) noexcept -> decltype(Self) requires(Allocator::IsAllowedToPushItems()) { Self.Push(Other.begin(), Other.end()); return std::forward<decltype(Self)>(Self); }
    FORCEINLINE auto Push(this auto&& Self, ConstPointer String) noexcept -> decltype(Self) requires(Allocator::IsAllowedToPushItems()) { Self.Push(String, String + Encoding::GetStringLength(String)); return std::forward<decltype(Self)>(Self); }
    template <TIteratorConcept UIterator>
    FORCEINLINE auto Push(this auto&& Self, UIterator Begin, const SizeType Length) noexcept -> decltype(Self) requires(Allocator::IsAllowedToPushItems()) { Self.Push(Begin, Begin + Length); return std::forward<decltype(Self)>(Self); }
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE auto Push(this auto&& Self, UIterator Begin, VIterator End) noexcept -> decltype(Self) requires(Allocator::IsAllowedToPushItems());
    FORCEINLINE auto Push(this auto&& Self, T Rune) noexcept -> decltype(Self) requires(Allocator::IsAllowedToPushItems()) { Self.Push(&Rune, &Rune + 1); return std::forward<decltype(Self)>(Self); }

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
    FORCEINLINE static TStringBase SprintF(ConstPointer Format, const TArgs&... Args) noexcept requires(TStringBase::IsOwningString());

    FORCEINLINE const Allocator& GetAllocator() const noexcept { return this->Impl; }
    FORCEINLINE Allocator& GetMutableAllocator() noexcept { return this->Impl; }

private:

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    void AssignImpl(UIterator Begin, VIterator End) noexcept requires(!TStringBase::IsStringView());

protected:

    void CreateInvariant() noexcept;
    void CreateInvariantWeak() noexcept;

#if LAL_CHECK_STRING_VALIDITY
    void EnsureInvariant() const;
#endif /* LAL_CHECK_STRING_VALIDITY */

    Allocator Impl;
};

} /* ~Namespace Lal */
