// Copyright mzoesch. All rights reserved.

#pragma once

#if CHECK_STRING_VALIDITY
    #define PRIVATE_JAFG_CHECK_STRING_STATE()   this->EnsureValidState();
    #define JAFG_CHECK_STRING(Expr)             jassert(Expr)
#else /* CHECK_STRING_VALIDITY */
    #define PRIVATE_JAFG_CHECK_STRING_STATE()
    #define JAFG_CHECK_STRING(Expr)
#endif /* !CHECK_STRING_VALIDITY */

namespace Jafg
{

/** Common traits. Define your own if you need to. These are the most basic UTF-8 traits. */
template <typename InT>
struct TStringTraits
{
    using T    = InT;
    using Self = TStringTraits<T>;

    inline static const T Terminator = '\0';

    template <typename SizeType>
    FORCEINLINE static SizeType GetStringLength(const T* InString) noexcept; // In Runes.

    template <typename SizeType>
    static SizeType GetCharacterCount(const T* InString) noexcept; // In Characters. But just basic UFT-8 stuff; So no compound shit.

    template <typename SizeType>
    static SizeType GetCharacterSize(const T* InCharacter) noexcept;

    template <typename SizeType>
    static void GoToMostRecentValidCharacter(const T* Self, SizeType* InOutCursor) noexcept;

    FORCEINLINE static T ToLower(const T InRune);
};

/**
 * The base class for all strings.
 */
template <typename InTraits, typename InAlloc>
class TStringBase
{
public:

