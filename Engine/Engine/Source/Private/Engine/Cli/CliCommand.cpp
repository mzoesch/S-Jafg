// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Cli/CliCommand.h"

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
    case ECommandReturnCode::MissingArgs:
    {
        return FMT("MissingArgs");
    }
    case ECommandReturnCode::TooManyArgs:
    {
        return FMT("TooManyArgs");
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
    for (const LCliToken* Arg : this->Args)
    {
        if (Cat.IsEmpty() == false)
        {
            Cat += " ";
        }
        Cat += Arg->GetStringRepresentation();
    }

    return Cat;
}

bool Jafg::LCommandParams::IsInvocable(const LCommandArgs& Args) const
{
    int32 ParamCursor = 0;
    int32 ArgCursor   = 0;

    while (ParamCursor < this->Tokens.GetSize())
    {
        if (ArgCursor >= Args.GetArgCount())
        {
            return false;
        }

        LCliToken* const& Param = this->Tokens[ParamCursor];
        if (Param->IsInvocable(Args.Args, &ArgCursor))
        {
            ++ParamCursor;
            continue;
        }

        return false;
    }

    return this->Tokens.GetSize() != 0;
}

void Jafg::LCommandParams::Invoke(const LCommandArgs& Args, LCommandExecutionResponse* OutResponse)
{
    this->OnExec.Invoke(Args, OutResponse);
}
