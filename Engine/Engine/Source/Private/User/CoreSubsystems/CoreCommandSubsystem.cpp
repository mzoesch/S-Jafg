// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreCommandSubsystem.h"
#include "Engine/Cli/CliCommand.h"

DECLARE_INLINE_LOG_CATEGORY(LogCoreCommands, Trace)

void Jafg::JCoreCommandSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LCommandLineInterface* CommandLineInterface = this->GetCommandLineInterface();
    check( CommandLineInterface )

    // Command: say
    {
        LCliCommand Command("say");
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

    return;
}

void Jafg::JCoreCommandSubsystem::TearDown()
{
    Super::TearDown();

    LCommandLineInterface* CommandLineInterface = this->GetCommandLineInterface();
    check( CommandLineInterface )

    if (CommandLineInterface->UnregisterCommand(&this->CommandHandle_Say) == false)
    {
        LOG_WARNING(LogCoreCommands, "Failed to unregister command [say].")
        this->CommandHandle_Say.Reset();
    }
    check( this->CommandHandle_Say.IsValid() == false )

    return;
}
