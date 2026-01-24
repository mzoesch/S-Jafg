// Copyright mzoesch. All rights reserved.

#include "Cli/CliType.h"
#include "Engine/Engine.h"

bool Jafg::LCliType::SetVariable(const LCommandArgs& InValue, LString* OutValue) const
{
    check( OutValue )

    if (this->OnValueSet.IsValid())
    {
        return this->OnValueSet(InValue, OutValue);
    }

    if (InValue.Name == *OutValue)
    {
        return false;
    }

    *OutValue = InValue.Name;
    return true;
}

bool Jafg::LCliType::CanParse(const LCommandArgs& Args, i32* Cursor) const
{
    check( Cursor )
    if (this->OnParseType.IsValid())
    {
        return this->OnParseType.Invoke(Args, Cursor);
    }

    check( GEngine )
    const LCliType* CliType{ GEngine->GetCommandLineInterface().GetTypeAsserted(*this) };

    if (this == CliType)
    {
        LOG_ERROR(LogCli, "Infinite recursive call detected for args [{}] on type [{}].", Args.GetCatRepresentation(), this->GetIdentifier())
        return false;
    }

    return CliType->OnParseType.Invoke(Args, Cursor);
}

TArray<LString> Jafg::LCliType::Suggest(const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) const
{
    if (this->OnSuggest.IsValid())
    {
        return this->OnSuggest.Invoke(Args, Cursor, MaxSuggestions);
    }

    check( GEngine )
    const LCliType* CliType{ GEngine->GetCommandLineInterface().GetType(*this) };

    if (CliType == nullptr)
    {
        return { };
    }

    if (this == CliType)
    {
        /*
         * Infinite recursion is okay, as not all types may have a suggestion delegate. This is completly okay.
         */
        return { };
    }

    return CliType->OnSuggest.Invoke(Args, Cursor, MaxSuggestions);
}
