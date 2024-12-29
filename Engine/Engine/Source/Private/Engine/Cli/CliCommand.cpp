// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Cli/CliCommand.h"

Jafg::LSimpleString Jafg::LexToString(const ECommandReturnCode::Type& InType)
{
    switch (InType)
    {
    case ECommandReturnCode::Invalid:
    {
        return TEXT("Invalid");
    }
    case ECommandReturnCode::Success:
    {
        return TEXT("Success");
    }
    case ECommandReturnCode::SuccessNoResponse:
    {
        return TEXT("SuccessNoResponse");
    }
    case ECommandReturnCode::SuccessBroadcast:
    {
        return TEXT("SuccessBroadcast");
    }
    case ECommandReturnCode::SuccessBroadcastWithAuthority:
    {
        return TEXT("SuccessBroadcastWithAuthority");
    }
    case ECommandReturnCode::Failure:
    {
        return TEXT("Failure");
    }
    case ECommandReturnCode::Unknown:
    {
        return TEXT("Unknown");
    }
    case ECommandReturnCode::Forbidden:
    {
        return TEXT("Forbidden");
    }
    case ECommandReturnCode::MissingArgs:
    {
        return TEXT("MissingArgs");
    }
    case ECommandReturnCode::TooManyArgs:
    {
        return TEXT("TooManyArgs");
    }
    case ECommandReturnCode::SemanticError:
    {
        return TEXT("SemanticError");
    }
    case ECommandReturnCode::SyntaxError:
    {
        return TEXT("SyntaxError");
    }
    case ECommandReturnCode::PlatformError:
    {
        return TEXT("PlatformError");
    }
    default:
    {
        checkNoEntry()
        return TEXT("Unknown");
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
