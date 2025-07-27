// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::Legacy
{

namespace ELiteralEncoding
{

    enum Type
    {
        Ascii,
        Utf8,
        Utf16,
        Utf32
    };

} /* ~Namespace ELiteralEncoding */

struct LStringTraitsBase;

template <typename InCharacterTy, class InTraitsTy>
class LStringBase;

struct LStringTraitsBase
{
};

template <class InCharacterTy, class InSizeType>
struct LStringTraits : public LStringTraitsBase
{
    static_assert(std::is_signed_v<InSizeType>, "InSizeType must be signed.");

    using RuneType = InCharacterTy;
    using SizeType = InSizeType;

    inline static RuneType Terminator = '\0';

    /** Whether the first rune of #Self is equal of the first rune of #InOther. */
    template <ELiteralEncoding::Type InOtherEncodingTy>
    static bool IsRuneEqual(const RuneType* Self, const RuneType* InOther) noexcept;

    /** The number of characters. Excluding terminator. */
    static SizeType GetCharacterLength(const RuneType* InString) noexcept;
    static SizeType GetRuneCount(const RuneType* Self) noexcept;

    static const RuneType* GetLastRunePointer(const RuneType* Self, const SizeType InCharacterSize) noexcept;
    static const RuneType* GetRuneAt(const RuneType* Self, const SizeType InRuneIndex) noexcept;

    static auto GetRuneSize(const RuneType* InRuneToCheck) noexcept -> SizeType;

    static void GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept;
    static void GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept;
    static void GoToPreviousRune(const RuneType* Self, SizeType& InOutCursor) noexcept;

    static void CopyRuneOfEqualSize(RuneType* Self, const RuneType* InOther) noexcept;

    static void ToLowerCase(RuneType* InRuneToCheck) noexcept;
};

template <class InCharacterTy, class InSizetype>
template <ELiteralEncoding::Type InOtherEncodingTy>
bool LStringTraits<InCharacterTy, InSizetype>::IsRuneEqual(const RuneType* Self, const RuneType* InOther) noexcept
{
    return *Self == *InOther;
}

template <class InCharacterTy, class InSizetype>
typename LStringTraits<InCharacterTy, InSizetype>::SizeType
LStringTraits<InCharacterTy, InSizetype>::GetCharacterLength(const RuneType* InString) noexcept
{
    SizeType Out = 0;
    while (*InString++ != Terminator)
    {
        ++Out;
    }

    return Out;
}

template <class InCharacterTy, class InSizetype>
typename LStringTraits<InCharacterTy, InSizetype>::SizeType LStringTraits<InCharacterTy, InSizetype>::GetRuneCount(const RuneType* Self) noexcept
{
    SizeType Out = 0;
    while (*Self++ != Terminator)
    {
        ++Out;
    }

    return Out;
}

template <class InCharacterTy, class InSizetype>
const typename LStringTraits<InCharacterTy, InSizetype>::RuneType*
LStringTraits<InCharacterTy, InSizetype>::GetLastRunePointer(const RuneType* Self, const SizeType InCharacterSize) noexcept
{
    return Self + InCharacterSize - 2; /* Null terminator. */
}

template <class InCharacterTy, class InSizetype>
const typename LStringTraits<InCharacterTy, InSizetype>::RuneType*
LStringTraits<InCharacterTy, InSizetype>::GetRuneAt(const RuneType* Self, const SizeType InRuneIndex) noexcept
{
    return Self + InRuneIndex;
}

template <class InCharacterTy, class InSizetype>
typename LStringTraits<InCharacterTy, InSizetype>::SizeType
LStringTraits<InCharacterTy, InSizetype>::GetRuneSize(const RuneType* InRuneToCheck) noexcept
{
    return 1;
}

template <class InCharacterTy, class InSizetype>
void LStringTraits<InCharacterTy, InSizetype>::GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept
{
    OutCursor = InRuneIndex;
}

template <class InCharacterTy, class InSizetype>
void LStringTraits<InCharacterTy, InSizetype>::GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept
{
    ++InOutCursor;
}

template <class InCharacterTy, class InSizetype>
void LStringTraits<InCharacterTy, InSizetype>::GoToPreviousRune(const RuneType* Self, SizeType& InOutCursor) noexcept
{
    --InOutCursor;
}

template <class InCharacterTy, class InSizetype>
void LStringTraits<InCharacterTy, InSizetype>::CopyRuneOfEqualSize(RuneType* Self, const RuneType* InOther) noexcept
{
    *Self = *InOther;
}

template <class InCharacterTy, class InSizeType>
void LStringTraits<InCharacterTy, InSizeType>::ToLowerCase(RuneType* InRuneToCheck) noexcept
{
    if (*InRuneToCheck >= 'A' && *InRuneToCheck <= 'Z')
    {
        *InRuneToCheck += 32;
    }

    return;
}

struct LAsciiStringTraits : public LStringTraits<char, DefaultContainerSizeType>
{
    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Ascii; }
    static constexpr bool bTriviallyConvertibleToUtf8 = true;
};

struct LEightStringTraits : public LStringTraits<char, DefaultContainerSizeType>
{
    using CharType = char;
    using SizeType = DefaultContainerSizeType;

    inline static char Terminator = '\0';
    // inline static char8_t Terminator = u8'\0';

    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Utf8; }
    static constexpr bool bTriviallyConvertibleToUtf8 = true;

    template <ELiteralEncoding::Type InOtherEncodingTy>
    static bool IsRuneEqual(const RuneType* Self, const RuneType* InOther) noexcept;

    static SizeType GetRuneCount(const RuneType* Self) noexcept;

    static SizeType GetRuneSize(const RuneType* InRuneToCheck) noexcept;

    static void GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept;
    static void GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept;
    static void GoToPreviousRune(const RuneType* Self, SizeType& InOutCursor) noexcept;

    static void CopyRuneOfEqualSize(RuneType* Self, const RuneType* InOther) noexcept;
};

template <ELiteralEncoding::Type InOtherEncodingTy>
bool LEightStringTraits::IsRuneEqual(const RuneType* Self, const RuneType* InOther) noexcept
{
    if constexpr (InOtherEncodingTy == ELiteralEncoding::Utf8)
    {
        const SizeType SelfRuneSize  = GetRuneSize(Self);
        const SizeType OtherRuneSize = GetRuneSize(InOther);

        if (SelfRuneSize != OtherRuneSize)
        {
            return false;
        }

        SizeType Cursor = 0;
        while (Cursor < SelfRuneSize)
        {
            if (Self[Cursor] != InOther[Cursor])
            {
                return false;
            }

            ++Cursor;
        }

        return true;
    }

    static_assert(InOtherEncodingTy == ELiteralEncoding::Utf8);
    return false;
}

