// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

class LCliCommand;
struct LCommandExecutionResponse;
struct LCommandArgs;
struct LCommandParams;
namespace ECommandReturnCode { enum Type : uint8; }

MAKE_DELEGATE_SIGNATURE(LOnCommandExec, void, const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)

namespace ECommandReturnCode
{

enum Type : uint8
{
    Invalid = 0,

    //////////////////////////////////////////////////////////////////////////
    // Somehow Successful Entries
    //////////////////////////////////////////////////////////////////////////

    //# Command executed successfully. The parser might want to decide to give user feedback with the command's response.
    Success,

    //# Command executed successfully. The parser is advised to not give method feedback to the user.
    SuccessNoResponse,

    //# Command executed successfully. The parser is advised to broadcast the command's response.
    SuccessBroadcast,

    //# Command executed successfully. The parser is advised to broadcast the command's response with the authority as sender.
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

    //# Command executed with missing arguments.
    MissingArgs,

    //# Command executed with too many arguments.
    TooManyArgs,

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
    ~LCommandArgs()
    {
        for (LCliToken* Arg : this->Args)
        {
            delete Arg;
        }
        this->Args.Empty();
        return;
    }

    FORCEINLINE int32  GetArgCount() const { return this->Args.GetSize(); }
    ENGINE_API LString GetCatRepresentation(void) const;

    TdhArray<LCliToken*> Args;
};

//#
//# The parameters of the command.
//#
struct LCommandParams
{
    FORCEINLINE LCommandParams() = default;
    PROHIBIT_COPY(LCommandParams)
    FORCEINLINE LCommandParams(LCommandParams&& InOther) noexcept
    {
        this->OnExec = std::move(InOther.OnExec);
        this->Tokens = std::move(InOther.Tokens);
        return;
    }
    FORCEINLINE LCommandParams& operator=(LCommandParams&& InOther) noexcept
    {
        this->OnExec = std::move(InOther.OnExec);
        this->Tokens = std::move(InOther.Tokens);
        return *this;
    }
    FORCEINLINE ~LCommandParams()
    {
        this->OnExec.Unbind();
        for (const LCliToken* Token : this->Tokens)
        {
            delete Token;
        }
        this->Tokens.Empty();
        return;
    }

    FORCEINLINE LCommandParams&& AddExec(LOnCommandExec&& InExec)
    {
        this->OnExec = std::move(InExec);
        return std::move(*this);
    }

    template <typename TToken>
    FORCEINLINE LCommandParams&& AddToken(TToken&& InToken)
    {
        TToken* Token = new TToken(std::forward<TToken>(InToken));
        this->Tokens.Add(Token);
        return std::move(*this);
    }

    //# Whether the command can be invoked with the given arguments.
    ENGINE_API bool IsInvocable(const LCommandArgs& Args) const;
    ENGINE_API void Invoke(const LCommandArgs& Args, LCommandExecutionResponse* OutResponse);

    LOnCommandExec       OnExec;
    TdhArray<LCliToken*> Tokens;
};

//#
//# The response of the command.
//#
struct LCommandExecutionResponse
{
    ECommandReturnCode::Type Rc;

    //#
    //# The stdout of the command. This is the output that the command produced and should contain as much information
    //# as possible about the command's execution. This will be logged in most cases.
    //#
    LString StdOut;

    //#
    //# The sanitized stdout of the command.
    //# If this is empty, the stdout will be used for the sanitized output.
    //# This parameter is used to hide sensitive information from the user. E.g. missing operation permissions. This
    //# will not be logged but will be the feedback to the user on error.
    //#
    LString SanitizedStdErr;
};

//#
//# A command inside the cli of the engine. Can be executed.
//#
class LCliCommand final : public LCliObject
{
public:

    LCliCommand() = delete;
    LCliCommand(const LSimpleString& InIdentifier) : LCliObject(InIdentifier) { }
    LCliCommand(const LSimpleString& InIdentifier, const LString& InHelp) : LCliObject(InIdentifier, InHelp) { }
    PROHIBIT_COPY(LCliCommand)
    FORCEINLINE LCliCommand(LCliCommand&& InOther) noexcept = delete;
    FORCEINLINE LCliCommand& operator=(LCliCommand&& InOther) noexcept
    {
        this->Params = std::move(InOther.Params);
        this->LCliObject::operator=(std::move(InOther));
        return *this;
    }

    ENGINE_API int32 GetOverloadCount() const { return this->Params.GetSize(); }
    FORCEINLINE auto GetOverloads()       ->       TdhArray<LCommandParams>& { return this->Params; }
    FORCEINLINE auto GetOverloads() const -> const TdhArray<LCommandParams>& { return this->Params; }
    FORCEINLINE void AddOverload(LCommandParams&& InParams) { this->Params.Add(std::move(InParams)); }

private:

    TdhArray<LCommandParams> Params;
};

} /* ~Namespace Jafg */
