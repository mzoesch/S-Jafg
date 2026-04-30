// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"
#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
#include "Cli/CliVariable.h"
#include "Cli/CliHandles.h"

namespace Jafg
{

//#
//# The command line interface of the engine.
//#
class LCommandLineInterface final
{
public:

    LCommandLineInterface() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCommandLineInterface)
    ~LCommandLineInterface() { this->TearDown(); }

    ENGINE_API void TearDown();

    //#
    //# Invoke a command.
    //#
    ENGINE_API void Invoke(LCommandExecutionInfo const& Info, LString const& CommandLine, LCommandExecutionResponse* OutResponse = nullptr);

    //#
    //# Based of the current state of the command line interface, this method returns a list of suggestions
    //# that are common for the given command line.
    //# The first index is the most common suggestion, the last index is the least common suggestion.
    //#
    ENGINE_API TArray<LString> GetCommonSuggestions(const LString& CommandLine, const u32 MaxSuggestions) const;

    ENGINE_API  LCliTypeHandle RegisterType(LCliType&& InType);
    FORCEINLINE LCliTypeHandle RegisterTypeChecked(LCliType&& InType) { auto Result{this->RegisterType(std::move(InType))}; check(Result.IsValid()) return Result; }
    FORCEINLINE LCliTypeRaiiHandle RegisterTypeRaii(LCliType&& InType) { return LCliTypeRaiiHandle{this->RegisterType(std::move(InType))}; }
    FORCEINLINE LCliTypeRaiiHandle RegisterTypeRaiiChecked(LCliType&& InType) { return LCliTypeRaiiHandle{this->RegisterTypeChecked(std::move(InType))}; }
    ENGINE_API  bool UnregisterType(LCliTypeHandle* InHandle);

    ENGINE_API  LCliCommandHandle RegisterCommand(LCliCommand&& InCommand);
    FORCEINLINE LCliCommandHandle RegisterCommandChecked(LCliCommand&& InCommand) { auto Result{this->RegisterCommand(std::move(InCommand))}; check(Result.IsValid()) return Result; }
    FORCEINLINE LCliCommandRaiiHandle RegisterCommandRaii(LCliCommand&& InCommand) { return LCliCommandRaiiHandle{this->RegisterCommand(std::move(InCommand))}; }
    FORCEINLINE LCliCommandRaiiHandle RegisterCommandRaiiChecked(LCliCommand&& InCommand) { return LCliCommandRaiiHandle{this->RegisterCommandChecked(std::move(InCommand))}; }
    ENGINE_API  bool UnregisterCommand(LCliCommandHandle* InHandle);

    ENGINE_API  LCliVariableHandle RegisterVariable(LCliVariable&& InVariable);
    FORCEINLINE LCliVariableHandle RegisterVariableChecked(LCliVariable&& InVariable) { auto Result{this->RegisterVariable(std::move(InVariable))}; check(Result.IsValid()) return Result; }
    FORCEINLINE LCliVariableRaiiHandle RegisterVariableRaii(LCliVariable&& InVariable) { return LCliVariableRaiiHandle{this->RegisterVariable(std::move(InVariable))}; }
    FORCEINLINE LCliVariableRaiiHandle RegisterVariableRaiiChecked(LCliVariable&& InVariable) { return LCliVariableRaiiHandle{this->RegisterVariableChecked(std::move(InVariable))}; }
    ENGINE_API  bool UnregisterVariable(LCliVariableHandle* InHandle);

    FORCEINLINE std::optional<LCliObjectHandle>   GetHandle(const LCliObject& InObject) const;
    FORCEINLINE std::optional<LCliTypeHandle>     GetHandle(const LCliType& InObject) const;
    FORCEINLINE std::optional<LCliCommandHandle>  GetHandle(const LCliCommand& InObject) const;
    FORCEINLINE std::optional<LCliVariableHandle> GetHandle(const LCliVariable& InObject) const;

