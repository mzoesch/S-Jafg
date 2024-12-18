// Copyright mzoesch. All rights reserved.

#pragma once

#include "Hal/Platform.h"

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

typedef int32 DefaultContainerSizeType;

namespace ResizePolicy
{

enum Type : uint8
{
    Static,
    Dynamic
};

} /* ~Namespace ResizePolicy */

namespace AllocationPolicy
{

enum Type : uint8
{
    Stack,
    Heap
};

} /* ~Namespace AllocationPolicy */

namespace EQueueKind
{

enum Type : uint8
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

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TComplexQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;

struct LStringTraitsBase
{
};

template <class InCharacterTy, class InSizetype>
struct LStringTraits : public LStringTraitsBase
{
    static_assert(std::is_signed_v<InSizetype>, "InSizetype must be signed.");

    using RuneType = InCharacterTy;
    using SizeType = InSizetype;

    inline static RuneType Terminator = '\0';

    /** Whether the first rune of #Self is equal of the first rune of #InOther. */
    template <ELiteralEncoding::Type InOtherEncodingTy>
    static bool IsRuneEqual(const RuneType* Self, const RuneType* InOther) noexcept;

    /** The number of characters. Excluding terminator. */
    static SizeType GetCharacterLength(const RuneType* InString) noexcept;
    static SizeType GetRuneCount(const RuneType* Self) noexcept;

    static const RuneType* GetLastRunePointer(const RuneType* Self, const SizeType InCharacterSize) noexcept;
    static const RuneType* GetRuneAt(const RuneType* Self, const SizeType InRuneIndex) noexcept;

    static SizeType GetRuneSize(const RuneType* InRuneToCheck) noexcept;
    static void GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept;
    static void GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept;
    static void GoToPreviousRune(const RuneType* Self, SizeType& InOutCursor) noexcept;
    static void CopyRuneOfEqualSize(RuneType* Self, const RuneType* InOther) noexcept;
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
void LStringTraits<InCharacterTy, InSizetype>::GoToNextRune(const RuneType* Self, SizeType& InOutCursor) noexcept
{
    ++InOutCursor;
}

template <class InCharacterTy, class InSizetype>
void LStringTraits<InCharacterTy, InSizetype>::GoToRune(const RuneType* Self, const SizeType InRuneIndex, SizeType& OutCursor) noexcept
{
    OutCursor = InRuneIndex;
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

struct LAsciiStringTraits : public LStringTraits<char, DefaultContainerSizeType>
{
    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Ascii; }
    static constexpr bool bTriviallyConvertibleToUtf8 = true;
};

struct LEightStringTraits : public LStringTraits<char8_t, DefaultContainerSizeType>
{
    static constexpr ELiteralEncoding::Type GetEncodingType() { return ELiteralEncoding::Utf8; }
    static constexpr bool bTriviallyConvertibleToUtf8 = true;
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
using LSimpleString = LStringBase<char, LAsciiStringTraits>;

/**
 * A string that uses the utf-8 encoding.
 */
using LEightString = LStringBase<char8_t, LEightStringTraits>;

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TSimpleQueue<T, TKind>;


/*----------------------------------------------------------------------------
    Functional Forwards.
----------------------------------------------------------------------------*/

template <typename ... ArgTy>
LSimpleString Format(const LChar* Format, const ArgTy& ... Args);

template <typename ... ArgTy>
LStringLegacy FormatLegacy(const LChar* Format, const ArgTy& ... Args);

} /* ~Namespace Jafg */
