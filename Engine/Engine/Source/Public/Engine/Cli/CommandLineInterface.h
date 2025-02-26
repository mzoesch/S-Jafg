// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/Cli/CliCommand.h"
#include "Engine/Cli/CliVariable.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliCommandHandle;
struct LCliVariableHandle;

//#
//# A handle to a command line function.
//#
struct LCliCommandHandle
{
    friend LCommandLineInterface;

    LCliCommandHandle() = default;
    LCliCommandHandle(const LCliObjectUuid InUuid) : Uuid(InUuid) { }
    LCliCommandHandle(const LCliCommandHandle& InHandle) : Uuid(InHandle.Uuid) { }
    LCliCommandHandle(LCliCommandHandle&& InHandle) noexcept : Uuid(InHandle.Uuid) { InHandle.Uuid = LCliObject::NoUuid; }
    LCliCommandHandle& operator=(const LCliCommandHandle& InHandle) { this->Uuid = InHandle.Uuid; return *this; }
    LCliCommandHandle& operator=(LCliCommandHandle&& InHandle) noexcept { this->Uuid = InHandle.Uuid; InHandle.Uuid = LCliObject::NoUuid; return *this; }
    ~LCliCommandHandle() = default;

    FORCEINLINE bool IsValid() const { return this->Uuid != LCliObject::NoUuid; }
    FORCEINLINE void Reset() { this->Uuid = LCliObject::NoUuid; }

private:

    LCliObjectUuid Uuid = LCliObject::NoUuid;
};

//#
//# A handle to a command line variable.
//#
struct LCliVariableHandle
{
    friend LCommandLineInterface;

    LCliVariableHandle() = default;
    LCliVariableHandle(const LCliObjectUuid InUuid) : Uuid(InUuid) { }
    LCliVariableHandle(const LCliVariableHandle& InHandle) : Uuid(InHandle.Uuid) { }
    LCliVariableHandle(LCliVariableHandle&& InHandle) noexcept : Uuid(InHandle.Uuid) { InHandle.Uuid = LCliObject::NoUuid; }
    LCliVariableHandle& operator=(const LCliVariableHandle& InHandle) { this->Uuid = InHandle.Uuid; return *this; }
    LCliVariableHandle& operator=(LCliVariableHandle&& InHandle) noexcept { this->Uuid = InHandle.Uuid; InHandle.Uuid = LCliObject::NoUuid; return *this; }
    ~LCliVariableHandle() = default;

    FORCEINLINE bool IsValid() const { return this->Uuid != LCliObject::NoUuid; }
    FORCEINLINE void Reset() { this->Uuid = LCliObject::NoUuid; }

private:

    LCliObjectUuid Uuid = LCliObject::NoUuid;
};

class LCommandLineInterface
{
public:

    LCommandLineInterface() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCommandLineInterface)
    ~LCommandLineInterface() = default;

    void Initialize();

    ENGINE_API void Invoke(const LString& InCommandLine, LCommandExecutionResponse* OutResponse = nullptr);

    ENGINE_API auto RegisterCommand(LCliCommand&& InCommand) -> LCliCommandHandle;
    ENGINE_API bool UnregisterCommand(LCliCommandHandle* InHandle);

    ENGINE_API auto RegisterVariable(const LCliVariable& InVariable) -> LCliVariableHandle;
    ENGINE_API bool UnregisterVariable(const LCliVariableHandle InHandle);

    /** Pointer is only valid for a very short time. So do not store that. But the handle to it. */
    ENGINE_API auto GetCommand(const LSimpleString& InCommandName)       ->       LCliCommand*;
    ENGINE_API auto GetCommand(const LSimpleString& InCommandName) const -> const LCliCommand*;
    ENGINE_API auto GetCheckedCommand(const LSimpleString& InCommandName)       ->       LCliCommand*;
    ENGINE_API auto GetCheckedCommand(const LSimpleString& InCommandName) const -> const LCliCommand*;
    ENGINE_API auto GetPanickedCommand(const LSimpleString& InCommandName)       ->       LCliCommand*;
    ENGINE_API auto GetPanickedCommand(const LSimpleString& InCommandName) const -> const LCliCommand*;

private:

    LCliObjectUuid UuidCursor = 0;
    TdhArray<LCliCommand> Commands;
};

} /* ~Namespace Jafg */