    using Traits     = InTraits;
    using T          = typename Traits::T;
    using Alloc      = InAlloc;
    using Self       = TStringBase<Traits, Alloc>;
    using SizeType   = typename Alloc::SizeType;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>, "SizeType must be a signed integral type.");

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField);

    FORCEINLINE  TStringBase() noexcept = default;
    FORCEINLINE  TStringBase(LNullptrTy) noexcept : Impl() { }
    FORCEINLINE  TStringBase(const TStringBase& Other) noexcept = default;
    FORCEINLINE  TStringBase(TStringBase&& Other) noexcept = default;
    explicit     TStringBase(const T InRune);
    FORCEINLINE  TStringBase(const T* InString);
    FORCEINLINE  TStringBase(const T* InString, const SizeType InLength);
    FORCEINLINE ~TStringBase() noexcept = default;

    FORCEINLINE Self& operator=(LNullptrTy) noexcept { this->Empty();  return *this; }
    FORCEINLINE Self& operator=(const Self& InOther) noexcept = default;
    FORCEINLINE Self& operator=(Self&& InOther) noexcept = default;
    FORCEINLINE Self& operator=(const T InRune);
    FORCEINLINE Self& operator=(const T* InString);

    NODISCARD FORCEINLINE const T* ToC()   const noexcept { const T* Out = this->Impl.GetFirst(); return Out ? Out : &Traits::Terminator; }
    NODISCARD FORCEINLINE const T* ToPtr() const noexcept { const T* Out = this->Impl.GetFirst(); return Out ? Out : &Traits::Terminator; }
    NODISCARD FORCEINLINE SizeType GetRuneCount() const noexcept { return Maths::Max(this->GetSize() - /*Terminator*/1, 0); }
    NODISCARD FORCEINLINE SizeType GetCharacterCount() const noexcept { return  Traits::template GetCharacterCount<SizeType>(this->ToPtr()); }
    NODISCARD FORCEINLINE SizeType GetRuneCountOfCharacterAt(const SizeType InRuneIndex) const noexcept;

    NODISCARD FORCEINLINE SizeType GetSize()     const noexcept { return this->Impl.GetSize();      }
    NODISCARD FORCEINLINE SizeType GetByteSize() const noexcept { return this->Impl.GetByteSize();  }
    NODISCARD FORCEINLINE bool     IsEmpty()     const noexcept { return this->GetRuneCount() == 0; }

    FORCEINLINE bool IsValidIndex(const SizeType InRuneIndex) const noexcept { return InRuneIndex > INDEX_NONE && InRuneIndex < this->Impl.GetSize() - 1; }

    FORCEINLINE       T& operator[](const SizeType InRuneIndex)       noexcept { JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) ) return *(this->Impl.GetData() + InRuneIndex); }
    FORCEINLINE const T& operator[](const SizeType InRuneIndex) const noexcept { JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) ) return *(this->Impl.GetData() + InRuneIndex); }

    FORCEINLINE void Reserve(const SizeType InRuneCount) noexcept { this->Impl.Reserve(InRuneCount + /*Terminator*/1); }
    FORCEINLINE void Reset(const SizeType InRuneCount) noexcept { this->Impl.Reset(InRuneCount + /*Terminator*/1); }
    FORCEINLINE void Shrink() noexcept { this->Impl.Shrink(); }
    FORCEINLINE void Empty() noexcept { this->Impl.Empty(); }
    FORCEINLINE void SwapStrings(Self& InOther) noexcept { this->Impl.SwapBuffers(InOther.Impl); }

    FORCEINLINE bool operator==(LNullptrTy) const noexcept { return this->IsEmpty(); }
    FORCEINLINE bool operator==(const Self& InOther) const noexcept;
    FORCEINLINE bool operator==(const T* InString) const noexcept;
    FORCEINLINE bool operator!=(LNullptrTy) const noexcept { return !(*this == nullptr); }
    FORCEINLINE bool operator!=(const Self& InOther) const noexcept { return !(*this == InOther); }
    FORCEINLINE bool operator!=(const T* InString) const noexcept { return !(*this == InString); }
    FORCEINLINE bool Equals(LNullptrTy) const noexcept { return *this == nullptr; }
    FORCEINLINE bool Equals(const Self& InOther) const noexcept { return *this == InOther; }
    FORCEINLINE bool Equals(const T* InString) const noexcept { return *this == InString; }

    FORCEINLINE Self& operator+=(const Self& InOther) noexcept { this->Append(InOther); return *this; }
    FORCEINLINE Self& operator+=(const T InRune) noexcept { this->Add(InRune); return *this; }
    FORCEINLINE Self& operator+=(const T* InString) noexcept { this->Append(InString); return *this; }

    FORCEINLINE Self  operator /(const T InRune) const noexcept;
    FORCEINLINE Self  operator /(const T* InString) const noexcept;
    FORCEINLINE Self  operator /(const Self& InOther) const noexcept;
    FORCEINLINE Self& operator/=(const T InRune) noexcept { return this->AppendPath(InRune); }
    FORCEINLINE Self& operator/=(const T* InString) noexcept { return this->AppendPath(InString); }
    FORCEINLINE Self& operator/=(const Self& InOther) noexcept { return this->AppendPath(InOther); }
    FORCEINLINE Self& AppendPath(const T InRune) noexcept;
    FORCEINLINE Self& AppendPath(const T* InString) noexcept;
    FORCEINLINE Self& AppendPath(const T* InString, const SizeType InLength) noexcept;
    FORCEINLINE Self& AppendPath(const Self& InOther) noexcept;
    FORCEINLINE Self& AppendPath(const Self& InOther, const SizeType InLength) noexcept;

    FORCEINLINE SizeType Add(const T InRune) noexcept;
    FORCEINLINE void     AddAt(const SizeType InRuneIndex, const T InRune) noexcept;

    FORCEINLINE void Append(const Self& InOther) noexcept;
    FORCEINLINE void Append(const T* InString) noexcept;
    FORCEINLINE void Append(const T* InString, const SizeType InLength) noexcept;
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const Self& InOther) noexcept;
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const T* InString) noexcept;
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const T* InString, const SizeType InLength) noexcept;

    FORCEINLINE void RemoveAt(const SizeType InRuneIndex) noexcept;
    FORCEINLINE void RemoveAt(const SizeType InRuneIndex, const SizeType InLength) noexcept;
    //# @return The number of runes removed.
    FORCEINLINE SizeType RemoveCharacterAt(const SizeType InRuneIndex) noexcept;
    FORCEINLINE SizeType RemoveCharacterAt(const SizeType InRuneIndex, const SizeType InCharacters) noexcept;

    FORCEINLINE const T* Peek() const noexcept;
    FORCEINLINE bool Pop() noexcept;
    FORCEINLINE bool PopCharacter() noexcept;
    FORCEINLINE void Pop(const SizeType InRuneCount) noexcept;
    FORCEINLINE void PopCharacter(SizeType InCharacterCount) noexcept;

    FORCEINLINE bool StartsWith(const T InRune) const noexcept;
    FORCEINLINE bool StartsWith(const T* InString) const noexcept;
    FORCEINLINE bool StartsWith(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool StartsWith(const Self& InOther) const noexcept;
    FORCEINLINE bool StartsWith(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE bool EndsWith(const T InRune) const noexcept;
    FORCEINLINE bool EndsWith(const T* InString) const noexcept;
    FORCEINLINE bool EndsWith(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool EndsWith(const Self& InOther) const noexcept;
    FORCEINLINE bool EndsWith(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE SizeType FindFirst(const T InRune) const noexcept;
    FORCEINLINE SizeType FindFirst(const T* InString) const noexcept;
    FORCEINLINE SizeType FindFirst(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindFirst(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindFirst(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindSecond(const T InRune) const noexcept;
    FORCEINLINE SizeType FindSecond(const T* InString) const noexcept;
    FORCEINLINE SizeType FindSecond(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindSecond(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindSecond(const Self& InOther, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindLast(const T InRune) const noexcept;
    FORCEINLINE SizeType FindLast(const T* InString) const noexcept;
    FORCEINLINE SizeType FindLast(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType FindLast(const Self& InOther) const noexcept;
    FORCEINLINE SizeType FindLast(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE bool Contains(const T InRune) const noexcept;
    FORCEINLINE bool Contains(const T* InString) const noexcept;
    FORCEINLINE bool Contains(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE bool Contains(const Self& InOther) const noexcept;
    FORCEINLINE bool Contains(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE void InlineCut(const SizeType InRuneIndex) noexcept;
    FORCEINLINE Self Cut(const SizeType InRuneIndex) const noexcept;

    FORCEINLINE void InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount) noexcept;
    FORCEINLINE void InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) noexcept;
    FORCEINLINE Self Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept;
    FORCEINLINE Self SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept;

    FORCEINLINE void InlineRightChop(const SizeType InRuneIndex) noexcept;
    FORCEINLINE Self RightChop(const SizeType InRuneIndex) const noexcept;
    FORCEINLINE void InlineLeftChop(const SizeType InRuneIndex) noexcept;
    FORCEINLINE Self LeftChop(const SizeType InRuneIndex) const noexcept;

    FORCEINLINE SizeType Replace(const T InRune, const T InReplacement) noexcept;
    FORCEINLINE SizeType Replace(const T* InString, const T* InReplacement) noexcept;

    FORCEINLINE SizeType Count(const T InRune) const noexcept;
    FORCEINLINE SizeType Count(const T* InString) const noexcept;
    FORCEINLINE SizeType Count(const T* InString, const SizeType InLength) const noexcept;
    FORCEINLINE SizeType Count(const Self& InOther) const noexcept;
    FORCEINLINE SizeType Count(const Self& InOther, const SizeType InLength) const noexcept;

    FORCEINLINE void ToLower() noexcept;
    FORCEINLINE Self GetLowerCase() const noexcept;

    template <typename ... TArgs>
    static Self SprintF(const T* InFormat, const TArgs& ... InArgs) noexcept;

    FORCEINLINE       T* GetBegin()       noexcept { return this->Impl.GetData(); }
    FORCEINLINE const T* GetBegin() const noexcept { return this->Impl.GetData(); }
    FORCEINLINE       T* GetEnd()         noexcept { return this->Impl.GetSlack(); }
    FORCEINLINE const T* GetEnd()   const noexcept { return this->Impl.GetSlack(); }

    FORCEINLINE Iterator<T>       begin()       noexcept { return this->Impl.begin(); }
    FORCEINLINE Iterator<const T> begin() const noexcept { return this->Impl.begin(); }
    FORCEINLINE Iterator<const T> end()   const noexcept { auto It = this->Impl.end(); return It == this->begin() ? It : --It; }
    FORCEINLINE Iterator<T>       end()         noexcept { auto It = this->Impl.end(); return It == this->begin() ? It : --It; }

    FORCEINLINE       Alloc& GetUnderlyingDataStructure()       noexcept { return this->Impl; }
    FORCEINLINE const Alloc& GetUnderlyingDataStructure() const noexcept { return this->Impl; }

private:

#if CHECK_STRING_VALIDITY
    void EnsureValidState() const;
#endif /* CHECK_STRING_VALIDITY */

    Alloc Impl;
};

template<typename InT>
template<typename SizeType>
FORCEINLINE SizeType TStringTraits<InT>::GetStringLength(const T* InString) noexcept
{
    SizeType Out = 0;
    while (*InString++ != Self::Terminator)
    {
        ++Out;
    }

    return Out;
}

template<typename InT>
template<typename SizeType>
SizeType TStringTraits<InT>::GetCharacterCount(const T* InString) noexcept
{
    SizeType Out = 0;

    const auto GetSize = [](const T InFirstByte)
    {
        if ((InFirstByte & 0x80) == 0)
        { /* ASCII byte. */
            return 1;
        }

        if ((InFirstByte & 0xE0) == 0xC0)
        { /* 2-byte sequence. */
            return 2;
        }

        if ((InFirstByte & 0xF0) == 0xE0)
        { /* 3-byte sequence. */
            return 3;
        }

        if ((InFirstByte & 0xF8) == 0xF0)
        { /* 4-byte sequence. */
            return 4;
        }

        checkNoEntry()
        /* Invalid UTF-8. */
        // Return 1; So we do not hang indefinitely.
        return 1;
    };

    while (*InString != Self::Terminator)
    {
        ++Out;
        InString += GetSize(*InString);

        continue;
    }

    return Out;
}

template<typename InT>
template<typename SizeType>
FORCEINLINE SizeType TStringTraits<InT>::GetCharacterSize(const T* InCharacter) noexcept
{
    if ((*InCharacter & 0x80) == 0)
    { /* ASCII byte. */
        return 1;
    }

    if ((*InCharacter & 0xE0) == 0xC0)
    { /* 2-byte sequence. */
        return 2;
    }

    if ((*InCharacter & 0xF0) == 0xE0)
    { /* 3-byte sequence. */
        return 3;
    }

    if ((*InCharacter & 0xF8) == 0xF0)
    { /* 4-byte sequence. */
        return 4;
    }

    checkNoEntry()
    /* Invalid UTF-8. */
    // Return 1; So we do not hang indefinitely.
    return 1;
}

template<typename InT>
template<typename SizeType>
void TStringTraits<InT>::GoToMostRecentValidCharacter(const T* Self, SizeType* InOutCursor) noexcept
{
    while (*InOutCursor > INDEX_NONE)
    {
        if
        (
               ((*(Self + *InOutCursor) & 0x80) == 0x00) // ASCII byte. (0xxx xxxx)
            || ((*(Self + *InOutCursor) & 0xC0) == 0xC0) // Multi-byte. (11xx xxxx)
        )
        {
            break;
        }

        --*InOutCursor;
    }

    return;
}

template<typename InT>
FORCEINLINE typename TStringTraits<InT>::T TStringTraits<InT>::ToLower(const T InRune)
{
    if (InRune >= 'A' && InRune <= 'Z')
    {
        return InRune + 32;
    }

    return InRune;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<InTraits, InAlloc>::TStringBase(const T InRune) : Impl()
{
    this->Reserve(1);
    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<InTraits, InAlloc>::TStringBase(const T* InString) : Impl()
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    this->Reserve(Length);

    this->Impl.Append(InString, Length + /*Terminator*/1);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE TStringBase<InTraits, InAlloc>::TStringBase(const T* InString, const SizeType InLength) : Impl()
{
    this->Reserve(InLength);

    this->Impl.Append(InString, InLength);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::operator=(const T InRune)
{
    this->Reset(1);

    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return *this;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::operator=(const T* InString)
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    this->Reset(Length);

    this->Impl.Append(InString, Length + /*Terminator*/1);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return *this;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::GetRuneCountOfCharacterAt(const SizeType InRuneIndex) const noexcept
{
    return Traits::template GetCharacterSize<SizeType>(this->ToPtr() + InRuneIndex);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::operator==(const Self& InOther) const noexcept
{
    if (this->IsEmpty())
    {
        return InOther.IsEmpty();
    }

    return this->Impl.IsDataEqual(InOther.Impl);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::operator==(const T* InString) const noexcept
{
    if (this->IsEmpty())
    {
        return InString == nullptr || *InString == Traits::Terminator;
    }

    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (*(this->Impl.GetData() + Index) != *(InString + Index))
        {
            return false;
        }

        continue;
    }

    return true;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::operator/(const T InRune) const noexcept
{
    Self Out;
    if (*this->Peek() == '/')
    {
        Out.Reserve(this->GetRuneCount() + 1);
        Out = *this;
        Out.Add(InRune);
    }
    else
    {
        Out.Reserve(this->GetRuneCount() + 2);
        Out = *this;
        Out.Add('/');
        Out.Add(InRune);
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::operator/(const T* InString) const noexcept
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    Self Out;
    if (*this->Peek() == '/')
    {
        Out.Reserve(this->GetRuneCount() + Length);
        Out = *this;
        Out.Append(InString, Length);
    }
    else
    {
        Out.Reserve(this->GetRuneCount() + Length + 1);
        Out = *this;
        Out.Add('/');
        Out.Append(InString, Length);
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::operator/(const Self& InOther) const noexcept
{
    return this->operator/(InOther.ToPtr());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::AppendPath(const T InRune) noexcept
{
    if (*this->Peek() == '/')
    {
        this->Add(InRune);
    }
    else
    {
        if (this->IsEmpty() == false)
        {
            this->Reserve(2);
            this->Add('/');
        }
        this->Add(InRune);
    }

    return *this;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::AppendPath(const T* InString) noexcept
{
    return this->AppendPath(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::AppendPath(const T* InString, const SizeType InLength) noexcept
{
    if (*this->Peek() == '/')
    {
        this->Append(InString, InLength);
    }
    else
    {
        if (this->IsEmpty() == false)
        {
            this->Reserve(InLength + 1);
            this->Add('/');
        }
        this->Append(InString, InLength);
    }

    return *this;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::AppendPath(const Self& InOther) noexcept
{
    return this->AppendPath(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self& TStringBase<InTraits, InAlloc>::AppendPath(const Self& InOther, const SizeType InLength) noexcept
{
    return this->AppendPath(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Add(const T InRune) noexcept
{
    this->Impl.Pop(); /* Terminator */
    this->Impl.Add(InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->GetRuneCount();
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::AddAt(const SizeType InRuneIndex, const T InRune) noexcept
{
    this->Impl.Pop(); /* Terminator */
    this->Impl.AddAt(InRuneIndex, InRune);
    this->Impl.Add(Traits::Terminator);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::Append(const Self& InOther) noexcept
{
    this->Impl.Pop(); /* Terminator */
    this->Impl.Append(InOther.Impl);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::Append(const T* InString) noexcept
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);
    if (Length > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.Append(InString, Length + /*Terminator*/1);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::Append(const T* InString, const SizeType InLength) noexcept
{
    if (InLength > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.Append(InString, InLength);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const Self& InOther) noexcept
{
    if (const SizeType OtherRuneCount = InOther.GetRuneCount(); OtherRuneCount > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InOther.ToPtr(), OtherRuneCount);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const T* InString) noexcept
{
    const SizeType Length = Traits::template GetStringLength<SizeType>(InString);

    if (Length > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InString, Length);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::AppendAt(const SizeType InRuneIndex, const T* InString, const SizeType InLength) noexcept
{
    if (InLength > 0)
    {
        this->Impl.Pop(); /* Terminator */
        this->Impl.AppendAt(InRuneIndex, InString, InLength);
        this->Impl.Add(Traits::Terminator);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::RemoveAt(const SizeType InRuneIndex) noexcept
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    this->Impl.RemoveAt(InRuneIndex);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::RemoveAt(const SizeType InRuneIndex, const SizeType InLength) noexcept
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex + InLength - 1) )
    this->Impl.RemoveAt(InRuneIndex, InLength);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::RemoveCharacterAt(const SizeType InRuneIndex) noexcept
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )
    const SizeType CharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + InRuneIndex);
    this->Impl.RemoveAt(InRuneIndex, CharacterSize);

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return CharacterSize;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::RemoveCharacterAt(const SizeType InRuneIndex, const SizeType InCharacters) noexcept
{
    JAFG_CHECK_STRING( this->IsValidIndex(InRuneIndex) )

    SizeType Cursor = InRuneIndex;
    SizeType TotalRunes = 0;
    for (SizeType Character = 0; Character < InCharacters; ++Character)
    {
        SizeType CharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + Cursor);
        TotalRunes += CharacterSize;
        Cursor += CharacterSize;
    }

    this->RemoveAt(InRuneIndex, TotalRunes);

    return TotalRunes;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE const typename TStringBase<InTraits, InAlloc>::T* TStringBase<InTraits, InAlloc>::Peek() const noexcept
{
    PRIVATE_JAFG_CHECK_STRING_STATE()
    return this->Impl.GetSize() > 1 ? this->Impl.GetData() + this->GetRuneCount() - 1 : &Traits::Terminator;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Pop() noexcept
{
    const bool bOut = this->GetSize() > 1;

    this->Impl.Pop(2); /* Terminator */

    if (this->GetSize() > 0)
    {
        this->Impl.Add(Traits::Terminator);
    }
    else
    {
        this->Empty();
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return bOut;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::PopCharacter() noexcept
{
    const bool bOut = this->GetSize() > 1;

    this->Impl.Pop();
    SizeType Cursor = this->GetSize() - 1;
    Traits::GoToMostRecentValidCharacter(this->Impl.GetData(), &Cursor);
    Traits::GoToMostRecentValidCharacter(this->Impl.GetData(), &--Cursor);

    if (Cursor > INDEX_NONE)
    {
        SizeType CurCharacterSize = Traits::template GetCharacterSize<SizeType>(this->Impl.GetData() + Cursor);
        SizeType NewStrSize = Cursor + CurCharacterSize;
        this->Impl.Pop(this->GetSize() - NewStrSize - 1); /* Terminator */
        this->Impl[NewStrSize] = Traits::Terminator;
    }
    else
    {
        this->Impl.Empty();
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()

    return bOut;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::Pop(const SizeType InRuneCount) noexcept
{
    this->Impl.Pop(InRuneCount + 1); /* Terminator */

    if (this->GetSize() > 0)
    {
        this->Impl.Add(Traits::Terminator);
    }
    else
    {
        this->Empty();
    }

    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::PopCharacter(SizeType InCharacterCount) noexcept
{
    while (InCharacterCount > 0)
    {
        if (this->PopCharacter())
        {
            --InCharacterCount;
            continue;
        }

        break;
    }

    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::StartsWith(const T InRune) const noexcept
{
    return *this->ToPtr() == InRune;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::StartsWith(const T* InString) const noexcept
{
    return this->StartsWith(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::StartsWith(const T* InString, const SizeType InLength) const noexcept
{
    if (*InString == Traits::Terminator)
    {
        return this->StartsWith(Traits::Terminator);
    }

    SizeType Cursor = 0;
    while (*InString != Traits::Terminator && Cursor < InLength)
    {
        if (this->GetRuneCount() <= Cursor)
        {
            return false;
        }

        if (*(this->Impl.GetData() + Cursor++) != *InString)
        {
            return false;
        }

        ++InString;

        continue;
    }

    return Cursor <= this->GetRuneCount();}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::StartsWith(const Self& InOther) const noexcept
{
    return this->StartsWith(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::StartsWith(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->StartsWith(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::EndsWith(const T InRune) const noexcept
{
    return *this->Peek() == InRune;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::EndsWith(const T* InString) const noexcept
{
    return this->EndsWith(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::EndsWith(const T* InString, const SizeType InLength) const noexcept
{
    SizeType Cursor = 0;
    const SizeType StringSize = this->GetSize();
    while (Cursor < StringSize)
    {
        if (*(this->Impl.GetSlack() - 1 - Cursor) != *(InString + InLength - Cursor))
        {
            return false;
        }

        if (Cursor == InLength)
        {
            return true;
        }

        ++Cursor;

        continue;
    }

    return Cursor == StringSize;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::EndsWith(const Self& InOther) const noexcept
{
    return this->EndsWith(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::EndsWith(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->EndsWith(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindFirst(const T InRune) const noexcept
{
    const SizeType Size = this->GetSize();
    for (SizeType Index = 0; Index < Size; ++Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindFirst(const T* InString) const noexcept
{
    return this->FindFirst(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindFirst(const T* InString, const SizeType InLength) const noexcept
{
    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return INDEX_NONE;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                return MainCursor;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindFirst(const Self& InOther) const noexcept
{
    return this->FindFirst(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindFirst(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindFirst(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindSecond(const T InRune) const noexcept
{
    bool bFound = false;

    const SizeType Size = this->GetSize();
    for (SizeType Index = 0; Index < Size; ++Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            if (bFound)
            {
                return Index;
            }
            else
            {
                bFound = true;
            }
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindSecond(const T* InString) const noexcept
{
    return this->FindSecond(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindSecond(const T* InString, const SizeType InLength) const noexcept
{
    bool bFound = false;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return INDEX_NONE;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                if (bFound)
                {
                    return MainCursor;
                }
                else
                {
                    bFound = true;
                }
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindSecond(const Self& InOther) const noexcept
{
    return this->FindSecond(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindSecond(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindSecond(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindLast(const T InRune) const noexcept
{
    for (SizeType Index = this->GetSize() - 1; Index > INDEX_NONE; --Index)
    {
        if (*(this->Impl.GetData() + Index) == InRune)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindLast(const T* InString) const noexcept
{
    return this->FindLast(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindLast(const T* InString, const SizeType InLength) const noexcept
{
    // This is not efficient at all, so if we use this frequently in larger strings,
    // we must revisit this.
    // Currently only very sparingly used in the codebase - so it doesn't fucking matter.

    SizeType Last = INDEX_NONE;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return Last;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                Last = MainCursor;
                break;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return Last;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindLast(const Self& InOther) const noexcept
{
    return this->FindLast(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::FindLast(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->FindLast(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Contains(const T InRune) const noexcept
{
    return this->FindFirst(InRune) != INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Contains(const T* InString) const noexcept
{
    return this->Contains(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Contains(const T* InString, const SizeType InLength) const noexcept
{
    return this->FindFirst(InString, InLength) != INDEX_NONE;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Contains(const Self& InOther) const noexcept
{
    return this->Contains(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE bool TStringBase<InTraits, InAlloc>::Contains(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->Contains(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::InlineCut(const SizeType InRuneIndex) noexcept
{
    this->Impl.Resize(InRuneIndex + /*Terminator*/1, true);
    this->Impl[InRuneIndex] = Traits::Terminator;

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::Cut(const SizeType InRuneIndex) const noexcept
{
    Self Out;
    Out.Impl.CopyFrom(this->Impl, InRuneIndex + 1);
    Out.Impl[InRuneIndex] = Traits::Terminator;

#if CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */
    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount) noexcept
{
    *this = this->Sub(InRuneStartIndex, InRuneCount);
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) noexcept
{
    this->InlineSub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const noexcept
{
    JAFG_CHECK_STRING( InRuneCount > INDEX_NONE )

    Self Out;

    if (InRuneCount > 0)
    {
        Out.Reserve(InRuneCount + /*Terminator*/1);
        Out.Impl.CopyFrom(this->Impl, InRuneStartIndex, InRuneCount);
        Out.Impl.Add(Traits::Terminator);
    }

#if CHECK_STRING_VALIDITY
    Out.EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */
    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const noexcept
{
    return this->Sub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::InlineRightChop(const SizeType InRuneIndex) noexcept
{
    this->InlineSub(InRuneIndex, this->GetRuneCount() - InRuneIndex);
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::RightChop(const SizeType InRuneIndex) const noexcept
{
    return this->Sub(InRuneIndex, this->GetRuneCount() - InRuneIndex);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::InlineLeftChop(const SizeType InRuneIndex) noexcept
{
    this->InlineCut(InRuneIndex);
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::LeftChop(const SizeType InRuneIndex) const noexcept
{
    return this->Cut(InRuneIndex);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Replace(const T InRune, const T InReplacement) noexcept
{
    SizeType Out = 0;

    for (T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if (*Bulk == InRune)
        {
            *Bulk = InReplacement;
            ++Out;
        }

        continue;
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Replace(const T* InString, const T* InReplacement) noexcept
{
    SizeType Out = 0;

    while (true)
    {
        const SizeType Where = this->FindFirst(InString);
        if (Where == INDEX_NONE)
        {
            break;
        }

        this->RemoveAt(Where, Traits::template GetStringLength<SizeType>(InString));
        this->AppendAt(Where, InReplacement);
        ++Out;

        continue;
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Count(const T InRune) const noexcept
{
    SizeType Out = 0;

    for (const T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if (*Bulk == InRune)
        {
            ++Out;
        }

        continue;
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Count(const T* InString) const noexcept
{
    return this->Count(InString, Traits::template GetStringLength<SizeType>(InString));
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Count(const T* InString, const SizeType InLength) const noexcept
{
    SizeType Out = 0;

    SizeType MainCursor = 0;
    SizeType MainStringSize = this->GetSize();
    while (MainCursor < MainStringSize)
    {
        SizeType SubCursor  = 0;

        while (SubCursor < InLength)
        {
            JAFG_CHECK_STRING(*(InString + SubCursor) != Traits::Terminator)

            if (SubCursor == MainStringSize)
            {
                return Out;
            }

            if (*(this->Impl.GetData() + MainCursor + SubCursor) != *(InString + SubCursor))
            {
                break;
            }

            ++SubCursor;
            if (SubCursor == InLength)
            {
                ++Out;
                break;
            }

            continue;
        }

        ++MainCursor;
        continue;
    }

    return Out;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Count(const Self& InOther) const noexcept
{
    return this->Count(InOther.ToPtr(), InOther.GetRuneCount());
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::SizeType TStringBase<InTraits, InAlloc>::Count(const Self& InOther, const SizeType InLength) const noexcept
{
    return this->Count(InOther.ToPtr(), InLength);
}

template<typename InTraits, typename InAlloc>
FORCEINLINE void TStringBase<InTraits, InAlloc>::ToLower() noexcept
{
    for (T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        *Bulk = Traits::ToLower(*Bulk);
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return;
}

template<typename InTraits, typename InAlloc>
FORCEINLINE typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::GetLowerCase() const noexcept
{
    Self Out; Out.Reserve(this->GetSize());

    for (const T* RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        Out.Impl.Add(Traits::ToLower(*Bulk));
    }

    PRIVATE_JAFG_CHECK_STRING_STATE()
    return Out;
}

template<typename InTraits, typename InAlloc>
template<typename ... TArgs>
typename TStringBase<InTraits, InAlloc>::Self TStringBase<InTraits, InAlloc>::SprintF(const T* InFormat, const TArgs&... InArgs) noexcept
{
    /**
     * Super supid solution. But who cares right now.
     * Later we write our own implementation with type safety etc.
     *
     * S will be stackallocated if in str is small enough. So its not that bad.
     * But still one unnecessary heap allocation by this class...
     */
    const LStringLegacy S = std::vformat(InFormat, std::make_format_args(InArgs...));
    Self Out = S.c_str();
    return Out;
}

#if CHECK_STRING_VALIDITY
template<typename InTraits, typename InAlloc>
void TStringBase<InTraits, InAlloc>::EnsureValidState() const
{
    for (const T *RESTRICT Bulk = this->Impl.GetData(), *RESTRICT BulkEnd = this->Impl.GetSlack(); Bulk != BulkEnd; ++Bulk)
    {
        if (*Bulk == Traits::Terminator && Bulk + 1 != BulkEnd)
        {
            panic("String contains a null terminator but is not the last character.")
        }

        continue;
    }

    if (this->Impl.IsEmpty() == false)
    {
        if (*this->Impl.Peek() != Traits::Terminator)
        {
            panic("String does not end with a null terminator.")
        }
    }

    return;
}
#endif /* CHECK_STRING_VALIDITY */

} /* ~Namespace Jafg */

#undef PRIVATE_JAFG_CHECK_STRING_STATE
#undef JAFG_CHECK_STRING

template <>
struct std::formatter<::Jafg::LString> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::LString& InString,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<const char*>::format(InString.ToC(), InContext);
    }
};
