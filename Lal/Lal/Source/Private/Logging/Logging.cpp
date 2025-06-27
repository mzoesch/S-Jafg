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

namespace Lal
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

Jafg::LString LexToString(const ELogVerbosity::Type Verbosity)
{
    switch (Verbosity)
    {
    case ELogVerbosity::Trace:      { return "Trace"; }
    case ELogVerbosity::Verbose:    { return "Verbose"; }
    case ELogVerbosity::Info:       { return "Info"; }
    case ELogVerbosity::Warning:    { return "Warning"; }
    case ELogVerbosity::Error:      { return "Error"; }
    case ELogVerbosity::Fatal:      { return "Fatal"; }
    case ELogVerbosity::KillLog:    { return "KillLog"; }
    default: { checkNoEntry() return "Unknown"; }
    }
}

LAL_API Jafg::LString LalColor_Trace   { LAL_LOG_COLOR_TRACE };
LAL_API Jafg::LString LalColor_Verbose { LAL_LOG_COLOR_VERBOSE };
LAL_API Jafg::LString LalColor_Info    { LAL_LOG_COLOR_INFO };
LAL_API Jafg::LString LalColor_Warning { LAL_LOG_COLOR_WARNING };
LAL_API Jafg::LString LalColor_Error   { LAL_LOG_COLOR_ERROR };
LAL_API Jafg::LString LalColor_Fatal   { LAL_LOG_COLOR_FATAL };
LAL_API Jafg::LString LalColor_End     { LAL_LOG_COLOR_END };

Jafg::LString GetColorForVerbosity(const ELogVerbosity::Type InVerbosity)
{
    if (InVerbosity == ELogVerbosity::Trace)   { return LalColor_Trace; }
    if (InVerbosity == ELogVerbosity::Verbose) { return LalColor_Verbose; }
    if (InVerbosity == ELogVerbosity::Info)    { return LalColor_Info; }
    if (InVerbosity == ELogVerbosity::Warning) { return LalColor_Warning; }
    if (InVerbosity == ELogVerbosity::Error)   { return LalColor_Error; }
    if (InVerbosity == ELogVerbosity::Fatal)   { return LalColor_Fatal; }

    return { };
}

std::string_view GetColorForVerbosityView(const ELogVerbosity::Type InVerbosity)
{
    if (InVerbosity == ELogVerbosity::Trace)   { return LAL_LOG_COLOR_TRACE; }
    if (InVerbosity == ELogVerbosity::Verbose) { return LAL_LOG_COLOR_VERBOSE; }
    if (InVerbosity == ELogVerbosity::Info)    { return LAL_LOG_COLOR_INFO; }
    if (InVerbosity == ELogVerbosity::Warning) { return LAL_LOG_COLOR_WARNING; }
    if (InVerbosity == ELogVerbosity::Error)   { return LAL_LOG_COLOR_ERROR; }
    if (InVerbosity == ELogVerbosity::Fatal)   { return LAL_LOG_COLOR_FATAL; }

    return { };
}

} /* ~Namespace Lal */