inline LEightStringTraits::SizeType LEightStringTraits::GetRuneCount(const RuneType* Self) noexcept
{
    SizeType Out = 0;
    SizeType Cursor = 0;
    while (Self[Cursor] != Terminator)
    {
        GoToNextRune(Self, Cursor);
        ++Out;
    }

    return Out;
}

inline LStringTraits<char, i32>::SizeType LEightStringTraits::GetRuneSize(const RuneType* InRuneToCheck) noexcept
{
    const unsigned char* Bytes = reinterpret_cast<const unsigned char*>(InRuneToCheck);

    if ((*Bytes & 0x80) == 0)
    { /* ASCII byte. */
        return 1;
    }

    if ((*Bytes & 0xE0) == 0xC0)
    { /* 2-byte sequence. */
        return 2;
    }

    if ((*Bytes & 0xF0) == 0xE0)
    { /* 3-byte sequence. */
        return 3;
    }

    if ((*Bytes & 0xF8) == 0xF0)
    { /* 4-byte sequence. */
        return 4;
    }

    /* Invalid UTF-8. */
    return -1;
}

inline void LEightStringTraits::GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept
{
    OutCursor = 0;

    SizeType Iterations = 0;
    while (Iterations < InRuneIndex)
    {
        GoToNextRune(Self, OutCursor);
        ++Iterations;
    }

    return;
}

inline void LEightStringTraits::GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept
{
    InOutCursor += GetRuneSize(Self + InOutCursor);
}

inline void LEightStringTraits::GoToPreviousRune(const RuneType* Self, SizeType& InOutCursor) noexcept
{
    if (InOutCursor == 0)
    {
        --InOutCursor; // Invalid operation.
        return;
    }

    do
    {
        --InOutCursor;

        if
        (
               ((*(Self + InOutCursor) & 0x80) == 0x00) // ASCII byte. (0xxx xxxx)
            || ((*(Self + InOutCursor) & 0xC0) == 0xC0) // Multi-byte. (11xx xxxx)
        )
        {
            break;
        }

    } while (InOutCursor > 0);

    return;
}

inline void LEightStringTraits::CopyRuneOfEqualSize(RuneType* Self, const RuneType* InOther) noexcept
{
    const SizeType RuneSize = GetRuneSize(InOther);
    for (SizeType Index = 0; Index < RuneSize; ++Index)
    {
        Self[Index] = InOther[Index];
    }

    return;
}

struct L16StringTraits : public LStringTraits<char16_t, DefaultContainerSizeType>
{
    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Utf16; }
    static constexpr bool bTriviallyConvertibleToUtf8 = false;
};

struct L32StringTraits : public LStringTraits<char32_t, DefaultContainerSizeType>
{
    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Utf32; }
    static constexpr bool bTriviallyConvertibleToUtf8 = false;
};


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

/**
 * A string that uses only simple (ascii) characters.
 * Each character is one byte.
 */
using LSimpleString = LStringBase<char,     LAsciiStringTraits>;
// So we actually want to use char8_t. But that does some really weird stuff that needs to be resolved.
// So for now, we just use the std char and change it later.
// For later reference to printing the shit utf-8...
// const char8_t* MyNativeStr = u8"これわテストです。";              // Printing with this conversion works, but I do not
// setlocale(LC_ALL, "");                                         // know why the fuck it does?? And of course, this
// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;  // sln is Win only. So yea ...
// std::wstring ConvertedW = Converter.from_bytes(reinterpret_cast<const char*>(MyNativeStr));
// std::wcout << ConvertedW << "\n";
/** A string that somewhat follows the utf-8 encoding standard. More or less. */
using LEightString  = LStringBase<char   ,  LEightStringTraits>;
// using LEightString  = LStringBase<char8_t,  LEightStringTraits>; // Currently not implemented. But we probably need
// using L16String     = LStringBase<char16_t, L16StringTraits   >; // the stuff for some weird shit later. So just
// using L32String     = LStringBase<char32_t, L32StringTraits   >; // leave it here and implement this in some time...

#if LAL_PLATFORM_USES_UTF8        // The std string. Do we actually want to make this conditionally?? I mean a program
    using LString = LEightString; // that uses another string encoding than utf-8 is probably written in the eighties
#else /* PLATFORM_USES_UTF8 */    // and sucks. And then we should use wides for Windows? But who wants that???
    static_assert(false, "Missing platform encoding.");
#endif /* !PLATFORM_USES_UTF8 */

using LStringView = std::basic_string_view<char>;


/*----------------------------------------------------------------------------
    Functional Forwards.
----------------------------------------------------------------------------*/

#if PLATFORM_WASM
template <typename TString, typename... ArgTy>
TString Format(const char* Format, const ArgTy&... Args);

template <typename... ArgTy>
LStringLegacy FormatLegacy(const char* Format, const ArgTy&... Args);
#endif /* PLATFORM_WASM */

/**
 * !!! @Deprecated Use JafgString.h instead. !!!
 *
 * The base class for all string types.
 * Namings:
 *    - A character always has a fixed size for a given platform. Either 8, 16, 32 or 64 bits.
 *    - A rune is one or a collection of characters that represent a single grapheme.
 */
template <typename InCharacterTy, class InTraitsTy>
class LStringBase
{
    static_assert(std::is_base_of_v<LStringTraitsBase, InTraitsTy>);

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArrayOldv2<TMemberField>* MemberField);

