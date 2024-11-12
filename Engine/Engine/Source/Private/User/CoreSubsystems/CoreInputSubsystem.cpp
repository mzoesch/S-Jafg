// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "User/Input/UserInputAction.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInputAction* CurMapping = nullptr;
    LInputActionMappedKey* CurMappedKey = nullptr;

    LUserInputContext ContextInMyWorld = LUserInputContext("InMyWorld");

    const LUserInputAction ToggleDebugScreen = LUserInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld.MapAction(ToggleDebugScreen);
    ContextInMyWorld.MapKey(CurMapping, EKeys::F3, EUserInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue) { this->OnDebugScreenToggle(InValue); }
    );

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    UserInput->RegisterContext(std::move(ContextInMyWorld), true);

    return;
}

void Jafg::JCoreInputSubsystem::OnNewPawnPossessed(APawn* InOld, APawn* InNew)
{
    Super::OnNewPawnPossessed(InOld, InNew);

    if (InNew == nullptr)
    {
        return;
    }

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    LUserInputContext* ContextInMyWorld;
    UserInput->GetCheckedContextByName("InMyWorld", ContextInMyWorld);

    LUserInputAction* CurMapping = nullptr;
    LInputActionMappedKey* CurMappedKey = nullptr;

    const LUserInputAction Move = LUserInputAction(EInputActionCategory::Axis3D);
    CurMapping = ContextInMyWorld->MapAction(Move);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::W);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::A);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeySwizzleModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyNegateModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::S);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyNegateModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::D);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeySwizzleModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::E);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeySwizzleXZModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::Q);
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeySwizzleXZModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyNegateModifier>());
    CurMappedKey->Modifiers.Add(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>());

    ContextInMyWorld->MapCallback(CurMapping, EUserInputActionTrigger::Ongoing,
        [InNew] (LInputActionValue& InValue) { InNew->AddMovementInput(InValue); }
    );

    return;
}

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LInputActionValue& InValue)
{
    LOG_INFO(LogTemporal, "Debug screen toggled.")
}
