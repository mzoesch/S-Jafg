// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"
#include "Cli/CliType.h"

namespace Jafg
{

class LCliObject;
class LCliCommand;
class LCliVariable;
struct LCommandExecutionResponse;
struct LCommandArgs;
struct LCommandParams;
namespace ECommandReturnCode { enum Type : u8; }

MAKE_DELEGATE_SIGNATURE(LOnCommandInvokation, void, const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)

namespace ECommandReturnCode
{

enum Type : u8
{
    Invalid = 0,

    //////////////////////////////////////////////////////////////////////////
    // Somehow Successful Entries
    //////////////////////////////////////////////////////////////////////////

    //# Command executed successfully. The parser might want to decide to give user feedback
    //# with the command's response.
    Success,

    //# Command executed successfully. The parser is advised to not give user feedback.
    SuccessNoResponse,

    //# Command executed successfully. The parser is advised to give user feedback with
    //# the command's response by broadcasting it to all listeners.
    SuccessBroadcast,

    //# Command executed successfully. The parser is advised to give user feedback with
    //# the command's response by broadcasting it to all listeners with the hosting authority as sender.
    SuccessBroadcastWithAuthority,

    //////////////////////////////////////////////////////////////////////////
    // Failure Entries - In rising order of severity
    //////////////////////////////////////////////////////////////////////////

    //
    // It is always safe to just compare with greater or equal to ECommandReturnCodes::Failure to be sure to catch all
    // failure states.
    //

    //# Command executed with an unknown failure.
    Failure,

    //# The command was not found in the subsystem's registry.
    Unknown,

    //# Command cannot be executed due to user rights, or the current application state is blocking the command.
    Forbidden,

    //# Command found but could not match any overload to the provided arguments.
    NoMatchingOverload,

    //# Command executed with a type error.
    TypeError,

    //# Command executed with a semantic error.
    SemanticError,

    //# Command executed with a syntax error.
    SyntaxError,

    //# Platform reports errors or blocks request.
    PlatformError,
};

} /* ~Namespace ECommandReturnCode */
ENGINE_API LSimpleString LexToString(const ECommandReturnCode::Type& InType);

//#
//# The arguments that the command receives.
//#
struct LCommandArgs
{
    FORCEINLINE LCommandArgs() = default;
    FORCEINLINE LCommandArgs(const LString& InName) : Name(InName) { }
    FORCEINLINE LCommandArgs(LString&& InName) : Name(std::move(InName)) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LCommandArgs)
    FORCEINLINE ~LCommandArgs() = default;

    FORCEINLINE i32    GetArgCount() const { check( this->IsValid() ) return this->SubArgs.GetSize(); }
    ENGINE_API LString GetCatRepresentation() const;
    ENGINE_API void    GetCatRepresentation(LString* AppendTo) const;

    FORCEINLINE bool IsValid() const { return Name.IsEmpty() ? true : this->SubArgs.IsEmpty(); }

    FORCEINLINE const LCommandArgs& operator[](const i32 Index) const { return this->SubArgs[Index]; }

    LString Name;
    TArray<LCommandArgs> SubArgs;
};

//#
//# The parameters of the command.
//#
struct LCommandParams
{
    FORCEINLINE LCommandParams() = default;
    PROHIBIT_COPY(LCommandParams)
    DEFAULT_MOVE(LCommandParams)
    FORCEINLINE ~LCommandParams() = default;

    FORCEINLINE LCommandParams&& SetExec(LOnCommandInvokation&& InExec)
    {
        this->OnExec = std::move(InExec);
        return std::move(*this);
    }

    FORCEINLINE LCommandParams&& AddToken(LCliType&& InToken)
    {
        this->Signature.Emplace(std::move(InToken));
        return std::move(*this);
    }

    //# Whether the command can be invoked with the given arguments.
    ENGINE_API  bool IsInvocable(const LCommandArgs& Args) const;
    FORCEINLINE void Invoke(const LCommandArgs& Args, LCommandExecutionResponse* OutResponse) const
    {
        this->OnExec.Invoke(Args, OutResponse);
    }

    LOnCommandInvokation OnExec;
    TArray<LCliType>   Signature;
};

//#
//# The response of the command.
//#
struct LCommandExecutionResponse
{
    //# Return code of the command. @see #ECommandReturnCode for meaning.
    ECommandReturnCode::Type Rc = ECommandReturnCode::Invalid;

    //#
    //# The stdout of the command. If applicable, this will be the user feedback to all listeners of this command.
    //# Do not put sensitive information here.
    //#
    LString StdOut;

    //#
    //# The stderr of the command. This is internal feedback to listeners of the command that have the authority and
    //# permission to see this information - based of the command internal administrator permissions.
    //# Only put that much sensitive information here that should be visible to administrators of the given
    //# command role or higher.
    //# This will be logged in most cases.
    //#
    LString StdErr;

    //#
    //# The sanitized stdout of the command.
    //# If this is empty, the stdout will be used for the sanitized output.
    //# This parameter is used to hide sensitive information from the user. E.g. missing operation permissions. This
    //# will not be logged but will most likely server as a user feedback for all attached listeners.
    //#
    LString SanitizedStdErr;
};

//#
//# A command inside the cli of the engine. Can be executed.
//#
class LCliCommand final : public LCliObject
{
public:

    FORCEINLINE LCliCommand() = delete;
    FORCEINLINE LCliCommand(const LString& InIdentifier) : LCliObject(InIdentifier) { }
    FORCEINLINE LCliCommand(const LString& InIdentifier, const LString& InHelp) : LCliObject(InIdentifier, InHelp) { }
    template <typename... TArgs>
    FORCEINLINE LCliCommand(const LString& InIdentifier, const LString& InHelp, TArgs&&... InArgs) : LCliObject(InIdentifier, InHelp)
    {
        this->AddOverload(std::forward<TArgs>(InArgs)...);
    }

    PROHIBIT_COPY(LCliCommand)
    FORCEINLINE LCliCommand(LCliCommand&& InOther) noexcept
    {
        this->Overloads = std::move(InOther.Overloads);
        this->LCliObject::operator=(std::move(InOther));
        return;
    }
    FORCEINLINE LCliCommand& operator=(LCliCommand&& InOther) noexcept
    {
        this->Overloads = std::move(InOther.Overloads);
        this->LCliObject::operator=(std::move(InOther));
        return *this;
    }

    FORCEINLINE i32  GetOverloadCount() const { return this->Overloads.GetSize(); }
    FORCEINLINE auto GetOverloads()       ->       TArray<LCommandParams>& { return this->Overloads; }
    FORCEINLINE auto GetOverloads() const -> const TArray<LCommandParams>& { return this->Overloads; }
    FORCEINLINE void AddOverload(LCommandParams&& InParams) { this->Overloads.Emplace(std::move(InParams)); }

private:

    template <typename T, typename... TArgs>
    FORCEINLINE void AddOverload(T&& InOverload, TArgs&&... InOverloads)
    {
        static_assert(std::is_constructible_v<LCommandParams, T>, "InOverload must be constructible to LCommandParams");
        this->AddOverload(std::forward<T>(InOverload));
        this->AddOverload(std::forward<TArgs>(InOverloads)...);
        return;
    }

    TArray<LCommandParams> Overloads;
};

} /* ~Namespace Jafg */
