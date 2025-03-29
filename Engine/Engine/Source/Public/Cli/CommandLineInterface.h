// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
#include "Cli/CliVariable.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliObjectHandle;
typedef LCliObjectHandle LCliTypeHandle;
typedef LCliObjectHandle LCliCommandHandle;
typedef LCliObjectHandle LCliVariableHandle;
namespace ECliType { enum Type : u8; }

struct LCliObjectHandle final
{
    friend LCommandLineInterface;

    FORCEINLINE LCliObjectHandle() = default;
    FORCEINLINE LCliObjectHandle(const LCliObjectUuid InUuid) : Uuid(InUuid) { }
    FORCEINLINE LCliObjectHandle(const LCliObjectHandle& InHandle) : Uuid(InHandle.Uuid) { }
    FORCEINLINE LCliObjectHandle(LCliObjectHandle&& InHandle) noexcept : Uuid(InHandle.Uuid) { InHandle.Uuid = LCliObject::NoUuid; }
    FORCEINLINE LCliObjectHandle& operator=(const LCliObjectHandle& InHandle) { this->Uuid = InHandle.Uuid; return *this; }
    FORCEINLINE LCliObjectHandle& operator=(LCliObjectHandle&& InHandle) noexcept { this->Uuid = InHandle.Uuid; InHandle.Uuid = LCliObject::NoUuid; return *this; }
    FORCEINLINE ~LCliObjectHandle() = default;

    FORCEINLINE bool IsValid() const { return this->Uuid != LCliObject::NoUuid; }
    FORCEINLINE void Reset() { this->Uuid = LCliObject::NoUuid; }

    template <typename TField> FORCEINLINE void GetValue(TField* Destination) const;

private:

    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const;

    LCliObjectUuid Uuid = LCliObject::NoUuid;
};

namespace ECliType
{

enum Type : u8
{
    Ty,
    Command,
    Variable,
};

} /* ~Namespace EObjectType */

//#
//# The command line interface of the engine.
//#
class LCommandLineInterface final
{
public:

