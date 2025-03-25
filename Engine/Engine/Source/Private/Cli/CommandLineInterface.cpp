// Copyright mzoesch. All rights reserved.

#include "Cli/CommandLineInterface.h"
#include "Cli/CliStatics.h"

void Jafg::LCommandLineInterface::TearDown()
{
    LOG_VERBOSE(LogCli, "Tearing down command line interface with [{}] commands.", this->Commands.GetSize())
    this->UuidCursor = 0;
    this->Commands.Empty();

    return;
}

void Jafg::LCommandLineInterface::Invoke(const LString& InCommandLine, LCommandExecutionResponse* OutResponse)
{
    const LString ComplexCommandStr = CliStatics::GetCommandFromText(InCommandLine);
    if (ComplexCommandStr.IsEmpty())
    {
        if (OutResponse)
        {
            OutResponse->Rc = ECommandReturnCode::Failure;
            OutResponse->StdOut = "Failed to extract command from input";
        }
        return;
    }

    if (Str::IsValidAscii(ComplexCommandStr.ToC()) == false)
    {
        if (OutResponse)
        {
            OutResponse->Rc = ECommandReturnCode::Failure;
            OutResponse->StdOut = "Command contains invalid characters";
        }
        return;
    }
    LSimpleString SimpleCommandStr = Str::ToSimpleString(ComplexCommandStr, EConvErrorHandling::Panic);

    LCliCommand* Cmd = this->Commands.FindByPredicate([&SimpleCommandStr](const LCliCommand& CommandObj)
    {
        return CommandObj.GetIdentifier() == SimpleCommandStr;
    });
    if (Cmd == nullptr)
    {
        if (OutResponse)
        {
            OutResponse->Rc = ECommandReturnCode::Unknown;
            OutResponse->StdOut = "Command not found";
        }
        return;
    }

    if (Cmd->GetOverloadCount() == 0)
    {
        if (OutResponse)
        {
            OutResponse->Rc = ECommandReturnCode::Failure;
            OutResponse->StdOut = "Command has no overloads and is therefore not invocable";
        }
        return;
    }

    LString ChoppedArgs = CliStatics::GetArgsFromText(InCommandLine);
    LCommandArgs Args;
    CliStatics::TokenizeCommand(std::move(ChoppedArgs), &Args);

    for (LCommandParams& Params : Cmd->GetOverloads())
    {
        if (Params.IsInvocable(Args))
        {
            Params.Invoke(Args, OutResponse);
            return;
        }
        continue;
    }

    if (OutResponse)
    {
        OutResponse->Rc = ECommandReturnCode::SyntaxError;
        OutResponse->StdOut = "Target is not invocable with given arguments. No overload is matching";
    }

    return;
}

Jafg::LCliCommandHandle Jafg::LCommandLineInterface::RegisterCommand(LCliCommand&& InCommand)
{
    LOG_VERBOSE(LogCli, "Registering command [{}].", InCommand.GetIdentifier())
    check( InCommand.Uuid == LCliObject::NoUuid )
    InCommand.Uuid = ++this->UuidCursor;
    this->Commands.Add(std::move(InCommand));
    return { this->UuidCursor };
}

bool Jafg::LCommandLineInterface::UnregisterCommand(LCliCommandHandle* InHandle)
{
    check( InHandle )
    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid command handle.")
        return false;
    }

    if (const i32 Idx = this->Commands.FindIndexByPredicate([InHandle](const LCliCommand& Command)
    {
        return Command.Uuid == InHandle->Uuid;
    }); Idx != INDEX_NONE)
    {
        LOG_VERBOSE(LogCli, "Unregistering command [{}].", this->Commands[Idx].GetIdentifier())
        this->Commands.RemoveAt(Idx);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find command with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliVariableHandle Jafg::LCommandLineInterface::RegisterVariable(const LCliVariable& InVariable)
{
    return { };
}

bool Jafg::LCommandLineInterface::UnregisterVariable(const LCliVariableHandle InHandle)
{
    return false;
}

Jafg::LCliCommand* Jafg::LCommandLineInterface::GetCommand(const LSimpleString& InCommandName)
{
    return this->Commands.FindByPredicate([InCommandName](const LCliCommand& Command)
    {
        return Command.GetIdentifier() == InCommandName;
    });

    return nullptr;
}

const Jafg::LCliCommand* Jafg::LCommandLineInterface::GetCommand(const LSimpleString& InCommandName) const
{
    return const_cast<LCommandLineInterface*>(this)->GetCommand(InCommandName);
}

Jafg::LCliCommand* Jafg::LCommandLineInterface::GetCheckedCommand(const LSimpleString& InCommandName)
{
    LCliCommand* Command = this->GetCommand(InCommandName);
    check( Command )
    return Command;
}

const Jafg::LCliCommand* Jafg::LCommandLineInterface::GetCheckedCommand(const LSimpleString& InCommandName) const
{
    return const_cast<LCommandLineInterface*>(this)->GetCheckedCommand(InCommandName);
}

Jafg::LCliCommand* Jafg::LCommandLineInterface::GetPanickedCommand(const LSimpleString& InCommandName)
{
    if (LCliCommand* Command = this->GetCommand(InCommandName))
    {
        return Command;
    }
    panicMsgf( "Failed to find command [{}].", InCommandName )
    return nullptr;
}

const Jafg::LCliCommand* Jafg::LCommandLineInterface::GetPanickedCommand(const LSimpleString& InCommandName) const
{
    return const_cast<LCommandLineInterface*>(this)->GetPanickedCommand(InCommandName);
}
