// Copyright mzoesch. All rights reserved.

#include "Cli/CommandLineInterface.h"
#include "Engine/Engine.h"
#include "Cli/CliStatics.h"
#include "Async/TaskUtility.h"
#include "Algo/Sort.h"

Jafg::LCommandLineInterface* Jafg::LCliObjectHandle::GetCommandLineInterface() const
{
    return GEngine->GetCommandLineInterface();
}

void Jafg::LCommandLineInterface::TearDown()
{
    LOG_VERBOSE(LogCli, "Tearing down command line interface with [{}] commands.", this->Commands.GetSize())
    this->UuidCursor = 0;
    this->Commands.Empty();

    return;
}

void Jafg::LCommandLineInterface::Invoke(const LString& InCommandLine, LCommandExecutionResponse* OutResponse)
{
    check( OutResponse )

    const LString CommandStr = CliStatics::GetCommandFromText(InCommandLine);
    if (CommandStr.IsEmpty())
    {
        OutResponse->Rc = ECommandReturnCode::Failure;
        OutResponse->StdErr = "Failed to extract command from input";
        return;
    }

    if (Str::IsValidAscii(CommandStr.ToPtr()) == false)
    {
        OutResponse->Rc = ECommandReturnCode::Failure;
        OutResponse->StdErr = "Command contains invalid characters";
        return;
    }

    LCliCommand* Cmd = this->GetCommand(CommandStr);
    if (Cmd == nullptr)
    {
        OutResponse->Rc = ECommandReturnCode::Unknown;
        OutResponse->StdErr = LString::SprintF("No such command [{}]", CommandStr);
        return;
    }

    if (Cmd->GetOverloadCount() == 0)
    {
        OutResponse->Rc = ECommandReturnCode::Failure;
        OutResponse->StdErr = LString::SprintF("Command [{}] has no overloads and is therefore not invokable", CommandStr);
        return;
    }

    LString ChoppedArgs = CliStatics::GetArgsFromText(InCommandLine);
    const LCommandArgs Args = CliStatics::TokenizeCommand(std::move(ChoppedArgs));
    for (LCommandParams& Params : Cmd->GetOverloads())
    {
        if (Params.IsInvocable(Args))
        {
            Params.Invoke(Args, OutResponse);
            check( OutResponse->Rc != ECommandReturnCode::Invalid )

            return;
        }
        continue;
    }

    OutResponse->Rc = ECommandReturnCode::SyntaxError;
    OutResponse->StdErr = "Target is not invocable with given arguments. No overload is matching";

    return;
}

TArray<Jafg::LString> Jafg::LCommandLineInterface::GetCommonSuggestions(const LString& InCommandLine, const u32 MaxSuggestions) const
{
    const LString CommandStr = CliStatics::GetCommandFromText(InCommandLine);
    if (CommandStr.IsEmpty())
    {
        LOG_ERROR(LogCli, "Failed to extract command from input");
        return { };
    }

    if (Str::IsValidAscii(CommandStr.ToPtr()) == false)
    {
        LOG_ERROR(LogCli, "Command contains invalid characters" );
        return { };
    }

    const LCliCommand* Cmd = this->GetCommand(CommandStr);
    if (Cmd == nullptr)
    {
        LOG_ERROR(LogCli, "No such command [{}]", CommandStr);
        return { };
    }

    if (Cmd->GetOverloadCount() == 0)
    {
        LOG_ERROR(LogCli, "Command [{}] has no overloads and is therefore not invokable", CommandStr);
        return { };
    }

    TArray<LString> Out;

    LString ChoppedArgs { CliStatics::GetArgsFromText(InCommandLine) };
    const LCommandArgs Args { CliStatics::TokenizeCommand(std::move(ChoppedArgs)) };
    for (const LCommandParams& Overloads : Cmd->GetOverloads())
    {
        if (Out.GetSize() >= MaxSuggestions)
        {
            break;
        }

        Out.Append(Overloads.GetCommonSuggestions(Args, MaxSuggestions - Out.GetSize(), InCommandLine.EndsWith(' ')));

        continue;
    }

    return Out;
}

Jafg::LCliTypeHandle Jafg::LCommandLineInterface::RegisterType(LCliType&& InType)
{
    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogCli, "Registering type [{}].", InType.GetIdentifier())
    check( InType.Uuid == LCliObject::NoUuid )

    if (InType.Identifier.IsEmpty())
    {
        LOG_ERROR(LogCli, "Failed to register type. Identifier is empty.")
        return { };
    }
    if (this->GetObject(InType.Identifier))
    {
        LOG_ERROR(LogCli, "Failed to register type. Identifier [{}] already in use.", InType.GetIdentifier())
        return { };
    }

    InType.Uuid = ++this->UuidCursor;
    this->Types.Emplace(std::move(InType));

    Algo::SortQuick(&this->Types);

    return { this->UuidCursor };
}

