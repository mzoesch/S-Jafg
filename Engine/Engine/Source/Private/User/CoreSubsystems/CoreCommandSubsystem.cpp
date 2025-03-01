// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreCommandSubsystem.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "Engine/Cli/CliCommand.h"

DECLARE_INLINE_LOG_CATEGORY(LogCoreCommands, Trace)

void Jafg::JCoreCommandSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LCommandLineInterface* CommandLineInterface = this->GetCommandLineInterface();
    check( CommandLineInterface )

    // Command: say
    {
        LCliCommand Command("Say");
        Command.AddOverload(
            LCommandParams()
            .AddToken(LCliToken_String().MakePack<LCliToken_String>())
            .AddExec(LOnCommandExec::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
            {
                LOG_WARNING(LogTemporal, "{}", InArgs.GetCatRepresentation())
            })
        ));
        this->CommandHandle_Say = CommandLineInterface->RegisterCommand(std::move(Command));
    }

    // Command: CreateNewSurface
    {
        LCliCommand Command("a");
        Command.AddOverload(
            LCommandParams()
            .AddExec(LOnCommandExec::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse)
            {
                LOG_WARNING(LogTemporal, "Creating new surface...")
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

    UNREGISTER_COMMAND(CommandHandle_Say)
    UNREGISTER_COMMAND(CommandHandle_CreateNewSurface)

#undef UNREGISTER_COMMAND

    return;
}
