// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

namespace
{

bool StartsWith(const char* InString, const char* InPrefix) noexcept
{
    while (*InString == *InPrefix)
    {
        if (*InPrefix == '\0')
        {
            return true;
        }

        ++InString;
        ++InPrefix;
    }

    return false;
}

const char* ConsumeType(const char* Begin, const char* End) noexcept
{
    static const char* const Signed("signed");
    static const char* const Unsigned("unsigned");

    const char* It = Begin;
    if (::StartsWith(It, Signed))
    {
        It += ::strlen(Signed) + 1;
    }
    else if (::StartsWith(It, Unsigned))
    {
        It += strlen(Unsigned) + 1;
    }

    i32 TemplateNest = 0;
    while (It != End)
    {
        if (*It == ' ' && TemplateNest == 0)
        {
            break;
        }

        if (*It == '<')
        {
            ++TemplateNest;
        }
        else if (*It == '>' && TemplateNest > 0)
        {
            --TemplateNest;
        }

        ++It;
    }

    return It;
}

} /* ~Namespace <Anonymous> */

namespace Jafg::Private
{

std::string_view PrettyFunctionName(const char* InFunctionName) noexcept
{
    const char* Begin = InFunctionName;
    const char* End   = InFunctionName;
    while (*End != '\0')
    {
        ++End;
    }

    Begin = ::ConsumeType(Begin, End);
    if (Begin == End)
    {
        return InFunctionName;
    }

    /* Space after return type */
    ++Begin;
    if (Begin == End)
    {
        return InFunctionName;
    }

    if (*Begin == '(')
    {
        Begin = ::ConsumeType(++Begin, End);

        /* Space after return type */
        ++Begin;
        if (Begin == End)
        {
            return InFunctionName;
        }
    }

    return { Begin, static_cast<std::string_view::size_type>(std::find(Begin, End, '(') - Begin) };
}

LSimpleString LalLogPrivateColor_Trace   = LOG_COLOR_TRACE;
LSimpleString LalLogPrivateColor_Verbose = LOG_COLOR_VERBOSE;
LSimpleString LalLogPrivateColor_Info    = LOG_COLOR_INFO;
LSimpleString LalLogPrivateColor_Warning = LOG_COLOR_WARNING;
LSimpleString LalLogPrivateColor_Error   = LOG_COLOR_ERROR;
LSimpleString LalLogPrivateColor_Fatal   = LOG_COLOR_FATAL;
LSimpleString LalLogPrivateColor_End     = LOG_COLOR_END;

LStringLegacy GetColorForVerbosity(const ELogVerbosityType Verbosity)
{
    if (Verbosity == ELogVerbosity::Trace)   { return LalLogPrivateColor_Trace.ToC(); }
    if (Verbosity == ELogVerbosity::Verbose) { return LalLogPrivateColor_Verbose.ToC(); }
    if (Verbosity == ELogVerbosity::Info)    { return LalLogPrivateColor_Info.ToC(); }
    if (Verbosity == ELogVerbosity::Warning) { return LalLogPrivateColor_Warning.ToC(); }
    if (Verbosity == ELogVerbosity::Error)   { return LalLogPrivateColor_Error.ToC(); }
    if (Verbosity == ELogVerbosity::Fatal)   { return LalLogPrivateColor_Fatal.ToC(); }

    return "";
}

} /* ~Namespace Jafg::Private */

namespace Jafg
{

LSimpleString LexToString(const ELogVerbosityType Verbosity)
{
    switch (Verbosity)
    {
    case ELogVerbosity::Trace: { return "Trace"; }
    case ELogVerbosity::Verbose: { return "Verbose"; }
    case ELogVerbosity::Info: { return "Info"; }
    case ELogVerbosity::Warning: { return "Warning"; }
    case ELogVerbosity::Error: { return "Error"; }
    case ELogVerbosity::Fatal: { return "Fatal"; }
    case ELogVerbosity::KillLog: { return "KillLog"; }
    default: { checkNoEntry() return "Unknown"; }
    }
}

} /* ~Namespace Jafg */
