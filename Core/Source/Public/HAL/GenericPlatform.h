// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>

///////////////////////////////////////////////////////////////////////////////
// Maybe undefined features
#ifndef NODISCARD
    #define NODISCARD [[nodiscard]]
#endif /* !NODISCARD */
#ifndef FORCEINLINE
    #define FORCEINLINE _forceinline
#endif /* !FORCEINLINE */
// ~Maybe undefined features
///////////////////////////////////////////////////////////////////////////////

/** Generic types for almost all compilers and platforms. */
struct GenericPlatformTypes
{
    // ReSharper disable CppInconsistentNaming

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

    // ReSharper restore CppInconsistentNaming

    /** An ANSI character. 8-bit fixed-width representation of 7-bit characters. */
    typedef char                TAnsiChar;
    /**
     * A wide character. ?-bit fixed-width representation of the platform's natural wide character set. It could
     * be different sizes on different platforms.
     */
    typedef wchar_t             TWideChar;

    /** A switchable character - either AnsiChar or WideChar. */
    typedef TWideChar           TChar;

    typedef std::string         String;
};
