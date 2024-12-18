// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

namespace Jafg::Testing::NativeLiterals
{

/** Checks byte patterns. */
inline bool IsUtf8(const char* InString)
{
    const unsigned char* Bytes = reinterpret_cast<const unsigned char*>(InString);
    while (*Bytes)
    {
        if ((*Bytes & 0x80) == 0)
        { /* ASCII byte. */
            ++Bytes;
        }
        else if ((*Bytes & 0xE0) == 0xC0)
        { /* 2-byte sequence. */
            if ((Bytes[1] & 0xC0) != 0x80) return false;
            Bytes += 2;
        }
        else if ((*Bytes & 0xF0) == 0xE0)
        { /* 3-byte sequence. */
            if ((Bytes[1] & 0xC0) != 0x80 || (Bytes[2] & 0xC0) != 0x80)
            {
                return false;
            }
            Bytes += 3;
        }
        else if ((*Bytes & 0xF8) == 0xF0)
        { /* 4-byte sequence. */
            if ((Bytes[1] & 0xC0) != 0x80 || (Bytes[2] & 0xC0) != 0x80 || (Bytes[3] & 0xC0) != 0x80)
            {
                return false;
            }
            Bytes += 4;
        }
        else
        { /* Invalid UTF-8. */
            return false;
        }
    }

    return true;
}

} /* ~Namespace Jafg::Testing::NativeLiterals */

TEST_CASE(NativeLiteralStringEcondings, "Lal.Native")
{
    const char* Utf8Test1 = "Utf-8 native literal ☺.";
    const char* Uft8Test2 = "これわテストです。";
#if PLATFORM_USES_UTF8
    CHECK_TRUE("Native UTF-8 encoding test", ::Jafg::Testing::NativeLiterals::IsUtf8(Utf8Test1))
    CHECK_TRUE("Native UTF-8 encoding test", ::Jafg::Testing::NativeLiterals::IsUtf8(Uft8Test2))
#else /* PLATFORM_USES_UTF8 */
    CHECK_TRUE("Test not implemented", false)
#endif /* !PLATFORM_USES_UTF8 */

    return;
}
