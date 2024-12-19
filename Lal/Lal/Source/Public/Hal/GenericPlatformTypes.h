// Copyright mzoesch. All rights reserved.

#pragma once

#include <codecvt>

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features for almost all compilers and platforms.

#ifndef NODISCARD
    #define NODISCARD           [[nodiscard]]
#endif /* !NODISCARD */

#ifndef UNUSED
    #define UNUSED              [[maybe_unused]]
#endif /* !UNUSED */

#ifndef NORETURN
    #define NORETURN            [[noreturn]]
#endif /* !NORETURN */

#ifndef RESTRICT
    #define RESTRICT            __restrict
#endif /* !RESTRICT */

#ifndef LITERAL_WIDE
    #define LITERAL_WIDE(x)     L##x
#endif /* !LITERAL_WIDE */
#ifndef LITERAL_UTF8
    #define LITERAL_UTF8(x)     u8##x
#endif /* !LITERAL_UTF8 */
#ifndef LITERAL_UTF16
    #define LITERAL_UTF16(x)    u##x
#endif /* !LITERAL_UTF16 */
#ifndef LITERAL_UTF32
    #define LITERAL_UTF32(x)    U##x
#endif /* !LITERAL_UTF32 */

// ~Compiler dependent features for almost all compilers and platforms.
///////////////////////////////////////////////////////////////////////////////

/** Generic types for almost all compilers and platforms. */
struct LGenericPlatformTypes
{
    /** 8-bit unsigned integer */
    typedef unsigned char       uint8;
    /** 16-bit unsigned integer */
    typedef unsigned short int  uint16;
    /** 32-bit unsigned integer */
    typedef unsigned int        uint32;
    /** 64-bit unsigned integer */
    typedef unsigned long long  uint64;

    /** 8-bit signed integer */
    typedef signed char         int8;
    /** 16-bit signed integer  */
    typedef signed short int    int16;
    /** 32-bit signed integer */
    typedef signed int          int32;
    /** 64-bit signed integer */
    typedef signed long long    int64;

    /** An ASCII character. 8-bit fixed-width representation of 7-bit characters. */
    typedef char                LAsciiChar;
    /**
     * A wide character. ?-bit fixed-width representation of the platform's natural wide character set. It could
     * be different sizes on different platforms.
     */
    typedef wchar_t             LWideChar;

    typedef std::wstring        LWideString;
    typedef std::string         LStringLegacy;

    /** No other type is more aligned than double. */
    typedef double              LMaxAlign;
    typedef decltype(nullptr)   LNullptrTy;
};
