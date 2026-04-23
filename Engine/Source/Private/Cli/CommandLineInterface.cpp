// Copyright mzoesch. All rights reserved.

#include "Cli/CommandLineInterface.h"
#include "Engine/Engine.h"
#include "Cli/CliStatics.h"
#include "Async/TaskUtility.h"
#include "Algo/Sort.h"

Jafg::LCommandLineInterface* Jafg::LCliObjectHandle::GetCommandLineInterface() const noexcept
{
    if (GMutableEngine)
    {
        return &GMutableEngine->GetCommandLineInterface();
    }
    return nullptr;
}

Jafg::LCommandLineInterface* Jafg::LCliObjectRaiiHandle::GetCommandLineInterface() const noexcept
{
    if (GMutableEngine)
    {
        return &GMutableEngine->GetCommandLineInterface();
    }
    return nullptr;
}


void Jafg::LCommandLineInterface::TearDown()
{
    LOG_VERBOSE(LogCli, "Tearing down command line interface with [{}] commands.", this->Commands.size())
    this->UuidCursor = 0;
    algo::orphan(&this->Commands);

    return;
}

void Jafg::LCommandLineInterface::Invoke(LCommandExecutionInfo const& Info, LString const& CommandLine, LCommandExecutionResponse* OutResponse /* = nullptr */)
{
    LCommandExecutionResponse Void;
    LCommandExecutionResponse& Response{OutResponse ? *OutResponse : Void};

    LString CommandStr{CliStatics::GetCommandFromText(CommandLine)};
    if (CommandStr.empty())
    {
        Response.Rc = ECommandReturnCode::Failure;
        Response.StdErr = "Failed to extract command from input";
        return;
    }

    LCliCommand* Cmd{this->GetCommand(CommandStr)};
    if (Cmd == nullptr)
    {
        Response.Rc = ECommandReturnCode::Unknown;
        Response.StdErr = Jafg::SprintF("No such command [{}]", CommandStr);
        return;
    }

    if (Cmd->GetOverloadCount() == 0)
    {
        Response.Rc = ECommandReturnCode::Failure;
        Response.StdErr = Jafg::SprintF("Command [{}] has no overloads and is therefore not invokable", CommandStr);
        return;
    }

    LString ChoppedArgs = CliStatics::GetArgsFromText(CommandLine);
    const LCommandArgs Args = CliStatics::TokenizeCommand(std::move(ChoppedArgs));
    for (LCommandParams& Params : Cmd->GetOverloads())
    {
        if (Params.IsInvocable(Args))
        {
            Params.Invoke(Info, Args, Response);
            check(Response.Rc != ECommandReturnCode::Invalid)
            return;
        }
        continue;
    }

    Response.Rc = ECommandReturnCode::SyntaxError;
    Response.StdErr = "Target is not invocable with given arguments. No overload is matching";

    return;
}

TArray<LString> Jafg::LCommandLineInterface::GetCommonSuggestions(const LString& InCommandLine, const u32 MaxSuggestions) const
{
    const LString CommandStr{ CliStatics::GetCommandFromText(InCommandLine) };
    if (CommandStr.empty())
    {
        LOG_ERROR(LogCli, "Failed to extract command from input");
        return {};
    }

    LCliCommand const* Cmd{ this->GetCommand(CommandStr) };
    if (Cmd == nullptr)
    {
        LOG_ERROR(LogCli, "No such command [{}]", CommandStr);
        return {};
    }

    if (Cmd->GetOverloadCount() == 0)
    {
        LOG_ERROR(LogCli, "Command [{}] has no overloads and is therefore not invokable", CommandStr);
        return {};
    }

    TArray<LString> Out;

    LString ChoppedArgs{ CliStatics::GetArgsFromText(InCommandLine) };
    const LCommandArgs Args{ CliStatics::TokenizeCommand(std::move(ChoppedArgs)) };
    for (LCommandParams const& Overloads : Cmd->GetOverloads())
    {
        if (Out.size() >= MaxSuggestions)
        {
            break;
        }

        Out.append_range(Overloads.GetCommonSuggestions(Args, MaxSuggestions - Out.size(), InCommandLine.ends_with(' ')));

        continue;
    }

    return Out;
}

