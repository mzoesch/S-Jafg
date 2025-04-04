// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

bool Jafg::Str::IsValidAscii(const char* InString, const i32 InRuneLengthToCheck)
{
    const u8* Bytes = reinterpret_cast<const u8*>(InString);

    i32 RuneCount = 0;
    while (*Bytes)
    {
        ++RuneCount;

        if ((*Bytes & 0x80) == 0)
        {
            ++Bytes;
        }
        else
        {
            return false;
        }

        if (InRuneLengthToCheck > 0 && RuneCount >= InRuneLengthToCheck)
        {
            break;
        }
    }

    return true;
}

bool Jafg::Str::IsValidUtf8(const char* InString, const i32 InRuneLengthToCheck)
{
    const u8* Bytes = reinterpret_cast<const u8*>(InString);

    i32 RuneCount = 0;
    while (*Bytes)
    {
        ++RuneCount;

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

        if (InRuneLengthToCheck > 0 && RuneCount >= InRuneLengthToCheck)
        {
            break;
        }
    }

    return true;
}

