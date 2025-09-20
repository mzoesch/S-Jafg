// Copyright mzoesch. All rights reserved.

#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Cli/CommandLineInterface.h"
#include "Engine/Engine.h"
#include "Stats/Stats.h"

void Jafg::Private::AddPrimitivesToCli(LCommandLineInterface* Cli)
{
    STAT_CYCLE_FUNCTION()

    check( Cli )

    const bool bValid_TypeInteger { Cli->RegisterType({"Integer", "A 64 bit signed integer.", "0",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
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
                String.begin_ptr(),
                String.end_ptr(),
                Value
            )
        };

        if (ec == std::errc{} && ptr == String.end_ptr())
        {
            ++*Cursor;
            return true;
        }

        return false;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeInteger);

    const bool bValid_TypeUInteger { Cli->RegisterType({"UInteger", "A 64 bit unsigned integer.", "0",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
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
                String.begin_ptr(),
                String.end_ptr(),
                Value
            )
        };

        if (ec == std::errc{} && ptr == String.end_ptr())
        {
            ++*Cursor;
            return true;
        }

        return false;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeUInteger);

    const bool bValid_TypeByte { Cli->RegisterType({"Byte", "A 8 bit unsigned integer.", "0",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
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
                String.begin_ptr(),
                String.end_ptr(),
                Value
            )
        };

        if (ec == std::errc{} && ptr == String.end_ptr() && (Value >= 0 && Value <= 255))
        {
            ++*Cursor;
            return true;
        }

        return false;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        if (MaxSuggestions >= 2)
        {
            return { "255", "0" };
        }

        if (MaxSuggestions == 1)
        {
            return { "255" };
        }

        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeByte);

    const bool bValid_TypeFloat { Cli->RegisterType({"Float", "A 32 bit floating point number.", "0.0",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
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
                String.begin_ptr(),
                String.end_ptr(),
                Value
            )
        };

        if (ec == std::errc{} && ptr == String.end_ptr())
        {
            ++*Cursor;
            return true;
        }

        return false;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeFloat);

    const bool bValid_TypeString { Cli->RegisterType({"String", "A string.", "",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        ++*Cursor;
        return true;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeString);

    const bool bValid_TypeBool { Cli->RegisterType({"Bool", "A boolean.", "false",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
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
    },
    [](const LCommandArgs& InValue, LString* OutValue) -> bool
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
    },
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        if (MaxSuggestions >= 2)
        {
            return { "true", "false" };
        }

        if (MaxSuggestions == 1)
        {
            return { "true" };
        }

        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeBool);

    return;
}

void Jafg::Private::AddExtendedPrimitivesToCli(LCommandLineInterface* Cli)
{
    STAT_CYCLE_FUNCTION()

    check( Cli )

    const bool bValid_TypeAny { Cli->RegisterType({"Any", "Any value.", "",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        ++*Cursor;
        return true;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        return { };
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeAny);

    const bool bValid_TypeVar { Cli->RegisterType({"Var", "A variable.", "NULL",
    [](const LCommandArgs& Args, i32* Cursor) -> bool
    {
        checkSlow( *Cursor < Args.GetArgCount() )
        if (Args[*Cursor].Name.IsEmpty())
        {
            return false;
        }

        if (GEngine == nullptr)
        {
            return false;
        }

        const LCliVariable* Var { GEngine->GetCommandLineInterface()->GetVariable(Args[*Cursor].Name) };

        if (Var)
        {
            ++*Cursor;
        }

        return Var != nullptr;
    },
    nullptr,
    [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
    {
        const LCommandArgs* Target { nullptr };

        if (Args.SubArgs.IsValidIndex(Cursor))
        {
            Target = &Args[Cursor];
        }

        if (GEngine == nullptr)
        {
            return { };
        }

        TArray<LString> Out;

        for (const LCliVariable& Var : GEngine->GetCommandLineInterface()->GetVariables())
        {
            if (Out.GetSize() >= static_cast<TArray<LString>::SizeType>(MaxSuggestions))
            {
                break;
            }

            if (Target)
            {
                if (Var.GetIdentifier().StartsWith(Target->Name))
                {
                    Out.Emplace(Var.GetIdentifier());
                }
            }
            else
            {
                Out.Emplace(Var.GetIdentifier());
            }

            continue;
        }

        return Out;
    },
    }).IsValid()};
    ensureDiscard(bValid_TypeVar);

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

TArray<LString> Jafg::Private::CliQuerySuggestImpl(const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions, const TArray<LString>& Values)
{
    const LCommandArgs* Target { nullptr };

    if (Args.SubArgs.IsValidIndex(Cursor))
    {
        Target = &Args[Cursor];
    }

    TArray<LString> Out;

    for (const LString& Value : Values)
    {
        if (Out.GetSize() >= MaxSuggestions)
        {
            break;
        }

        if (Target)
        {
            if (Value.StartsWith(Target->Name))
            {
                Out.Emplace(Value);
            }
        }
        else
        {
            Out.Emplace(Value);
        }

        continue;
    }

    return Out;
}

bool Jafg::Private::CliStringImpl(const LCommandArgs& Args, i32* Cursor, const LString& Value)
{
    checkSlow( *Cursor < Args.GetArgCount() )
    if (Args[*Cursor].Name.IsEmpty())
    {
        return false;
    }

    if (Args[*Cursor].Name == Value)
    {
        ++*Cursor;
        return true;
    }

    return false;
}

TArray<LString> Jafg::Private::CliStringSuggestImpl(const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions, const LString& Value)
{
    if (MaxSuggestions > 0)
    {
        if (Args.SubArgs.IsValidIndex(Cursor))
        {
            if (Value.StartsWith(Args[Cursor].Name))
            {
                return { Value };
            }
        }
        else
        {
            return { Value };
        }
    }

    return { };
}
