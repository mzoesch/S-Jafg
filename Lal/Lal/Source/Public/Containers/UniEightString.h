// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * Jafg interpretation of a simple Unicode string.
 * A string is mutable and can therefore be modified in place.
 * Strings are null-terminated and can therefore be used in C-style functions.
 */
class LUniEightString
{
    using SizeType = int32;
    /**
     * A rune is a single grapheme represented by a single byte.
     * Said grapheme has to be a valid ascii character.
     */
    using Rune     = uint8;

    inline static uint8 StringTerminator     = '\0';
    inline static char  StringTerminatorChar = '\0';

public:

};

} /* ~Namespace Jafg */
