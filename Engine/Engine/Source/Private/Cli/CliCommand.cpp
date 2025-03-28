// Copyright mzoesch. All rights reserved.

#include "Cli/CliCommand.h"

Jafg::LSimpleString Jafg::LexToString(const ECommandReturnCode::Type& InType)
{
    switch (InType)
    {
    case ECommandReturnCode::Invalid:
    {
        return FMT("Invalid");
    }
    case ECommandReturnCode::Success:
    {
        return FMT("Success");
    }
    case ECommandReturnCode::SuccessNoResponse:
    {
        return FMT("SuccessNoResponse");
    }
    case ECommandReturnCode::SuccessBroadcast:
    {
        return FMT("SuccessBroadcast");
    }
    case ECommandReturnCode::SuccessBroadcastWithAuthority:
    {
        return FMT("SuccessBroadcastWithAuthority");
    }
    case ECommandReturnCode::Failure:
    {
        return FMT("Failure");
    }
    case ECommandReturnCode::Unknown:
    {
        return FMT("Unknown");
    }
    case ECommandReturnCode::Forbidden:
    {
        return FMT("Forbidden");
    }
    case ECommandReturnCode::NoMatchingOverload:
    {
        return FMT("NoMatchingOverload");
    }
    case ECommandReturnCode::SemanticError:
    {
        return FMT("SemanticError");
    }
    case ECommandReturnCode::SyntaxError:
    {
        return FMT("SyntaxError");
    }
    case ECommandReturnCode::PlatformError:
    {
        return FMT("PlatformError");
    }
    default:
    {
        checkNoEntry()
        return FMT("Unknown");
    }
    }
}

Jafg::LString Jafg::LCommandArgs::GetCatRepresentation() const
{
    LString Cat;
    this->GetCatRepresentation(&Cat);
    return Cat;
}

void Jafg::LCommandArgs::GetCatRepresentation(LString* AppendTo) const
{
    checkSlow( AppendTo )
    check( this->IsValid() )

    if (this->Name.IsEmpty() == false && AppendTo->IsEmpty() == false)
    {
        AppendTo->Append(" ");
    }
    AppendTo->Append(this->Name);

    for (const LCommandArgs& SubArg : this->SubArgs)
    {
        SubArg.GetCatRepresentation(AppendTo);
    }

    return;
}

bool Jafg::LCommandParams::IsInvocable(const LCommandArgs& Args) const
{
    if (this->Signature.GetSize() == 0 && Args.GetArgCount() == 0)
    {
        return true;
    }

    i32 ArgCursor = 0;
    for (const LCliType& Param : this->Signature)
    {
        if (ArgCursor >= Args.GetArgCount())
        {
            return false;
        }

        if (Param.CanParse(Args, &ArgCursor))
        {
            continue;
        }

        return false;
    }

    if (ArgCursor != Args.GetArgCount())
    {
        return false;
    }

    return this->Signature.GetSize() > 0;
}
