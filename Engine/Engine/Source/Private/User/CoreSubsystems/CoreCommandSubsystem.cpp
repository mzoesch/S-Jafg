// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreCommandSubsystem.h"
#include "Engine/Engine.h"
#include "Cli/CliCommand.h"
#include "Cli/CliStatics.h"

DECLARE_INLINE_LOG_CATEGORY(LogCoreCommands, Trace)

void Jafg::JCoreCommandSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LCommandLineInterface* CommandLineInterface = this->GetCommandLineInterface();
    check( CommandLineInterface )

    // Command: quit
    {

        this->CommandHandle_Quit = CommandLineInterface->RegisterCommand({"Quit", "Quit to desktop.",
        LCommandParams()
        .Exec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
        {
            LOG_VERBOSE(LogCoreCommands, "Received quit request.")
            GEngine->RequestEngineExit("Invoked by CLI command.");
            OutResponse->Rc = ECommandReturnCode::SuccessNoResponse;
        }))});
        check( this->CommandHandle_Quit.IsValid() )
    }

    // Command: say
    {
        this->CommandHandle_Say = CommandLineInterface->RegisterCommand({"Say", "Say something.",
        LCommandParams()
        .Token(LCliType::Type("String"))
        .Exec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
        {
            LOG_WARNING(LogTemporal, "{}", InArgs.GetCatRepresentation())
            OutResponse->Rc = ECommandReturnCode::SuccessNoResponse;
        }))});
        check( this->CommandHandle_Say.IsValid() )
    }

    // Command: CreateNewSurface
    {
        LCliCommand Command("a");
        Command.AddOverload(
            LCommandParams()
            .Exec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
            {
                LOG_WARNING(LogTemporal, "Creating new surface...")
                OutResponse->Rc = ECommandReturnCode::SuccessNoResponse;
            })

        ));
        this->CommandHandle_CreateNewSurface = CommandLineInterface->RegisterCommand(std::move(Command));
    }

    return;
}

void Jafg::JCoreCommandSubsystem::TearDown()
{
    Super::TearDown();

    LCommandLineInterface* CommandLineInterface = this->GetCommandLineInterface();
    check( CommandLineInterface )

#define UNREGISTER_COMMAND(CommandHandle)                                                  \
    if (CommandLineInterface->UnregisterCommand(&(CommandHandle)) == false)                \
    {                                                                                      \
        LOG_WARNING(LogCoreCommands, "Failed to unregister command [" #CommandHandle "].") \
        (CommandHandle).Reset();                                                           \
    }                                                                                      \
    check( this->CommandHandle.IsValid() == false )                                        \

    UNREGISTER_COMMAND(CommandHandle_Quit)
    UNREGISTER_COMMAND(CommandHandle_Say)
    UNREGISTER_COMMAND(CommandHandle_CreateNewSurface)

#undef UNREGISTER_COMMAND

    return;
}