bool Jafg::LCommandLineInterface::UnregisterType(LCliTypeHandle* InHandle)
{
    check( Tasks::IsOnMasterThread() )
    check( InHandle )

    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid type handle.")
        return false;
    }

    if (const TArray<LCliVariable>::SizeType Idx { this->Types.FindIndexByPredicate([InHandle](const LCliType& Type)
    {
        return Type.Uuid == InHandle->Uuid;
    })}; Idx != this->Types.end_idx())
    {
        LOG_VERBOSE(LogCli, "Unregistering type [{}].", this->Types[Idx].GetIdentifier())
        this->Types.RemoveAt(Idx);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find type with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliCommandHandle Jafg::LCommandLineInterface::RegisterCommand(LCliCommand&& InCommand)
{
    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogCli, "Registering command [{}].", InCommand.GetIdentifier())
    check( InCommand.Uuid == LCliObject::NoUuid )

    if (InCommand.Identifier.IsEmpty())
    {
        LOG_ERROR(LogCli, "Failed to register command. Identifier is empty.")
        return { };
    }
    if (this->GetObject(InCommand.Identifier))
    {
        LOG_ERROR(LogCli, "Failed to register command. Identifier [{}] already in use.", InCommand.GetIdentifier())
        return { };
    }

    InCommand.Uuid = ++this->UuidCursor;
    this->Commands.Emplace(std::move(InCommand));

    Algo::SortQuick(&this->Commands);

    return { this->UuidCursor };
}

bool Jafg::LCommandLineInterface::UnregisterCommand(LCliCommandHandle* InHandle)
{
    check( Tasks::IsOnMasterThread() )
    check( InHandle )
    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid command handle.")
        return false;
    }

    if (const TArray<LCliVariable>::SizeType Idx { this->Commands.FindIndexByPredicate([InHandle](const LCliCommand& Command)
    {
        return Command.Uuid == InHandle->Uuid;
    })}; Idx != this->Commands.end_idx())
    {
        LOG_VERBOSE(LogCli, "Unregistering command [{}].", this->Commands[Idx].GetIdentifier())
        this->Commands.RemoveAt(Idx);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find command with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliVariableHandle Jafg::LCommandLineInterface::RegisterVariable(LCliVariable&& InVariable)
{
    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogCli, "Registering variable [{}].", InVariable.GetIdentifier())
    check( InVariable.Uuid == LCliObject::NoUuid )

    if (InVariable.Identifier.IsEmpty())
    {
        LOG_ERROR(LogCli, "Failed to register variable. Identifier is empty.")
        return { };
    }
    if (this->GetObject(InVariable.Identifier))
    {
        LOG_ERROR(LogCli, "Failed to register variable. Identifier [{}] already in use.", InVariable.GetIdentifier())
        return { };
    }

    InVariable.Uuid = ++this->UuidCursor;
    this->Variables.Emplace(std::move(InVariable));

    Algo::SortQuick(&this->Variables);

    return { this->UuidCursor };
}

bool Jafg::LCommandLineInterface::UnregisterVariable(LCliVariableHandle* InHandle)
{
    check( Tasks::IsOnMasterThread() )
    check( InHandle )
    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid variable handle.")
        return false;
    }

    if (const TArray<LCliVariable>::SizeType Idx { this->Variables.FindIndexByPredicate([InHandle](const LCliVariable& Variable)
    {
        return Variable.Uuid == InHandle->Uuid;
    })}; Idx != this->Variables.end_idx())
    {
        LOG_VERBOSE(LogCli, "Unregistering variable [{}].", this->Variables[Idx].GetIdentifier())
        this->Variables.RemoveAt(Idx);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find variable with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LCliObject& InHandle)
{
    if (LCliType* Type = this->GetType(InHandle.Uuid); Type)
    {
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(InHandle.Uuid); Command)
    {
        return Command;
    }

    return this->GetVariable(InHandle.Uuid);
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LCliObjectHandle& InHandle)
{
    if (LCliType* Type = this->GetType(InHandle); Type)
    {
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(InHandle); Command)
    {
        return Command;
    }

    return this->GetVariable(InHandle);
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LCliObjectHandle& InHandle, ECliType::Type* OutType)
{
    if (LCliType* Type = this->GetType(InHandle); Type)
    {
        *OutType = ECliType::Type::Ty;
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(InHandle); Command)
    {
        *OutType = ECliType::Type::Command;
        return Command;
    }

    *OutType = ECliType::Type::Variable;
    return this->GetVariable(InHandle);
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LString& InName)
{
    if (LCliType* Type = this->GetType(InName); Type)
    {
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(InName); Command)
    {
        return Command;
    }

    return this->GetVariable(InName);
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LString& InName, ECliType::Type* OutType)
{
    check( OutType )

    if (LCliType* Type = this->GetType(InName); Type)
    {
        *OutType = ECliType::Type::Ty;
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(InName); Command)
    {
        *OutType = ECliType::Type::Command;
        return Command;
    }

    *OutType = ECliType::Type::Variable;
    return this->GetVariable(InName);
}
