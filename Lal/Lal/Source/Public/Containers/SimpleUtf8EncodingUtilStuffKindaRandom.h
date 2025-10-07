// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Lal::Utf8
{

template<typename T, typename U = LSize>
NODISCARD
inline U GetCharacterLength(const T Char) noexcept
{
    /* ASCII byte. */
    if ((Char & 0x80) == 0)
    {
        return 1;
    }

    /* 2-byte sequence. */
    if ((Char & 0xE0) == 0xC0)
    {
        return 2;
    }

    /* 3-byte sequence. */
    if ((Char & 0xF0) == 0xE0)
    {
        return 3;
    }

    /* 4-byte sequence. */
    if ((Char & 0xF8) == 0xF0)
    {
        return 4;
    }

    checkNoEntry()
    /* Invalid UTF-8. */
    // Return 1; So we do not hang indefinitely.
    return 1;
}

} /* ~Namespace Lal::Utf8 */
