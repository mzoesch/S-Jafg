// Copyright mzoesch. All rights reserved.

#pragma once

#include <codecvt>

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features

#ifndef NODISCARD
    #define NODISCARD           [[nodiscard]]
#endif /* !NODISCARD */

#ifndef FORCEINLINE
    #define FORCEINLINE         _forceinline
#endif /* !FORCEINLINE */

#ifndef RESTRICT
    #define RESTRICT            __restrict
#endif /* !RESTRICT */

#ifndef LITERAL_WIDE
    #define LITERAL_WIDE(x)     L##x
#endif /* !LITERAL_WIDE */

// ~Compiler dependent features
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

    /** An ANSI character. 8-bit fixed-width representation of 7-bit characters. */
    typedef char                LAnsiChar;
    /**
     * A wide character. ?-bit fixed-width representation of the platform's natural wide character set. It could
     * be different sizes on different platforms.
     */
    typedef wchar_t             LWideChar;

    /** A switchable character - either AnsiChar or WideChar. */
    typedef LAnsiChar           LChar;

    typedef std::wstring        LWideString;
    typedef std::string         LString;

    static LString Ws2S(const LWideString& Ws)
    {
        __pragma( warning(push) )
        __pragma( warning(disable: 4996) )

        typedef std::codecvt_utf8<wchar_t> TypeX;
        std::wstring_convert<TypeX, wchar_t> Converter;
        return Converter.to_bytes(Ws);

        __pragma( warning(pop) )
    }

    /** @return You are the owner. Plz delete! */
    static const LChar* Ws2CStr(const LWideString& Ws)
    {
        const LString* StrPtr = new LString(Ws2S(Ws));
        return StrPtr->c_str();
    }
};
