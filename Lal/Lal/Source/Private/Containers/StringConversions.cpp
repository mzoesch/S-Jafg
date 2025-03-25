// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

Jafg::LSimpleString Jafg::Str::ToSimpleString
(
    const LEightString& InEightString,
    const EConvErrorHandling::Type InErrorHandling /*= EConvErrorHandling::Ignore*/,
    const char* InReplacement /*= "?"*/
)
{
    LSimpleString OutString;

    for (const LEightString::CharacterTy& Char : InEightString)
    {
        if (Char == LEightString::Terminator)
        {
            break;
        }

        if (Str::IsValidAscii(&Char, 1))
        {
            OutString.Add(Char);
            continue;
        }

        if (InErrorHandling & EConvErrorHandling::Emit)
        {
            OutString.Add(Char);
        }

        if (InErrorHandling & EConvErrorHandling::Warn)
        {
            LOG_WARNING
            (
                LogJafgInternal,
                "Invalid character in string: [{}].",
                static_cast<i32>(*reinterpret_cast<const u8*>(&Char))
            )
        }

        if (InErrorHandling & EConvErrorHandling::Error)
        {
            LOG_ERROR
            (
                LogJafgInternal,
                "Invalid character in string: [{}].",
                static_cast<i32>(*reinterpret_cast<const u8*>(&Char))
            )
        }

        if (InErrorHandling & EConvErrorHandling::Panic)
        {
            LOG_FATAL
            (
                LogJafgInternal,
                "Invalid character in string: [{}].",
                static_cast<i32>(*reinterpret_cast<const u8*>(&Char))
            )
        }

        if (InErrorHandling & EConvErrorHandling::Replace)
        {
            OutString.Append(InReplacement);
        }

        continue;
    }

    return OutString;
}

Jafg::LEightString Jafg::Str::ToEightString(const LSimpleString& InSimpleString)
{
    return { InSimpleString.ToPtr() };
}
