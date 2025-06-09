// Copyright mzoesch. All rights reserved.

#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Cli/CommandLineInterface.h"
#include "Stats/Stats.h"

void Jafg::AddPrimitivesToCli(LCommandLineInterface* Cli)
{
    STAT_CYCLE_FUNCTION()

    check( Cli )

    ensure(Cli->RegisterType({"Integer", "A 64 bit signed integer.", "0",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        const LString& String = Args[*Cursor].Name;

        if (String.IsEmpty())
        {
            return false;
        }

        i64 Value;
        auto [ptr, ec]
        {
            std::from_chars
            (
                String.GetBegin(),
                String.GetEnd(),
                Value
            )
        };

        ++*Cursor;
        return ec == std::errc{} && ptr == String.GetEnd();
    })}).IsValid());

    ensure(Cli->RegisterType({"UInteger", "A 64 bit unsigned integer.", "0",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        const LString& String = Args[*Cursor].Name;

        if (String.IsEmpty())
        {
            return false;
        }

        u64 Value;
        auto [ptr, ec]
        {
            std::from_chars
            (
                String.GetBegin(),
                String.GetEnd(),
                Value
            )
        };

        ++*Cursor;
        return ec == std::errc{} && ptr == String.GetEnd();
    })}).IsValid());

    ensure(Cli->RegisterType({"Byte", "A 8 bit unsigned integer.", "0",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        const LString& String = Args[*Cursor].Name;

        if (String.IsEmpty())
        {
            return false;
        }

        i32 Value;
        auto [ptr, ec]
        {
            std::from_chars
            (
                String.GetBegin(),
                String.GetEnd(),
                Value
            )
        };

        ++*Cursor;
        return ec == std::errc{} && ptr == String.GetEnd() && (Value >= 0 && Value <= 255);
    })}).IsValid());

    ensure(Cli->RegisterType({"Float", "A 32 bit floating point number.", "0.0",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        const LString& String = Args[*Cursor].Name;

        if (String.IsEmpty())
        {
            return false;
        }

        f32 Value;
        auto [ptr, ec]
        {
            std::from_chars
            (
                String.GetBegin(),
                String.GetEnd(),
                Value
            )
        };

        ++*Cursor;
        return ec == std::errc{} && ptr == String.GetEnd();
    })}).IsValid());

    ensure(Cli->RegisterType({"String", "A string.", "",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        ++*Cursor;
        return true;
    })}).IsValid());

    ensure(Cli->RegisterType({"Bool", "A boolean.", "false",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        if
        (
               Args[*Cursor].Name.Equals("true")  == false
            && Args[*Cursor].Name.Equals("false") == false
            && Args[*Cursor].Name.Equals("1")     == false
            && Args[*Cursor].Name.Equals("0")     == false
        )
        {
            return false;
        }

        ++*Cursor;
        return true;
    }),
    LOnValueSetDelegate::CreateStrong([](const LCommandArgs& InValue, LString* OutValue) -> bool
    {
        check( InValue.IsValid() && InValue.Name.IsEmpty() == false )

        if (InValue.Name.Equals("true") || InValue.Name.Equals("1"))
        {
            if (*OutValue != "true")
            {
                *OutValue = "true";
                return true;
            }
        }
        else if (InValue.Name.Equals("false") || InValue.Name.Equals("0"))
        {
            if (*OutValue != "false")
            {
                *OutValue = "false";
                return true;
            }
        }

        return false;
    })}).IsValid());

    return;
}

void Jafg::AddExtendedPrimitivesToCli(LCommandLineInterface* Cli)
{
    STAT_CYCLE_FUNCTION()

    check( Cli )

    ensure(Cli->RegisterType({"Any", "Any value.", "",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        ++*Cursor;
        return true;
    })}).IsValid());

    ensure(Cli->RegisterType({"Var", "A variable.", "NULL",
    LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        ++*Cursor;
        return true;
    })}).IsValid());

    return;
}

bool Jafg::Private::CliQueryImpl(const LCommandArgs& Args, i32* Cursor, const TArray<LString>& Values)
{
    checkSlow( *Cursor < Args.GetArgCount() )
    if (Args[*Cursor].Name.IsEmpty())
    {
        return false;
    }

    if (Values.Contains(Args[*Cursor].Name))
    {
        ++*Cursor;
        return true;
    }

    return false;
}