Jafg::LCliTypeHandle Jafg::LCommandLineInterface::RegisterType(LCliType&& InType)
{
    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogCli, "Registering type [{}].", InType.GetIdentifier())
    check( InType.Uuid == LCliObject::NoUuid )

    if (InType.Identifier.empty())
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
    this->Types.emplace_back(std::move(InType));

    Algo::SortQuick(&this->Types);

    return LCliCommandHandle{ this->UuidCursor };
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

    if (auto It { algo::find(this->Types, InHandle->Uuid, &LCliType::Uuid) }; It != this->Types.end())
    {
        LOG_VERBOSE(LogCli, "Unregistering type [{}].", It->GetIdentifier())
        this->Types.erase(It);
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

    if (InCommand.Identifier.empty())
    {
        LOG_ERROR(LogCli, "Failed to register command. Identifier is empty.")
        return {};
    }
    if (this->GetObject(InCommand.Identifier))
    {
        LOG_ERROR(LogCli, "Failed to register command. Identifier [{}] already in use.", InCommand.GetIdentifier())
        return {};
    }

    InCommand.Uuid = ++this->UuidCursor;
    this->Commands.emplace_back(std::move(InCommand));

    Algo::SortQuick(&this->Commands);

    return LCliCommandHandle{ this->UuidCursor };
}

bool Jafg::LCommandLineInterface::UnregisterCommand(LCliCommandHandle* InHandle)
{
    check(Tasks::IsOnMasterThread())
    check(InHandle)
    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid command handle.")
        return false;
    }

    if (auto It{algo::find(this->Commands, InHandle->Uuid, &LCliCommand::Uuid)}; It != this->Commands.end())
    {
        LOG_VERBOSE(LogCli, "Unregistering command [{}].", It->GetIdentifier())
        this->Commands.erase(It);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find command with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliVariableHandle Jafg::LCommandLineInterface::RegisterVariable(LCliVariable&& InVariable)
{
    check(Tasks::IsOnMasterThread())
    LOG_VERBOSE(LogCli, "Registering variable [{}].", InVariable.GetIdentifier())
    check(InVariable.Uuid == LCliObject::NoUuid)

    if (InVariable.Identifier.empty())
    {
        LOG_ERROR(LogCli, "Failed to register variable. Identifier is empty.")
        return {};
    }
    if (this->GetObject(InVariable.Identifier))
    {
        LOG_ERROR(LogCli, "Failed to register variable. Identifier [{}] already in use.", InVariable.GetIdentifier())
        return {};
    }

    InVariable.Uuid = ++this->UuidCursor;
    this->Variables.emplace_back(std::move(InVariable));

    Algo::SortQuick(&this->Variables);

    return LCliCommandHandle{this->UuidCursor};
}

bool Jafg::LCommandLineInterface::UnregisterVariable(LCliVariableHandle* InHandle)
{
    check(Tasks::IsOnMasterThread())
    check(InHandle)
    if (InHandle->IsValid() == false)
    {
        LOG_WARNING(LogCli, "Received invalid variable handle.")
        return false;
    }

    if (auto It{algo::find(this->Variables, InHandle->Uuid, &LCliVariable::Uuid)}; It != this->Variables.end())
    {
        LOG_VERBOSE(LogCli, "Unregistering variable [{}].", It->GetIdentifier())
        this->Variables.erase(It);
        InHandle->Reset();
        return true;
    }

    LOG_WARNING(LogCli, "Failed to find variable with handle [{}].", InHandle->Uuid)
    return false;
}

Jafg::LCliObject* Jafg::LCommandLineInterface::GetObject(const LCliObject& InHandle)
{
    if (LCliType* Type{this->GetType(LCliTypeHandle{InHandle.GetUuid()})}; Type)
    {
        return Type;
    }

    if (LCliCommand* Command = this->GetCommand(LCliTypeHandle{InHandle.GetUuid()}); Command)
    {
        return Command;
    }

    return this->GetVariable(LCliTypeHandle{InHandle.GetUuid()});
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