    FORCEINLINE const TArray<LCliType>&     GetTypes() const { return this->Types; }
    FORCEINLINE const TArray<LCliCommand>&  GetCommands() const { return this->Commands; }
    FORCEINLINE const TArray<LCliVariable>& GetVariables() const { return this->Variables; }

    //#
    //# All pointers that the following methods return are only valid for a very short time.
    //# Never store them. Always store the handle to the object.
    //#

    ENGINE_API        LCliObject* GetObject(const LCliObject& InHandle);
    FORCEINLINE const LCliObject* GetObject(const LCliObject& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObject& InHandle) { LCliObject* Out = this->GetObject(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObject& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObject& InHandle) { LCliObject* Out = this->GetObject(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObject& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); jassert( Out ) return Out; }
    ENGINE_API        LCliObject* GetObject(const LCliObjectHandle& InHandle);
    FORCEINLINE const LCliObject* GetObject(const LCliObjectHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle) { LCliObject* Out = this->GetObject(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle) { LCliObject* Out = this->GetObject(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); jassert( Out ) return Out; }
    ENGINE_API        LCliObject* GetObject(const LCliObjectHandle& InHandle, ECliType::Type* OutType);
    FORCEINLINE const LCliObject* GetObject(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle, OutType); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InHandle, OutType); check( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InHandle, OutType); check( Out ) return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InHandle, OutType); jassert( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InHandle, OutType); jassert( Out ) return Out; }
    ENGINE_API        LCliObject* GetObject(const LString& InName);
    FORCEINLINE const LCliObject* GetObject(const LString& InName) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InName); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LString& InName) { LCliObject* Out = this->GetObject(InName); check( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LString& InName) const { const LCliObject* Out = this->GetObject(InName); check( Out ) return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LString& InName) { LCliObject* Out = this->GetObject(InName); jassert( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LString& InName) const { const LCliObject* Out = this->GetObject(InName); jassert( Out ) return Out; }
    ENGINE_API        LCliObject* GetObject(const LString& InName, ECliType::Type* OutType);
    FORCEINLINE const LCliObject* GetObject(const LString& InName, ECliType::Type* OutType) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InName, OutType); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LString& InName, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InName, OutType); check( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LString& InName, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InName, OutType); check( Out ) return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LString& InName, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InName, OutType); jassert( Out ) return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LString& InName, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InName, OutType); jassert( Out ) return Out; }

    FORCEINLINE       LCliType* GetType(const LCliType& InHandle) { return algo::find_pointer(this->Types, InHandle.Uuid, &LCliType::Uuid); }
    FORCEINLINE const LCliType* GetType(const LCliType& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetType(InHandle); }
    FORCEINLINE       LCliType* GetTypeChecked(const LCliType& InHandle) { LCliType* Out = this->GetType(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LCliType& InHandle) const { const LCliType* Out = this->GetType(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LCliType& InHandle) { LCliType* Out = this->GetType(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LCliType& InHandle) const { const LCliType* Out = this->GetType(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliType* GetType(const LCliTypeHandle& InHandle) { return algo::find_pointer(this->Types, InHandle.Uuid, &LCliType::Uuid); }
    FORCEINLINE const LCliType* GetType(const LCliTypeHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetType(InHandle); }
    FORCEINLINE       LCliType* GetTypeChecked(const LCliTypeHandle& InHandle) { LCliType* Out = this->GetType(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LCliTypeHandle& InHandle) const { const LCliType* Out = this->GetType(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LCliTypeHandle& InHandle) { LCliType* Out = this->GetType(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LCliTypeHandle& InHandle) const { const LCliType* Out = this->GetType(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliType* GetType(const LString& InTypeName) { return algo::find_pointer(this->Types, InTypeName, &LCliType::Identifier); }
    FORCEINLINE const LCliType* GetType(const LString& InTypeName) const { return const_cast<LCommandLineInterface*>(this)->GetType(InTypeName); }
    FORCEINLINE       LCliType* GetTypeChecked(const LString& InTypeName) { LCliType* Out = this->GetType(InTypeName); check( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LString& InTypeName) const { const LCliType* Out = this->GetType(InTypeName); check( Out ) return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LString& InTypeName) { LCliType* Out = this->GetType(InTypeName); jassert( Out ) return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LString& InTypeName) const { const LCliType* Out = this->GetType(InTypeName); jassert( Out ) return Out; }

    FORCEINLINE       LCliCommand* GetCommand(const LCliCommand& InHandle) { return algo::find_pointer(this->Commands, InHandle.Uuid, &LCliCommand::Uuid); }
    FORCEINLINE const LCliCommand* GetCommand(const LCliCommand& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InHandle); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LCliCommand& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LCliCommand& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LCliCommand& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LCliCommand& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliCommand* GetCommand(const LCliCommandHandle& InHandle) { return algo::find_pointer(this->Commands, InHandle.Uuid, &LCliCommand::Uuid); }
    FORCEINLINE const LCliCommand* GetCommand(const LCliCommandHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InHandle); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LCliCommandHandle& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LCliCommandHandle& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LCliCommandHandle& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LCliCommandHandle& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliCommand* GetCommand(const LString& InCommandName) { return algo::find_pointer(this->Commands, InCommandName, &LCliCommand::Identifier); }
    FORCEINLINE const LCliCommand* GetCommand(const LString& InCommandName) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InCommandName); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LString& InCommandName) { LCliCommand* Out = this->GetCommand(InCommandName); check( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LString& InCommandName) const { const LCliCommand* Out = this->GetCommand(InCommandName); check( Out ) return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LString& InCommandName) { LCliCommand* Out = this->GetCommand(InCommandName); jassert( Out ) return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LString& InCommandName) const { const LCliCommand* Out = this->GetCommand(InCommandName); jassert( Out ) return Out; }

    FORCEINLINE       LCliVariable* GetVariable(const LCliVariable& InHandle) { return algo::find_pointer(this->Variables, InHandle.Uuid, &LCliVariable::Uuid); }
    FORCEINLINE const LCliVariable* GetVariable(const LCliVariable& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InHandle); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LCliVariable& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LCliVariable& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LCliVariable& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LCliVariable& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliVariable* GetVariable(const LCliVariableHandle& InHandle) { return algo::find_pointer(this->Variables, InHandle.Uuid, &LCliVariable::Uuid); }
    FORCEINLINE const LCliVariable* GetVariable(const LCliVariableHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InHandle); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LCliVariableHandle& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); check( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LCliVariableHandle& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); check( Out ) return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LCliVariableHandle& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); jassert( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LCliVariableHandle& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); jassert( Out ) return Out; }
    FORCEINLINE       LCliVariable* GetVariable(const LString& InVariableName) { return algo::find_pointer(this->Variables, InVariableName, &LCliVariable::Identifier); }
    FORCEINLINE const LCliVariable* GetVariable(const LString& InVariableName) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InVariableName); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LString& InVariableName) { LCliVariable* Out = this->GetVariable(InVariableName); check( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LString& InVariableName) const { const LCliVariable* Out = this->GetVariable(InVariableName); check( Out ) return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LString& InVariableName) { LCliVariable* Out = this->GetVariable(InVariableName); jassert( Out ) return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LString& InVariableName) const { const LCliVariable* Out = this->GetVariable(InVariableName); jassert( Out ) return Out; }

    template <typename T>
    FORCEINLINE auto Get(const LCliObjectHandle& InHandle);
    template <typename T>
    FORCEINLINE auto Get(const LCliObjectHandle& InHandle) const -> decltype(this->Get<T>(InHandle));
    template <typename T>
    FORCEINLINE auto Get(const LString& InName);
    template <typename T>
    FORCEINLINE auto Get(const LString& InName) const -> decltype(this->Get<T>(InName));

private:

    LCliObjectUuid       UuidCursor { 0 };
    TArray<LCliType>     Types;
    TArray<LCliCommand>  Commands;
    TArray<LCliVariable> Variables;
};

template<typename TField>
FORCEINLINE void LCliObjectHandle::GetValue(TField* Destination) const
{
    auto* Cli{this->GetCommandLineInterface()};
    check(Cli)
    Cli->GetVariableChecked(*this)->GetValue<TField>(Destination);
    return;
}

template<typename TField>
FORCEINLINE void LCliObjectRaiiHandle::GetValue(TField* Destination) const
{
    auto* Cli{this->GetCommandLineInterface()};
    check(Cli)
    Cli->GetVariableChecked(static_cast<LCliObjectHandle>(*this))->GetValue<TField>(Destination);
    return;
}

FORCEINLINE constexpr void LCliObjectRaiiHandle::Reset() noexcept
{
    if (this->IsValid())
    {
        if (auto* Cli{this->GetCommandLineInterface()})
        {
            if (auto Temp{static_cast<LCliObjectHandle>(*this)}; Cli->UnregisterCommand(&Temp))
            {
                this->Release();
            }
            return;
        }
        LOG_ERROR(LogCli, "Cli is no longer valid but [{}] is dangling.", this->Uuid)
    }

    return;
}

FORCEINLINE std::optional<LCliObjectHandle> LCommandLineInterface::GetHandle(const LCliObject& InObject) const
{
    return (InObject.Uuid == LCliObject::NoUuid)
        ? std::optional<LCliObjectHandle>{ }
        : std::optional<LCliObjectHandle>{ LCliObjectHandle{InObject.Uuid} };
}

FORCEINLINE std::optional<LCliTypeHandle> LCommandLineInterface::GetHandle(const LCliType& InObject) const
{
    return (InObject.Uuid == LCliObject::NoUuid)
        ? std::optional<LCliTypeHandle>{ }
        : std::optional<LCliTypeHandle>{ LCliTypeHandle{InObject.Uuid} };
}

FORCEINLINE std::optional<LCliCommandHandle> LCommandLineInterface::GetHandle(const LCliCommand& InObject) const
{
    return (InObject.Uuid == LCliObject::NoUuid)
        ? std::optional<LCliCommandHandle>{ }
        : std::optional<LCliCommandHandle>{ LCliCommandHandle{InObject.Uuid} };
}

FORCEINLINE std::optional<LCliVariableHandle> LCommandLineInterface::GetHandle(const LCliVariable& InObject) const
{
    return (InObject.Uuid == LCliObject::NoUuid)
        ? std::optional<LCliVariableHandle>{ }
        : std::optional<LCliVariableHandle>{ LCliVariableHandle{InObject.Uuid} };
}

template <typename T>
FORCEINLINE auto LCommandLineInterface::Get(const LCliObjectHandle& InHandle)
{
    if constexpr (std::is_same_v<T, LCliType>)
    {
        return this->GetType(InHandle);
    }
    else if constexpr (std::is_same_v<T, LCliCommand>)
    {
        return this->GetCommand(InHandle);
    }
    else if constexpr (std::is_same_v<T, LCliVariable>)
    {
        return this->GetVariable(InHandle);
    }
    else
    {
        static_assert(false, "Unsupported type.");
    }
}

template <typename T>
FORCEINLINE auto LCommandLineInterface::Get(const LCliObjectHandle& InHandle) const -> decltype(this->Get<T>(InHandle))
{
    return const_cast<LCommandLineInterface*>(this)->Get<T>(InHandle);
}

template <typename T>
FORCEINLINE auto LCommandLineInterface::Get(const LString& InName)
{
    if constexpr (std::is_same_v<T, LCliType>)
    {
        return this->GetType(InName);
    }
    else if constexpr (std::is_same_v<T, LCliCommand>)
    {
        return this->GetCommand(InName);
    }
    else if constexpr (std::is_same_v<T, LCliVariable>)
    {
        return this->GetVariable(InName);
    }
    else
    {
        static_assert(false, "Unsupported type.");
    }
}

template <typename T>
FORCEINLINE auto LCommandLineInterface::Get(const LString& InName) const -> decltype(this->Get<T>(InName))
{
    return const_cast<LCommandLineInterface*>(this)->Get<T>(InName);
}

} /* ~Namespace Jafg */
