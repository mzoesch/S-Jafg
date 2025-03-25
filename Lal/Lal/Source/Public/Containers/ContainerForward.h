// Copyright mzoesch. All rights reserved.

#pragma once

#include "Hal/Platform.h"

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

typedef i32 DefaultContainerSizeType;

namespace ResizePolicy
{

enum Type : u8
{
    Static,
    Dynamic
};

} /* ~Namespace ResizePolicy */

namespace AllocationPolicy
{

enum Type : u8
{
    Stack,
    Heap
};

} /* ~Namespace AllocationPolicy */

namespace EQueueKind
{

enum Type : u8
{
    /**
     * Single producer; single consumer.
     */
    Spsc,

    /**
     * Multiple producers; single consumer.
     */
    Mpsc,

    /**
     * Multiple producers; multiple consumers.
     */
    Mpmc,
};

} /* ~Namespace EQueueKind */

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

template
<
    typename                T                   ,
    ResizePolicy::Type      ResizePolicy        ,
    AllocationPolicy::Type  AllocationPolicy    ,
    typename                SizeType            = DefaultContainerSizeType
>
class TArray;

struct LStringTraitsBase;
template <typename InCharacterTy, class InTraitsTy>
class LStringBase;

template <typename T, typename TSizeType = DefaultContainerSizeType>
class TMpscQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;

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

template <typename T> using TStaticHeapArray        = TArray<T, ResizePolicy::Static , AllocationPolicy::Heap >;
template <typename T> using TDynamicHeapArray       = TArray<T, ResizePolicy::Dynamic, AllocationPolicy::Heap >;
template <typename T> using TStaticStackArray       = TArray<T, ResizePolicy::Static , AllocationPolicy::Stack>;
template <typename T> using TDynamicStackArray      = TArray<T, ResizePolicy::Dynamic, AllocationPolicy::Stack>;

template <typename T> using TshArray                = TStaticHeapArray<T>;
template <typename T> using TdhArray                = TDynamicHeapArray<T>;
template <typename T> using TssArray                = TStaticStackArray<T>;
template <typename T> using TdsArray                = TDynamicStackArray<T>;

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

#if PLATFORM_USES_UTF8            // The std string. Do we actually want to make this conditionally?? I mean a program
    using LString = LEightString; // that uses another string encoding than utf-8 is probably written in the eighties
#else /* PLATFORM_USES_UTF8 */    // and sucks. And then we should use wides for Windows? But who wants that???
    static_assert(false, "Missing platform encoding.");
#endif /* !PLATFORM_USES_UTF8 */

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TSimpleQueue<T, TKind>;

using LStringView = std::basic_string_view<char>;

/*----------------------------------------------------------------------------
    Functional Forwards.
----------------------------------------------------------------------------*/

#if PLATFORM_WASM
template <typename TString, typename ... ArgTy>
TString Format(const char* Format, const ArgTy& ... Args);

template <typename ... ArgTy>
LStringLegacy FormatLegacy(const char* Format, const ArgTy& ... Args);
#endif /* PLATFORM_WASM */

} /* ~Namespace Jafg */
