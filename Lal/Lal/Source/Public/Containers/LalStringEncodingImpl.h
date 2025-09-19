// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, std::integral TSizeType>
template <typename T>
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetStringLength(const T* String) noexcept
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
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterCount(const T* String) noexcept
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
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterCount(const T* Begin, const T* End) noexcept
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
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::SizeType
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetCharacterSize(const T* Character) noexcept
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
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T*
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::RecoverToMostRecentValidCharacter(T* Character, const T* Guard /* = nullptr */)
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
FORCEINLINE const typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T*
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::RecoverToMostRecentValidCharacter(const T* Character, const T* Guard /* = nullptr */)
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
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetUppercaseRune(const T Rune)
{
    if (Rune >= 'a' && Rune <= 'z')
    {
        return Rune - 32;
    }

    return Rune;
}

template <typename TIn, std::integral TSizeType>
FORCEINLINE typename TStringBaseDefaultUtf8Traits<TIn, TSizeType>::T
TStringBaseDefaultUtf8Traits<TIn, TSizeType>::GetLowercaseRune(const T Rune)
{
    if (Rune >= 'A' && Rune <= 'Z')
    {
        return Rune + 32;
    }

    return Rune;
}

} /* ~Namespace Lal */
