// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"
#include "Cli/CliType.h"

namespace Jafg
{

class JCxxClass;
class LViewport;
class LCliObject;
class LCliCommand;
class LCliVariable;
struct LCommandExecutionInfo;
struct LCommandExecutionResponse;
struct LCommandArgs;
struct LCommandParams;
namespace ECommandReturnCode { enum Type : u8; }

typedef TFunction<void(LCommandExecutionInfo const& Info, LCommandArgs const& InArgs, LCommandExecutionResponse& OutResponse)> LOnCommandInvocation;

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
ENGINE_API LString LexToString(const ECommandReturnCode::Type& InType);

template<typename T>
struct LCommandArgsTypeRet final
{
    UTILITY_STRUCT(LCommandArgsTypeRet)
    typedef T type;
};
template<typename T>
using LCommandArgsTypeRet_t = typename LCommandArgsTypeRet<T>::type;

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

    FORCEINLINE i32    GetArgCount() const { check( this->IsValid() ) return this->SubArgs.size(); }
    ENGINE_API LString GetCatRepresentation() const;
    ENGINE_API void    GetCatRepresentation(LString* AppendTo) const;

    FORCEINLINE bool IsValid() const { return Name.empty() ? true : this->SubArgs.empty(); }

    FORCEINLINE const LCommandArgs& operator[](const i32 Index) const { return this->SubArgs[Index]; }

    //#
    //# Get the argument of the command to the given LCommandArgsTypeRet<TField>::Type C++ type.
    //# This method is typesafe.
    //#
    template<typename TField>
    FORCEINLINE LCommandArgsTypeRet_t<TField> GetAs() const
    {
        typedef LCommandArgsTypeRet_t<TField> return_type;
        static_assert(std::is_default_constructible_v<return_type>);
        return_type Field{};
        serde::FromString<return_type>(&Field, this->GetCatRepresentation());
        return Field;
    }

    //#
    //# Same as the above #GetAs method but allows for more advanced context checking by allowing any number
    //# of arguments. Some types might resolve to differently depending on which context they are used.
    //#
    template<typename TField, typename... TArgs> requires(sizeof... (TArgs) > 0
        && std::is_invocable_r_v<LCommandArgsTypeRet_t<TField>, typename LCommandArgsTypeRet<TField>::dispatcher, LCommandArgs const&, TArgs...>)
    FORCEINLINE LCommandArgsTypeRet_t<TField> GetAs(TArgs&&... Args) const
    {
        return LCommandArgsTypeRet<TField>::dispatch(*this, std::forward<TArgs>(Args)...);
    }

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

    FORCEINLINE LCommandParams&& Exec(LOnCommandInvocation&& InExec)
    {
        this->OnExec = std::move(InExec);
        return std::move(*this);
    }

    FORCEINLINE LCommandParams&& Token(LCliType&& InToken)
    {
        this->Signature.emplace_back(std::move(InToken));
        return std::move(*this);
    }

    //# Whether the command can be invoked with the given arguments.
    ENGINE_API  bool IsInvocable(const LCommandArgs& Args) const;
    FORCEINLINE void Invoke(LCommandExecutionInfo const& Info, const LCommandArgs& Args, LCommandExecutionResponse& OutResponse) const
    {
        this->OnExec.Invoke(Info, Args, OutResponse);
    }

    ENGINE_API TArray<LString> GetCommonSuggestions(const LCommandArgs& Args, const u32 MaxSuggestions, const bool bParseNotBeginTypedArg) const;

    ENGINE_API LString GetCatRepresentation() const;

    LOnCommandInvocation OnExec;
    TArray<LCliType>     Signature;
};

//#
//# The context information for a command.
//#
struct LCommandExecutionInfo final
{
    //# Optional jxx-class which may act as the invoker.
    JCxxClass* Invoker;

    //# Optional viewport where the command was executed from.
    LViewport* Viewport;
};

//#
//# The response of the command.
//#
struct LCommandExecutionResponse final
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
    //# If this is empty, the stderr will be used for the sanitized output.
    //# This parameter is used to hide sensitive information from the user. E.g. missing operation permissions. This
    //# will not be logged but will most likely serve as a user feedback for all attached listeners.
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

    FORCEINLINE i32  GetOverloadCount() const { return this->Overloads.size(); }
    FORCEINLINE auto GetOverloads()       ->       TArray<LCommandParams>& { return this->Overloads; }
    FORCEINLINE auto GetOverloads() const -> const TArray<LCommandParams>& { return this->Overloads; }
    FORCEINLINE void AddOverload(LCommandParams&& InParams) { this->Overloads.emplace_back(std::move(InParams)); }

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