    LCommandLineInterface() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCommandLineInterface)
    ~LCommandLineInterface() = default;

    void TearDown();

    ENGINE_API void Invoke(const LString& InCommandLine, LCommandExecutionResponse* OutResponse = nullptr);

    ENGINE_API auto RegisterType(LCliType&& InType) -> LCliTypeHandle;
    ENGINE_API bool UnregisterType(LCliTypeHandle* InHandle);

    ENGINE_API auto RegisterCommand(LCliCommand&& InCommand) -> LCliCommandHandle;
    ENGINE_API bool UnregisterCommand(LCliCommandHandle* InHandle);

    ENGINE_API auto RegisterVariable(LCliVariable&& InVariable) -> LCliVariableHandle;
    ENGINE_API bool UnregisterVariable(LCliVariableHandle* InHandle);

    //#
    //# All pointer that the following methods return are only valid for a very short time.
    //# Never store them. Always store the handle to the object.
    //#

    ENGINE_API        LCliObject* GetObject(const LCliObject& InHandle);
    FORCEINLINE const LCliObject* GetObject(const LCliObject& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObject& InHandle) { LCliObject* Out = this->GetObject(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObject& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObject& InHandle) { LCliObject* Out = this->GetObject(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObject& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); jassert(Out); return Out; }
    ENGINE_API        LCliObject* GetObject(const LCliObjectHandle& InHandle);
    FORCEINLINE const LCliObject* GetObject(const LCliObjectHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle) { LCliObject* Out = this->GetObject(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle) { LCliObject* Out = this->GetObject(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle) const { const LCliObject* Out = this->GetObject(InHandle); jassert(Out); return Out; }
    ENGINE_API        LCliObject* GetObject(const LCliObjectHandle& InHandle, ECliType::Type* OutType);
    FORCEINLINE const LCliObject* GetObject(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InHandle, OutType); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InHandle, OutType); check(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InHandle, OutType); check(Out); return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InHandle, OutType); jassert(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LCliObjectHandle& InHandle, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InHandle, OutType); jassert(Out); return Out; }
    ENGINE_API        LCliObject* GetObject(const LString& InName);
    FORCEINLINE const LCliObject* GetObject(const LString& InName) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InName); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LString& InName) { LCliObject* Out = this->GetObject(InName); check(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LString& InName) const { const LCliObject* Out = this->GetObject(InName); check(Out); return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LString& InName) { LCliObject* Out = this->GetObject(InName); jassert(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LString& InName) const { const LCliObject* Out = this->GetObject(InName); jassert(Out); return Out; }
    ENGINE_API        LCliObject* GetObject(const LString& InName, ECliType::Type* OutType);
    FORCEINLINE const LCliObject* GetObject(const LString& InName, ECliType::Type* OutType) const { return const_cast<LCommandLineInterface*>(this)->GetObject(InName, OutType); }
    FORCEINLINE       LCliObject* GetObjectChecked(const LString& InName, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InName, OutType); check(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectChecked(const LString& InName, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InName, OutType); check(Out); return Out; }
    FORCEINLINE       LCliObject* GetObjectAsserted(const LString& InName, ECliType::Type* OutType) { LCliObject* Out = this->GetObject(InName, OutType); jassert(Out); return Out; }
    FORCEINLINE const LCliObject* GetObjectAsserted(const LString& InName, ECliType::Type* OutType) const { const LCliObject* Out = this->GetObject(InName, OutType); jassert(Out); return Out; }

    FORCEINLINE       LCliType* GetType(const LCliType& InHandle) { return this->Types.FindByPredicate([&InHandle](const LCliType& Type) { return Type.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliType* GetType(const LCliType& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetType(InHandle); }
    FORCEINLINE       LCliType* GetTypeChecked(const LCliType& InHandle) { LCliType* Out = this->GetType(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LCliType& InHandle) const { const LCliType* Out = this->GetType(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LCliType& InHandle) { LCliType* Out = this->GetType(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LCliType& InHandle) const { const LCliType* Out = this->GetType(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliType* GetType(const LCliTypeHandle& InHandle) { return this->Types.FindByPredicate([InHandle](const LCliType& Type) { return Type.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliType* GetType(const LCliTypeHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetType(InHandle); }
    FORCEINLINE       LCliType* GetTypeChecked(const LCliTypeHandle& InHandle) { LCliType* Out = this->GetType(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LCliTypeHandle& InHandle) const { const LCliType* Out = this->GetType(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LCliTypeHandle& InHandle) { LCliType* Out = this->GetType(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LCliTypeHandle& InHandle) const { const LCliType* Out = this->GetType(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliType* GetType(const LString& InTypeName) { return this->Types.FindByPredicate([InTypeName](const LCliType& Type) { return Type.GetIdentifier() == InTypeName; }); }
    FORCEINLINE const LCliType* GetType(const LString& InTypeName) const { return const_cast<LCommandLineInterface*>(this)->GetType(InTypeName); }
    FORCEINLINE       LCliType* GetTypeChecked(const LString& InTypeName) { LCliType* Out = this->GetType(InTypeName); check(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeChecked(const LString& InTypeName) const { const LCliType* Out = this->GetType(InTypeName); check(Out); return Out; }
    FORCEINLINE       LCliType* GetTypeAsserted(const LString& InTypeName) { LCliType* Out = this->GetType(InTypeName); jassert(Out); return Out; }
    FORCEINLINE const LCliType* GetTypeAsserted(const LString& InTypeName) const { const LCliType* Out = this->GetType(InTypeName); jassert(Out); return Out; }

    FORCEINLINE       LCliCommand* GetCommand(const LCliCommand& InHandle) { return this->Commands.FindByPredicate([&InHandle](const LCliCommand& Command) { return Command.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliCommand* GetCommand(const LCliCommand& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InHandle); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LCliCommand& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LCliCommand& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LCliCommand& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LCliCommand& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliCommand* GetCommand(const LCliCommandHandle& InHandle) { return this->Commands.FindByPredicate([InHandle](const LCliCommand& Command) { return Command.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliCommand* GetCommand(const LCliCommandHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InHandle); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LCliCommandHandle& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LCliCommandHandle& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LCliCommandHandle& InHandle) { LCliCommand* Out = this->GetCommand(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LCliCommandHandle& InHandle) const { const LCliCommand* Out = this->GetCommand(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliCommand* GetCommand(const LString& InCommandName) { return this->Commands.FindByPredicate([InCommandName](const LCliCommand& Command) { return Command.GetIdentifier() == InCommandName; }); }
    FORCEINLINE const LCliCommand* GetCommand(const LString& InCommandName) const { return const_cast<LCommandLineInterface*>(this)->GetCommand(InCommandName); }
    FORCEINLINE       LCliCommand* GetCommandChecked(const LString& InCommandName) { LCliCommand* Out = this->GetCommand(InCommandName); check(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandChecked(const LString& InCommandName) const { const LCliCommand* Out = this->GetCommand(InCommandName); check(Out); return Out; }
    FORCEINLINE       LCliCommand* GetCommandAsserted(const LString& InCommandName) { LCliCommand* Out = this->GetCommand(InCommandName); jassert(Out); return Out; }
    FORCEINLINE const LCliCommand* GetCommandAsserted(const LString& InCommandName) const { const LCliCommand* Out = this->GetCommand(InCommandName); jassert(Out); return Out; }

    FORCEINLINE       LCliVariable* GetVariable(const LCliVariable& InHandle) { return this->Variables.FindByPredicate([&InHandle](const LCliVariable& Variable) { return Variable.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliVariable* GetVariable(const LCliVariable& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InHandle); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LCliVariable& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LCliVariable& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LCliVariable& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LCliVariable& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliVariable* GetVariable(const LCliVariableHandle& InHandle) { return this->Variables.FindByPredicate([InHandle](const LCliVariable& Variable) { return Variable.Uuid == InHandle.Uuid; }); }
    FORCEINLINE const LCliVariable* GetVariable(const LCliVariableHandle& InHandle) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InHandle); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LCliVariableHandle& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); check(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LCliVariableHandle& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); check(Out); return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LCliVariableHandle& InHandle) { LCliVariable* Out = this->GetVariable(InHandle); jassert(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LCliVariableHandle& InHandle) const { const LCliVariable* Out = this->GetVariable(InHandle); jassert(Out); return Out; }
    FORCEINLINE       LCliVariable* GetVariable(const LString& InVariableName) { return this->Variables.FindByPredicate([InVariableName](const LCliVariable& Variable) { return Variable.GetIdentifier() == InVariableName; }); }
    FORCEINLINE const LCliVariable* GetVariable(const LString& InVariableName) const { return const_cast<LCommandLineInterface*>(this)->GetVariable(InVariableName); }
    FORCEINLINE       LCliVariable* GetVariableChecked(const LString& InVariableName) { LCliVariable* Out = this->GetVariable(InVariableName); check(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableChecked(const LString& InVariableName) const { const LCliVariable* Out = this->GetVariable(InVariableName); check(Out); return Out; }
    FORCEINLINE       LCliVariable* GetVariableAsserted(const LString& InVariableName) { LCliVariable* Out = this->GetVariable(InVariableName); jassert(Out); return Out; }
    FORCEINLINE const LCliVariable* GetVariableAsserted(const LString& InVariableName) const { const LCliVariable* Out = this->GetVariable(InVariableName); jassert(Out); return Out; }

    template <typename T>
    FORCEINLINE auto Get(const LCliObjectHandle& InHandle)
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
    FORCEINLINE auto Get(const LCliObjectHandle& InHandle) const -> decltype(this->Get<T>(InHandle))
    {
        return const_cast<LCommandLineInterface*>(this)->Get<T>(InHandle);
    }

    template <typename T>
    FORCEINLINE auto Get(const LString& InName)
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
    FORCEINLINE auto Get(const LString& InName) const -> decltype(this->Get<T>(InName))
    {
        return const_cast<LCommandLineInterface*>(this)->Get<T>(InName);
    }

private:

    LCliObjectUuid         UuidCursor = 0;
    TdhArray<LCliType>     Types;
    TdhArray<LCliCommand>  Commands;
    TdhArray<LCliVariable> Variables;
};

template<typename TField>
void LCliObjectHandle::GetValue(TField* Destination) const
{
    this->GetCommandLineInterface()->GetVariable(this->Uuid)->GetValue<TField>(Destination);
}

} /* ~Namespace Jafg */
