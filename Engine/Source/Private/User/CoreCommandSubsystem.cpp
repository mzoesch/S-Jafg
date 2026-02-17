// Copyright mzoesch. All rights reserved.

#include "User/CoreCommandSubsystem.h"
#include "Engine/Engine.h"
#include "Cli/CliCommand.h"
#include "Cli/CliStatics.h"

DECLARE_INLINE_LOG_CATEGORY(LogCoreCommands, Trace)

void Jafg::JCoreCommandSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LCommandLineInterface& CommandLineInterface{this->GetCommandLineInterface()};

    // Command: Quit
    this->CommandHandle_Quit = CommandLineInterface.RegisterCommandChecked({"Quit", "Quit to desktop.",
    LCommandParams()
    .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
    {
        LOG_VERBOSE(LogCoreCommands, "Received quit request.")
        GEngine->RequestEngineExit("Invoked by CLI command.");
        OutResponse.Rc = ECommandReturnCode::SuccessNoResponse;
    })});

    // Command: Say
    this->CommandHandle_Say = CommandLineInterface.RegisterCommandChecked({"Say", "Say something.",
    LCommandParams()
    .Token(LCliType::Type("String"))
    .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
    {
        LOG_WARNING(LogTemporal, "{}", Args.GetCatRepresentation())
        OutResponse.Rc = ECommandReturnCode::SuccessNoResponse;
    })});

    return;
}

void Jafg::JCoreCommandSubsystem::TearDown()
{
    Super::TearDown();

    LCommandLineInterface& CommandLineInterface{this->GetCommandLineInterface()};

#define UNREGISTER_COMMAND(CommandHandle)                                                  \
    if (CommandLineInterface.UnregisterCommand(&(CommandHandle)) == false)                 \
    {                                                                                      \
        LOG_WARNING(LogCoreCommands, "Failed to unregister command [" #CommandHandle "].") \
        (CommandHandle).Reset();                                                           \
    }                                                                                      \
    check(this->CommandHandle.IsValid() == false)                                          \

    UNREGISTER_COMMAND(CommandHandle_Quit)
    UNREGISTER_COMMAND(CommandHandle_Say)

#undef UNREGISTER_COMMAND

    return;
}
