// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "User/Input/UserInputAction.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInputContext ContextInMyWorld = LUserInputContext("InMyWorld");

    LUserInputAction ToggleDebugScreen = LUserInputAction();
    ContextInMyWorld.MapAction(
        ToggleDebugScreen, EKeys::F3, EUserInputActionTrigger::Triggered,
        [this] (LUserInputActionValue& InValue) { this->OnDebugScreenToggle(InValue); }
    );

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->RegisterContext(std::move(ContextInMyWorld));

    return;
}

void Jafg::JCoreInputSubsystem::OnNewPersonaControllerPossessed(APersonaController* InOld, APersonaController* InNew)
{
    Super::OnNewPersonaControllerPossessed(InOld, InNew);



    return;
}

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LUserInputActionValue& InValue)
{
    LOG_INFO(LogTemporal, "Debug screen toggled.")
}
