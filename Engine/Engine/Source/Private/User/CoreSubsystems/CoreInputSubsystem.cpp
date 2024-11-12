// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/UserInput.h"
#include "User/Input/InputAction.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();

    LUserInputContext PreMoveContext = LUserInputContext("InMyWorld");
    UserInput->RegisterContext(std::move(PreMoveContext), true);

    LUserInputContext* ContextInMyWorld = UserInput->GetCheckedContextByName("InMyWorld");

    LInputAction* CurMapping = nullptr;
    LInputActionMappedKey* CurMappedKey = nullptr;

    const LInputAction ToggleDebugScreen = LInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld->MapAction(ToggleDebugScreen);
    ContextInMyWorld->MapKey(CurMapping, EKeys::F3, EInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue) { this->OnDebugScreenToggle(InValue); }
    );

    const LInputAction EnableMouse = LInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld->MapAction(EnableMouse);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::Escape);
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue)
        {
            check( this )
            check( this->GetLocalEgo() )
            check( this->GetLocalEgo()->GetPrimarySurface() )

            this->GetLocalEgo()->GetPrimarySurface()->SetInputMode(
                !this->GetLocalEgo()->GetPrimarySurface()->IsShowMouseCursor()
            );
        }
    );

    const LInputAction BreakToDebugger = LInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld->MapAction(BreakToDebugger);
    ContextInMyWorld->MapKey(CurMapping, EKeys::P, EInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue) {
            CONDITIONALLY_BREAK()
        }
    );

    const LInputAction SwitchToWireframe = LInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld->MapAction(SwitchToWireframe);
    ContextInMyWorld->MapKey(CurMapping, EKeys::F1, EInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue) {
            GetMutableDefault<JUserPreferences>()->SetPolygonMode(EPolygonMode::Wireframe);
        }
    );

    const LInputAction SwitchToFill = LInputAction(EInputActionCategory::Boolean);
    CurMapping = ContextInMyWorld->MapAction(SwitchToWireframe);
    ContextInMyWorld->MapKey(CurMapping, EKeys::F2, EInputActionTrigger::Triggered,
        [this] (LInputActionValue& InValue) {
            GetMutableDefault<JUserPreferences>()->SetPolygonMode(EPolygonMode::Fill);
        }
    );

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
    LUserInputContext* ContextInMyWorld = UserInput->GetCheckedContextByName("InMyWorld");

    LInputAction* CurMapping = nullptr;
    LInputActionMappedKey* CurMappedKey = nullptr;

    const LInputAction Move = LInputAction(EInputActionCategory::Axis3D);
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
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing,
        [InNew] (LInputActionValue& InValue) { InNew->AddMovementInput(InValue); }
    );

    const LInputAction Look = LInputAction(EInputActionCategory::Axis2D);
    CurMapping = ContextInMyWorld->MapAction(Look);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::MouseXY);
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing,
        [InNew] (LInputActionValue& InValue) { InNew->AddRotationInput(InValue); }
    );

    const LInputAction ChangeVelocity = LInputAction(EInputActionCategory::Axis1D);
    CurMapping = ContextInMyWorld->MapAction(ChangeVelocity);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::MouseWheelAxis);
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing,
        [InNew] (LInputActionValue& InValue) { InNew->ChangeVelocity(InValue); }
    );

    return;
}

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LInputActionValue& InValue)
{
    LOG_INFO(LogTemporal, "Debug screen toggled.")
}