public:

    using CharacterTy = InCharacterTy;
    using TraitsTy    = InTraitsTy;
    using SizeType    = typename TraitsTy::SizeType;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    inline static const CharacterTy Terminator = TraitsTy::Terminator;

    FORCEINLINE static constexpr bool IsNativeChar()  noexcept { return std::is_same_v<char,     CharacterTy>; }
    FORCEINLINE static constexpr bool IsNativeChar8() noexcept { return std::is_same_v<char8_t,  CharacterTy>; }
    FORCEINLINE static constexpr bool IsNativeCharu() noexcept { return std::is_same_v<char16_t, CharacterTy>; }
    FORCEINLINE static constexpr bool IsNativeCharU() noexcept { return std::is_same_v<char32_t, CharacterTy>; }
    FORCEINLINE static constexpr bool IsNativeCharW() noexcept { return std::is_same_v<wchar_t,  CharacterTy>; }

    FORCEINLINE LStringBase()           noexcept;
    FORCEINLINE LStringBase(LNullptrTy) noexcept;

    FORCEINLINE explicit LStringBase(const CharacterTy  InRune);
    FORCEINLINE          LStringBase(const CharacterTy* InString);
    FORCEINLINE explicit LStringBase(const CharacterTy* InString, const SizeType InLength);
    FORCEINLINE LStringBase(const LStringBase&  InOther);
    FORCEINLINE LStringBase(      LStringBase&& InOther) noexcept;
    FORCEINLINE LStringBase(const LStringBase&& InOther) noexcept = delete;

    FORCEINLINE LStringBase& operator=(const CharacterTy   InRune);
    FORCEINLINE LStringBase& operator=(const CharacterTy*  InString);
    FORCEINLINE LStringBase& operator=(const LStringBase&  InOther);
    FORCEINLINE LStringBase& operator=(      LStringBase&& InOther) noexcept;
    FORCEINLINE LStringBase& operator=(const LStringBase&& InOther) noexcept = delete;

    FORCEINLINE operator       CharacterTy*()       noexcept = delete;
    FORCEINLINE operator const CharacterTy*() const noexcept = delete;

    FORCEINLINE ~LStringBase() noexcept = default;

    /** Adds a single rune to this string. */
    FORCEINLINE void Add(const CharacterTy InRune);

    /** Appends multiple runes to this string. */
    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto Append(const char* InString) -> TEnableIf<!Condition, void>;
    FORCEINLINE void Append(const CharacterTy* InString);
    FORCEINLINE void Append(const CharacterTy* InString, const SizeType InLength);
    FORCEINLINE void Append(const LStringBase& InOther);
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const CharacterTy* InString);
    FORCEINLINE void AppendAt(const SizeType InRuneIndex, const LStringBase& InOther);

    /** Removes a single rune at a given index. */
    FORCEINLINE void RemoveAt(const SizeType InRuneIndex);

    /** Pop the last rune. */
    FORCEINLINE void Pop();
    /** Peek at the last rune. If the string is empty, a null terminator is returned. */
    FORCEINLINE const CharacterTy* Peek() const;

    /** Reserve #InCapacity number of characters (not runes). */
    FORCEINLINE void Reserve(const SizeType InCapacity) { this->Data.Reserve(InCapacity); }
    /**
     * Empties the string and sets its size to zero. The memory buffer will not be deallocated or reallocated unless
     * the #InSize parameter is greater than the current capacity and growing the current memory buffer is not
     * possible. A null-terminator will be written to the first character of the buffer.
     */
    FORCEINLINE void Reset(const SizeType InSize = 0);
    /** Empties the string and sets its size to zero. The memory buffer will be deallocated. */
    FORCEINLINE void Empty();
    FORCEINLINE bool IsEmpty() const;
    FORCEINLINE void SwapStrings(LStringBase& InOther) noexcept { this->Data.SwapBuffers(InOther.Data); }

    FORCEINLINE void CopyInto(LStringBase& InOther) const;
    FORCEINLINE void MoveInto(LStringBase& InOther) noexcept;
    FORCEINLINE void CopyFrom(const LStringBase& InOther);
    FORCEINLINE void MoveFrom(      LStringBase&& InOther) noexcept;
    FORCEINLINE auto MoveOut() -> LStringBase { return std::move(*this); }

    FORCEINLINE SizeType GetSize() const { return this->Data.GetSize(); }
    FORCEINLINE SizeType GetByteSize() const { return this->Data.GetSize() * static_cast<SizeType>(sizeof(CharacterTy)); }
    FORCEINLINE SizeType GetCharacterCount() const { return Maths::Max(this->Data.GetSize() - 0x01, 0x00); }
    FORCEINLINE SizeType GetRuneCount() const;

    /**
     * Converts the data to a c string. Utf8 will be the output if the traits allow for trivial utf8 conversion.
     */
    FORCEINLINE const char* ToC() const;
    /** Convert to a char pointer. The output encoding is not defined. */
    FORCEINLINE const char* ToCUnsafe() const;
    FORCEINLINE const CharacterTy* ToPtr() const;
    FORCEINLINE const TArrayOldv2<CharacterTy>& GetUnderlyingDataStructure() const { return this->Data; }

    /** @return The rune that is the #InRuneIndex rune in this string. */
    FORCEINLINE const CharacterTy* GetRuneAt(const SizeType InRuneIndex);
    FORCEINLINE const CharacterTy* operator[](const SizeType InRuneIndex);
    FORCEINLINE       CharacterTy& GetCharacterAtIndex(const SizeType InIndex)       { return this->Data[InIndex]; }
    FORCEINLINE const CharacterTy& GetCharacterAtIndex(const SizeType InIndex) const { return this->Data[InIndex]; }

    FORCEINLINE bool operator==(LNullptrTy) const;

    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator==(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator!=(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator <(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator >(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator<=(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> operator>=(const char* InString) const;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, LStringBase&> operator+=(const char  InString) = delete;
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, LStringBase&> operator+=(const char* InString);
    template <typename TCharacterTy = CharacterTy>
    FORCEINLINE std::enable_if_t<!std::is_same_v<char, TCharacterTy>, LStringBase&> operator-=(const char* InString) = delete;

    FORCEINLINE bool operator==(const CharacterTy* InString) const;
    FORCEINLINE bool operator!=(const CharacterTy* InString) const;
    FORCEINLINE bool operator <(const CharacterTy* InString) const;
    FORCEINLINE bool operator >(const CharacterTy* InString) const;
    FORCEINLINE bool operator<=(const CharacterTy* InString) const;
    FORCEINLINE bool operator>=(const CharacterTy* InString) const;
    FORCEINLINE LStringBase& operator+=(const CharacterTy  InRune);
    FORCEINLINE LStringBase& operator+=(const CharacterTy* InString);
    FORCEINLINE LStringBase& operator-=(const CharacterTy  InRune) = delete;

    FORCEINLINE bool operator==(const LStringBase& InOther) const;
    FORCEINLINE bool operator!=(const LStringBase& InOther) const;
    FORCEINLINE bool operator <(const LStringBase& InOther) const;
    FORCEINLINE bool operator >(const LStringBase& InOther) const;
    FORCEINLINE bool operator<=(const LStringBase& InOther) const;
    FORCEINLINE bool operator>=(const LStringBase& InOther) const;
    FORCEINLINE LStringBase& operator+=(const LStringBase& InOther);
    FORCEINLINE LStringBase& operator-=(const LStringBase& InOther) = delete;

    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto Equals(const char* InString) const -> TEnableIf<!Condition, bool>;
    FORCEINLINE bool Equals(const CharacterTy* InString) const;
    FORCEINLINE bool Equals(const LStringBase& InOther) const;

    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto Contains(const char* InString) const -> TEnableIf<!Condition, bool>;
    FORCEINLINE bool Contains(const CharacterTy* InString) const;
    FORCEINLINE bool Contains(const LStringBase& InOther) const;

    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto StartsWith(const char* InString) const -> TEnableIf<!Condition, bool>;
    FORCEINLINE bool StartsWith(const CharacterTy* InString) const;
    FORCEINLINE bool StartsWith(const LStringBase& InOther) const;
    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto EndsWith(const char* InString) const -> TEnableIf<!Condition, bool>;
    FORCEINLINE bool EndsWith(const CharacterTy* InString) const;
    FORCEINLINE bool EndsWith(const LStringBase& InString) const;

    /** Replaces a single rune. No substrings allowed. */
    FORCEINLINE void Replace(const CharacterTy  InOldRune, const CharacterTy  InNewRune);
    FORCEINLINE void Replace(const CharacterTy* InOldRune, const CharacterTy* InNewRune);

    /** Finds a single rune. Not substring. */
    FORCEINLINE SizeType FindFirst(const CharacterTy   InRune) const;
    FORCEINLINE SizeType FindFirst(const CharacterTy*  InRune) const;
    FORCEINLINE SizeType FindSecond(const CharacterTy  InRune) const;
    FORCEINLINE SizeType FindSecond(const CharacterTy* InRune) const;
    FORCEINLINE SizeType FindLast(const CharacterTy    InRune) const;
    FORCEINLINE SizeType FindLast(const CharacterTy*   InRune) const;

    template <bool Condition = IsNativeChar()>
    FORCEINLINE auto FindFirstSub(const char* InString) const -> TEnableIf<!Condition, SizeType>;
    FORCEINLINE auto FindFirstSub(const CharacterTy* InString) const -> SizeType;

    /** The index of the rune. Not the index of the targeted character. */
    FORCEINLINE auto InlineCut(const SizeType InRuneIndex) -> void;
    FORCEINLINE auto Cut(const SizeType InRuneIndex) const -> LStringBase;
    FORCEINLINE auto InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount) -> void;
    FORCEINLINE auto InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) -> void;
    FORCEINLINE auto Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const -> LStringBase;
    FORCEINLINE auto SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const -> LStringBase;
    FORCEINLINE auto RightChop(const SizeType InRuneCount) const -> LStringBase;
    FORCEINLINE auto InlineRightChop(const SizeType InRuneCount) -> void;
    FORCEINLINE auto LeftChop(const SizeType InRuneCount) const -> LStringBase;
    FORCEINLINE auto InlineLeftChop(const SizeType InRuneCount) -> void;

    /** Count the amounts a single rune occurred in a string. */
    FORCEINLINE SizeType Count(const CharacterTy  InRune) const;
    FORCEINLINE SizeType Count(const CharacterTy* InRune) const;

    FORCEINLINE void ToLower();
    FORCEINLINE LStringBase GetLowerCase() const;

    FORCEINLINE Iterator<      CharacterTy> begin()       noexcept;
    FORCEINLINE Iterator<const CharacterTy> begin() const noexcept;
    FORCEINLINE Iterator<      CharacterTy> end()         noexcept;
    FORCEINLINE Iterator<const CharacterTy> end()   const noexcept;

    template <typename... ArgTy>
    static LStringBase SprintF(const char* InFormat, const ArgTy&... InArgs);

private:

    FORCEINLINE void EnsureValidState();
    FORCEINLINE void PanicValidState() const;

    TArrayOldv2<CharacterTy> Data;
};

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase() noexcept
{
    check( this->Data.GetData() == nullptr )
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(LNullptrTy) noexcept
{
    this->Data.Empty();
    check( this->Data.GetData() == nullptr )
    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(const CharacterTy InRune)
{
    checkSlow( this->Data.GetData() == nullptr )

    this->Data.Reset(2);
    this->Data.Add(InRune);
    this->Data.Add(TraitsTy::Terminator);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(const CharacterTy* InString)
{
    checkSlow( InString )
    checkSlow( this->Data.GetData() == nullptr )

    if (*InString == TraitsTy::Terminator)
    {
        return;
    }

    const SizeType Length = TraitsTy::GetCharacterLength(InString);
    this->Data.Reset(Length + 1);
    while (*InString != TraitsTy::Terminator)
    {
        this->Data.Add(*InString++);
    }
    this->Data.Add(TraitsTy::Terminator);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(const CharacterTy* InString, const SizeType InLength)
{
    checkSlow( InString )
    checkSlow( this->Data.GetData() == nullptr )
    checkSlow( InLength > 0 )

    this->Data.Reset(InLength + 1);
    for (SizeType Index = 0; Index < InLength; ++Index)
    {
        this->Data.Add(InString[Index]);
    }
    this->Data.Add(TraitsTy::Terminator);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(const LStringBase& InOther)
{
    this->Data = InOther.Data;

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>::LStringBase(LStringBase&& InOther) noexcept
{
    this->Data = std::forward<TArrayOldv2<CharacterTy>>(InOther.Data);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
    InOther.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator=(const CharacterTy InRune)
{
    const CharacterTy Characters[2] = { InRune, TraitsTy::Terminator };
    this->operator=(Characters);
    return *this;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator=(const CharacterTy* InString)
{
    this->Reset(0);
    this->Append(InString);
    return *this;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator=(const LStringBase& InOther)
{
    this->Data = InOther.Data;
    return *this;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator=(LStringBase&& InOther) noexcept
{
    this->Data = std::forward<TArrayOldv2<CharacterTy>>(InOther.Data);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
    InOther.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return *this;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Add(const CharacterTy InRune)
{
    this->Append(&InRune, 1);
    return;
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, void>
LStringBase<InCharacterTy, InTraitsTy>::Append(const char* InString)
{
    this->Append(reinterpret_cast<const CharacterTy*>(InString));
    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Append(const CharacterTy* InString)
{
    checkSlow( InString )
    const SizeType Length = TraitsTy::GetCharacterLength(InString);
    if (Length == 0)
    {
        return;
    }

    this->Append(InString, Length);

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Append(const CharacterTy* InString, const SizeType InLength)
{
    checkSlow( InString )
    checkSlow( InLength > 0 )

    if (this->Data.IsEmpty() == false)
    {
        this->Data.Pop(); /* Null terminator. */
    }

    this->Data.Reserve(this->Data.GetSize() + InLength + /* Terminator */1);

    for (SizeType Index = 0; Index < InLength; ++Index)
    {
        checkSlow( InString[Index] != TraitsTy::Terminator )
        this->Data.Add(InString[Index]);
    }
    this->Data.Add(TraitsTy::Terminator);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Append(const LStringBase& InOther)
{
    if (InOther.IsEmpty())
    {
        return;
    }

    if (this->Data.IsEmpty() == false)
    {
        this->Data.Pop(); /* Null terminator. */
    }

    this->Data.Append(InOther.Data);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::AppendAt(const SizeType InRuneIndex, const CharacterTy* InString)
{
    checkSlow( InString )

    const SizeType Length = TraitsTy::GetCharacterLength(InString);
    if (Length == 0)
    {
        return;
    }

    SizeType Cursor = 0;
    TraitsTy::GoToRune(this->Data.GetData(), InRuneIndex, Cursor);
    checkSlow( this->Data.IsValidIndex(Cursor) )

    this->Data.Reserve(this->Data.GetSize() + Length);
    this->Data.AppendAt(Cursor, InString, Length);

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::AppendAt(const SizeType InRuneIndex, const LStringBase& InOther)
{
    this->AppendAt(InRuneIndex, InOther.ToPtr());
    return;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::RemoveAt(const SizeType InRuneIndex)
{
    checkSlow( this->Data.IsValidIndex(InRuneIndex) )

    SizeType Cursor = 0;
    TraitsTy::GoToRune(this->Data.GetData(), InRuneIndex, Cursor);
    checkSlow( this->Data.IsValidIndex(Cursor) )
    this->Data.RemoveAt(Cursor, TraitsTy::GetRuneSize(this->Data.GetData() + Cursor));

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Pop()
{
    if (this->GetCharacterCount() > 0)
    {
        checkSlow( this->Peek() != &TraitsTy::Terminator )
        this->Data.Pop(); /* Null terminator */

        SizeType Cursor = this->GetSize(); /* Use size and not size-1 because we already popped the null terminator. */
        TraitsTy::GoToPreviousRune(this->Data.GetData(), Cursor);
        this->Data.Resize(Cursor, false);

        if (this->Data.IsEmpty())
        {
            /* Orphan memory. */
            this->Empty();
        }
        else
        {
            this->Data.Add(TraitsTy::Terminator);
        }
    }

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
const typename LStringBase<InCharacterTy, InTraitsTy>::CharacterTy* LStringBase<InCharacterTy, InTraitsTy>::Peek() const
{
    if (this->Data.IsEmpty() == false)
    {
        return TraitsTy::GetLastRunePointer(this->Data.GetData(), this->GetSize());
    }

    return &TraitsTy::Terminator;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Reset(const SizeType InSize /* = 0 */)
{
    check( InSize >= 0 )

    this->Data.Reset(InSize);
    this->Data.Add(TraitsTy::Terminator);

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Empty()
{
    this->Data.Empty();

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::IsEmpty() const
{
    if (this->Data.IsEmpty())
    {
        return true;
    }

    return this->Data[0] == TraitsTy::Terminator;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::CopyInto(LStringBase& InOther) const
{
    InOther = *this;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::MoveInto(LStringBase& InOther) noexcept
{
    InOther = std::move(*this);
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::CopyFrom(const LStringBase& InOther)
{
    *this = InOther;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::MoveFrom(LStringBase&& InOther) noexcept
{
    *this = std::move(InOther);
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::GetRuneCount() const
{
    return TraitsTy::GetRuneCount(this->ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
const char* LStringBase<InCharacterTy, InTraitsTy>::ToC() const
{
    if (this->Data.IsEmpty())
    {
        return reinterpret_cast<const char*>(&TraitsTy::Terminator);
    }

    if constexpr (TraitsTy::bTriviallyConvertibleToUtf8)
    {
        return reinterpret_cast<const char*>(this->Data.GetData());
    }

    panic( "Traits do not allow for trivial utf8 conversion." )
    return reinterpret_cast<const char*>(this->Data.GetData());
}

template <typename InCharacterTy, class InTraitsTy>
const char* LStringBase<InCharacterTy, InTraitsTy>::ToCUnsafe() const
{
    if (this->Data.IsEmpty() == false)
    {
        return reinterpret_cast<const char*>(this->Data.GetData());
    }

    return reinterpret_cast<const char*>(&TraitsTy::Terminator);
}

template <class InCharacterTy, class InTraitsTy>
const InCharacterTy* LStringBase<InCharacterTy, InTraitsTy>::ToPtr() const
{
    if (this->Data.IsEmpty() == false)
    {
        return this->Data.GetData();
    }

    return &TraitsTy::Terminator;
}

template <class InCharacterTy, class InTraitsTy>
const InCharacterTy* LStringBase<InCharacterTy, InTraitsTy>::GetRuneAt(const SizeType InRuneIndex)
{
    if (this->Data.IsEmpty() == false)
    {
        checkSlow( InRuneIndex >= 0 )
        checkSlow( InRuneIndex < this->GetCharacterCount() )

        return TraitsTy::GetRuneAt(this->ToPtr(), InRuneIndex);
    }

    return nullptr;
}

template <class InCharacterTy, class InTraitsTy>
const InCharacterTy* LStringBase<InCharacterTy, InTraitsTy>::operator[](const SizeType InRuneIndex)
{
    return this->GetRuneAt(InRuneIndex);
}

template <typename InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator==(LNullptrTy) const
{
    return this->Data.IsEmpty();
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator==(const char* InString) const
{
    checkSlow( InString )

    if constexpr (InTraitsTy::bTriviallyConvertibleToUtf8)
    {
        return this->operator==(reinterpret_cast<const InCharacterTy*>(InString));
    }

    unimplemented()
    return false;
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator!=(const char* InString) const
{
    checkSlow( InString )
    return !this->operator==(InString);
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator<(const char* InString) const
{
    checkSlow( InString )

    if constexpr (InTraitsTy::bTriviallyConvertibleToUtf8)
    {
        return this->operator<(reinterpret_cast<const InCharacterTy*>(InString));
    }

    unimplemented()
    return false;
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator>(const char* InString) const
{
    checkSlow( InString )

    if constexpr (InTraitsTy::bTriviallyConvertibleToUtf8)
    {
        return this->operator>(reinterpret_cast<const InCharacterTy*>(InString));
    }

    unimplemented()
    return false;
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator<=(const char* InString) const
{
    checkSlow( InString )
    return !this->operator>(InString);
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, bool> LStringBase<InCharacterTy, InTraitsTy>::operator>=(const char* InString) const
{
    checkSlow( InString )
    return !this->operator<(InString);
}

template <typename InCharacterTy, class InTraitsTy>
template <typename TCharacterTy>
std::enable_if_t<!std::is_same_v<char, TCharacterTy>, LStringBase<InCharacterTy, InTraitsTy>&>
LStringBase<InCharacterTy, InTraitsTy>::operator+=(const char* InString)
{
    checkSlow( InString )

#if LAL_PLATFORM_USES_UTF8
    return this->operator+=(reinterpret_cast<const CharacterTy*>(InString));
#else
    unimplemented()
    return *this;
#endif /* PLATFORM_USES_UTF8 */
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator==(const InCharacterTy* InString) const
{
    checkSlow( InString != nullptr )

    if (this->Data.IsEmpty())
    {
        return *InString == TraitsTy::Terminator;
    }

    for (const CharacterTy Character : this->Data)
    {
        if (Character != *InString++)
        {
            return false;
        }

        continue;
    }

    return true;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator!=(const InCharacterTy* InString) const
{
    checkSlow( InString != nullptr )
    return !this->operator==(InString);
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator<(const InCharacterTy* InString) const
{
    checkSlow( InString != nullptr )

    if (this->Data.IsEmpty())
    {
        return *InString != TraitsTy::Terminator;
    }

    for (const CharacterTy Character : this->Data)
    {
        if (*InString == TraitsTy::Terminator)
        {
            return false;
        }

        if (Character < *InString)
        {
            return true;
        }

        if (Character > *InString)
        {
            return false;
        }

        ++InString;

        continue;
    }

    return true;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator>(const InCharacterTy* InString) const
{
    checkSlow( InString != nullptr )

    if (this->Data.IsEmpty())
    {
        return false;
    }

    for (const CharacterTy Rune : this->Data)
    {
        if (*InString == TraitsTy::Terminator)
        {
            return Rune != TraitsTy::Terminator;
        }

        if (Rune > *InString)
        {
            return true;
        }

        if (Rune < *InString)
        {
            return false;
        }

        ++InString;

        continue;
    }

    return false;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator<=(const InCharacterTy* InString) const
{
    return !this->operator>(InString);
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator>=(const InCharacterTy* InString) const
{
    return !this->operator<(InString);
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator+=(const InCharacterTy InRune)
{
    CharacterTy Characters[1] = { InRune };
    this->Append(Characters, 1);
    return *this;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator+=(const InCharacterTy* InString)
{
    this->Append(InString);
    return *this;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator==(const LStringBase& InOther) const
{
    if (this->Data.GetSize() != InOther.Data.GetSize())
    {
        return false;
    }

    return this->operator==(InOther.ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator!=(const LStringBase& InOther) const
{
    return !this->operator==(InOther);
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator<(const LStringBase& InOther) const
{
    return this->operator<(InOther.ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator>(const LStringBase& InOther) const
{
    return this->operator>(InOther.ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator<=(const LStringBase& InOther) const
{
    return this->operator<=(InOther.ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::operator>=(const LStringBase& InOther) const
{
    return this->operator>=(InOther.ToPtr());
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy>& LStringBase<InCharacterTy, InTraitsTy>::operator+=(const LStringBase& InOther)
{
    this->Append(InOther);
    return *this;
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, bool>
LStringBase<InCharacterTy, InTraitsTy>::Equals(const char* InString) const
{
    return *this == InString;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::Equals(const InCharacterTy* InString) const
{
    return *this == InString;
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::Equals(const LStringBase& InOther) const
{
    return *this == InOther;
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, bool>
LStringBase<InCharacterTy, InTraitsTy>::Contains(const char* InString) const
{
    return this->Contains(reinterpret_cast<const InCharacterTy*>(InString));
}

template <typename InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::Contains(const CharacterTy* InString) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    if (*InString == TraitsTy::Terminator)
    {
        return true;
    }

    SizeType Cursor      = 0;
    SizeType InnerCursor = 0;
    const CharacterTy* DataPtr = this->ToPtr();
    while (*(DataPtr + Cursor) != TraitsTy::Terminator)
    {
        SizeType InStringCursor = 0;

        while (true)
        {
            if (*(InString + InStringCursor) == TraitsTy::Terminator)
            {
                return true;
            }

            if (*(DataPtr + InnerCursor) == TraitsTy::Terminator) // The InString is longer than the substr we
            {                                                     // are comparing.
                return false;
            }

            if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + InnerCursor, InString + InStringCursor))
            {
                const SizeType CurRuneSize = TraitsTy::GetRuneSize(DataPtr + InnerCursor);
                InnerCursor += CurRuneSize;
                InStringCursor += CurRuneSize;
                continue;
            }

            goto RunesAreNotEqual;
        }

        checkNoEntry()
        RunesAreNotEqual:
            TraitsTy::GoToNextRune(DataPtr, Cursor);
            InnerCursor = Cursor;
            continue;
    }

    return false;
}

template <typename InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::Contains(const LStringBase& InOther) const
{
    return this->Contains(InOther.ToPtr());
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, bool>
LStringBase<InCharacterTy, InTraitsTy>::StartsWith(const char* InString) const
{
    return this->StartsWith(reinterpret_cast<const InCharacterTy*>(InString));
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::StartsWith(const InCharacterTy* InString) const
{
    if (*InString == TraitsTy::Terminator)
    {
        return *this->Peek() == *InString;
    }

    i32 Cursor = 0;
    while (*InString != TraitsTy::Terminator)
    {
        if (this->GetCharacterCount() <= Cursor)
        {
            return false;
        }

        if (this->Data[Cursor++] != *InString)
        {
            return false;
        }

        ++InString;

        continue;
    }

    return Cursor <= this->GetCharacterCount();
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::StartsWith(const LStringBase& InOther) const
{
    return this->StartsWith(InOther.ToPtr());
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, bool>
LStringBase<InCharacterTy, InTraitsTy>::EndsWith(const char* InString) const
{
    return this->EndsWith(reinterpret_cast<const InCharacterTy*>(InString));
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::EndsWith(const InCharacterTy* InString) const
{
    LStringBase Helper = InString;
    return this->EndsWith(Helper);
}

template <class InCharacterTy, class InTraitsTy>
bool LStringBase<InCharacterTy, InTraitsTy>::EndsWith(const LStringBase& InString) const
{
    if (InString.GetCharacterCount() <= 0 || InString.GetCharacterCount() > this->GetCharacterCount())
    {
        return false;
    }

    SizeType Cursor = this->GetSize() - InString.GetSize();
    check( Cursor >= 0 )
    while (Cursor < this->GetSize())
    {
        if (this->Data[Cursor] != InString.Data[Cursor - this->GetSize() + InString.GetSize()])
        {
            return false;
        }

        ++Cursor;

        continue;
    }

    return true;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Replace(const InCharacterTy InOldRune, const InCharacterTy InNewRune)
{
    for (CharacterTy& Rune : this->Data)
    {
        if (Rune == InOldRune)
        {
            Rune = InNewRune;
        }

        continue;
    }

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::Replace(const InCharacterTy* InOldRune, const InCharacterTy* InNewRune)
{
    if (this->Data.IsEmpty())
    {
        return;
    }

    const SizeType OldRuneLength = TraitsTy::GetRuneSize(InOldRune);
    const SizeType NewRuneLength = TraitsTy::GetRuneSize(InNewRune);

    if (OldRuneLength == NewRuneLength)
    {
        SizeType Cursor = 0;
        while (Cursor < this->GetSize())
        {
            if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(this->Data.GetData() + Cursor, InOldRune))
            {
                TraitsTy::CopyRuneOfEqualSize(this->Data.GetData() + Cursor, InNewRune);
            }

            TraitsTy::GoToNextRune(this->Data.GetData(), Cursor);

            continue;
        }
    }
    else
    {
        unimplemented()
    }

    return;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindFirst(const InCharacterTy InRune) const
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Data[Index] == InRune)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindFirst(const InCharacterTy* InRune) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    SizeType Out    = 0;
    SizeType Cursor = 0;
    const CharacterTy* DataPtr = this->ToPtr();
    while (*(DataPtr + Cursor) != TraitsTy::Terminator)
    {
        if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + Cursor, InRune))
        {
            return Out;
        }

        TraitsTy::GoToNextRune(DataPtr, Cursor);
        ++Out;

        continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindSecond(const InCharacterTy InRune) const
{
    bool bFound = false;

    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Data[Index] == InRune)
        {
            if (bFound)
            {
                return Index;
            }

            bFound = true;
        }

        continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindSecond(const InCharacterTy* InRune) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    bool bFound = false;
    SizeType Out    = 0;
    SizeType Cursor = 0;
    const CharacterTy* DataPtr = this->ToPtr();
    while (*(DataPtr + Cursor) != TraitsTy::Terminator)
    {
        if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + Cursor, InRune))
        {
            if (bFound)
            {
                return Out;
            }

            bFound = true;
        }

        TraitsTy::GoToNextRune(DataPtr, Cursor);
        ++Out;

        continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindLast(const InCharacterTy InRune) const
{
    for (i32 Index = this->GetSize() - 1; Index >= 0; --Index)
    {
        if (this->Data[Index] == InRune)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindLast(const InCharacterTy* InRune) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    if (this->Data.IsEmpty())
    {
        return INDEX_NONE;
    }

    SizeType Out    = 0;
    SizeType Cursor = this->GetSize();
    const CharacterTy* DataPtr = this->ToPtr();
    while (Cursor >= 0)
    {
        /* We instantly go to the previous rune as we start on the null-terminator. */
        TraitsTy::GoToPreviousRune(DataPtr, Cursor);

        if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + Cursor, InRune))
        {
            return this->GetRuneCount() - Out;
        }

        ++Out;

        continue;
    }

    return INDEX_NONE;
}

template <typename InCharacterTy, class InTraitsTy>
template <bool Condition>
typename LStringBase<InCharacterTy, InTraitsTy>::template TEnableIf<!Condition, typename LStringBase<InCharacterTy, InTraitsTy>::SizeType>
LStringBase<InCharacterTy, InTraitsTy>::FindFirstSub(const char* InString) const
{
    return this->FindFirstSub(reinterpret_cast<const InCharacterTy*>(InString));
}

template <typename InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::FindFirstSub(const CharacterTy* InString) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    if (*InString == TraitsTy::Terminator)
    {
        return INDEX_NONE;
    }

    SizeType RuneCursor  = 0;
    SizeType Cursor      = 0;
    SizeType InnerCursor = 0;
    const CharacterTy* DataPtr = this->ToPtr();
    while (*(DataPtr + Cursor) != TraitsTy::Terminator)
    {
        SizeType InStringCursor = 0;

        while (true)
        {
            if (*(InString + InStringCursor) == TraitsTy::Terminator)
            {
                return RuneCursor;
            }

            if (*(DataPtr + InnerCursor) == TraitsTy::Terminator) // The InString is longer than the substr we
            {                                                     // are comparing.
                return INDEX_NONE;
            }

            if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + InnerCursor, InString + InStringCursor))
            {
                const SizeType CurRuneSize = TraitsTy::GetRuneSize(DataPtr + InnerCursor);
                InnerCursor += CurRuneSize;
                InStringCursor += CurRuneSize;
                ++RuneCursor;
                continue;
            }

            goto RunesAreNotEqual;
        }

        checkNoEntry()
        RunesAreNotEqual:
            TraitsTy::GoToNextRune(DataPtr, Cursor);
            InnerCursor = Cursor;
            continue;
    }

    return INDEX_NONE;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::InlineCut(const SizeType InRuneIndex)
{
    SizeType Cursor = 0;
    TraitsTy::GoToRune(this->ToPtr(), InRuneIndex, Cursor);
    check( this->Data.IsValidIndex(Cursor) )

    this->Data.Resize(Cursor + /* Terminator */1, true);
    this->Data[Cursor] = TraitsTy::Terminator;

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::Cut(const SizeType InRuneIndex) const
{
    if (InRuneIndex == 0)
    {
        return { };
    }

    SizeType Cursor = 0;
    TraitsTy::GoToRune(this->ToPtr(), InRuneIndex, Cursor);
    check( this->Data.IsValidIndex(Cursor) )

    LStringBase Sub;
    Sub.Data.CopyFrom(this->Data, Cursor + 1);
    Sub.Data[Cursor] = TraitsTy::Terminator;

#if LAL_CHECK_STRING_VALIDITY
    Sub.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return Sub;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::InlineSub(const SizeType InRuneStartIndex, const SizeType InRuneCount)
{
    check( InRuneCount >= 0 )

    SizeType Cursor; // Cursor in characters.
    TraitsTy::GoToRune(this->ToPtr(), InRuneStartIndex, Cursor);
    check( this->Data.IsValidIndex(Cursor) )

    SizeType EndCursor; // Cursor in characters.
    TraitsTy::GoToRune(this->ToPtr() + Cursor, InRuneCount, EndCursor);
    check( this->Data.IsValidIndex(EndCursor + Cursor) )

    LStringBase Sub;
    Sub.Data.CopyFrom(this->Data, Cursor, EndCursor + 1);
    Sub.Data[EndCursor] = TraitsTy::Terminator;

    *this = std::move(Sub);

    check( Sub.GetSize()           == 0 )
    check( Sub.GetCharacterCount() == 0 )
    check( Sub.Data.GetCapacity()  == 0 )

#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::InlineSubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex)
{
    check( InRuneStartIndex <= InRuneEndIndex )
    this->InlineSub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
    return;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::Sub(const SizeType InRuneStartIndex, const SizeType InRuneCount) const
{
    if (InRuneStartIndex == 0)
    {
        return this->Cut(InRuneCount); /* Count is index. */
    }

    if (InRuneCount == 0)
    {
        return { };
    }

    SizeType Cursor; // Cursor in characters.
    TraitsTy::GoToRune(this->ToPtr(), InRuneStartIndex, Cursor);
    check( this->Data.IsValidIndex(Cursor) )

    SizeType EndCursor; // Cursor in characters.
    TraitsTy::GoToRune(this->ToPtr() + Cursor, InRuneCount, EndCursor);
    check( this->Data.IsValidIndex(EndCursor + Cursor) )

    LStringBase Sub;
    Sub.Data.CopyFrom(this->Data, Cursor, EndCursor + 1);
    Sub.Data[EndCursor] = TraitsTy::Terminator;

#if LAL_CHECK_STRING_VALIDITY
    Sub.EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    return Sub;
}

template <class InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::SubIdx(const SizeType InRuneStartIndex, const SizeType InRuneEndIndex) const
{
    check( InRuneStartIndex <= InRuneEndIndex )
    return this->Sub(InRuneStartIndex, InRuneEndIndex - InRuneStartIndex);
}

template <typename InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::RightChop(const SizeType InRuneCount) const
{
    LStringBase Out;
    this->CopyInto(Out);
    Out.InlineRightChop(InRuneCount);
    return Out;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::InlineRightChop(const SizeType InRuneCount)
{
    this->InlineSubIdx(InRuneCount, this->GetRuneCount());
}

template <typename InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::LeftChop(const SizeType InRuneCount) const
{
    LStringBase Out;
    this->CopyInto(Out);
    Out.InlineLeftChop(InRuneCount);
    return Out;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::InlineLeftChop(const SizeType InRuneCount)
{
    this->InlineSub(0, InRuneCount);
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::Count(const InCharacterTy InRune) const
{
    SizeType Out = 0;
    for (const CharacterTy Rune : this->Data)
    {
        if (Rune == InRune)
        {
            ++Out;
        }

        continue;
    }

    return Out;
}

template <class InCharacterTy, class InTraitsTy>
typename LStringBase<InCharacterTy, InTraitsTy>::SizeType LStringBase<InCharacterTy, InTraitsTy>::Count(const InCharacterTy* InRune) const
{
#if LAL_CHECK_STRING_VALIDITY
    this->PanicValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    SizeType Out    = 0;
    SizeType Cursor = 0;
    const CharacterTy* DataPtr = this->ToPtr();
    while (*(DataPtr + Cursor) != TraitsTy::Terminator)
    {
        if (TraitsTy::template IsRuneEqual<TraitsTy::GetEncodingType()>(DataPtr + Cursor, InRune))
        {
            ++Out;
        }

        TraitsTy::GoToNextRune(DataPtr, Cursor);

        continue;
    }

    return Out;
}

template <typename InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::ToLower()
{
#if LAL_CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* LAL_CHECK_STRING_VALIDITY */

    SizeType Cursor = 0;
    while (Cursor < this->GetSize())
    {
        TraitsTy::ToLowerCase(this->Data.GetData() + Cursor);
        TraitsTy::GoToNextRune(this->Data.GetData(), Cursor);

        continue;
    }

    return;
}

template <typename InCharacterTy, class InTraitsTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::GetLowerCase() const
{
    LStringBase Out = *this;
    Out.ToLower();
    return Out;
}

template <class InCharacterTy, class InTraitsTy>
Iterator<InCharacterTy> LStringBase<InCharacterTy, InTraitsTy>::begin() noexcept
{
    if (this->Data.IsEmpty())
    {
        return { nullptr };
    }

    return { this->Data.GetData() };
}

template <class InCharacterTy, class InTraitsTy>
Iterator<const InCharacterTy> LStringBase<InCharacterTy, InTraitsTy>::begin() const noexcept
{
    if (this->Data.IsEmpty())
    {
        return { nullptr };
    }

    return { this->Data.GetData() };
}

template <class InCharacterTy, class InTraitsTy>
Iterator<InCharacterTy> LStringBase<InCharacterTy, InTraitsTy>::end() noexcept
{
    if (this->Data.IsEmpty())
    {
        return { nullptr };
    }

    return { this->Data.GetSlack() - 1 };
}

template <class InCharacterTy, class InTraitsTy>
Iterator<const InCharacterTy> LStringBase<InCharacterTy, InTraitsTy>::end() const noexcept
{
    if (this->Data.IsEmpty())
    {
        return { nullptr };
    }

    return { this->Data.GetSlack() - 1 };
}

template <typename InCharacterTy, class InTraitsTy>
template <typename... ArgTy>
LStringBase<InCharacterTy, InTraitsTy> LStringBase<InCharacterTy, InTraitsTy>::SprintF(const char* InFormat, const ArgTy&... InArgs)
{
// #if PLATFORM_WASM
//     return ::Jafg::Format<LStringBase>(InFormat, InArgs...);
// #else /* PLATFORM_WASM */
    /**
     * Super supid solution. But who cares right now.
     * Later we write our own implementation with type safety etc.
     *
     * S will be stackallocated if in str is small enough. So its not that bad.
     * But still one unnecessary heap allocation by this class...
     */
    const LStringLegacy S = std::vformat(InFormat, std::make_format_args(InArgs...));
    return { S.c_str() };
// #endif /* !PLATFORM_WASM */
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::EnsureValidState()
{
    if (this->Data.IsEmpty() == false)
    {
        if (*this->Data.Peek() != TraitsTy::Terminator)
        {
            this->Data.Add(TraitsTy::Terminator);
            LOG_ERROR
            (
                LogJafgInternal,
                "Ensured that string is null-terminated. This may not happen in shipping configurations."
            )
        }

        return;
    }

    check( this->GetByteSize()      == 0       )
    check( this->GetRuneCount()     == 0       )
    check( this->Data.GetCapacity() == 0       )
    check( this->Data.GetData()     == nullptr )

    return;
}

template <class InCharacterTy, class InTraitsTy>
void LStringBase<InCharacterTy, InTraitsTy>::PanicValidState() const
{
    if (this->Data.IsEmpty() == false)
    {
        if (*this->Data.Peek() != TraitsTy::Terminator)
        {
            LOG_FATAL
            (
                LogJafgInternal,
                "String is not null terminated."
            )
        }

        return;
    }

    check( this->GetByteSize()      == 0       )
    check( this->GetRuneCount()     == 0       )
    check( this->Data.GetCapacity() == 0       )
    check( this->Data.GetData()     == nullptr )

    return;
}

} /* ~Namespace Jafg::Legacy */

template <>
struct std::formatter<::Jafg::Legacy::LSimpleString> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::Legacy::LSimpleString& InString,
        ::std::format_context&       InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<const char*>::format(InString.ToC(), InContext);
    }
};
template <>
struct std::formatter<::Jafg::Legacy::LEightString> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::Legacy::LEightString& InString,
        ::std::format_context&      InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<const char*>::format(InString.ToC(), InContext);
    }
};
