// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "Core/CoreNames.h"
#include "Engine/Framework/Hud.h"
#include "Engine/Framework/Pawn.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"
#include "User/Input/UserInput.h"
#include "User/Input/InputAction.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();

    {
        LUserInputContext Context = LUserInputContext(Name_UicInMyWorld);
        UserInput->RegisterContext(std::move(Context));
    }
    {
        LUserInputContext Context = LUserInputContext(Name_UicInPause);
        UserInput->RegisterContext(std::move(Context));
    }
    {
        LUserInputContext Context = LUserInputContext(Name_UicInConsole);
        UserInput->RegisterContext(std::move(Context));
    }

    UserInput->ActivateContext("InMyWorld");

    LUserInputContext* ContextMyWorld   = UserInput->GetCheckedContextByName(Name_UicInMyWorld);
    LUserInputContext* ContextInPause   = UserInput->GetCheckedContextByName(Name_UicInPause);
    LUserInputContext* ContextInConsole = UserInput->GetCheckedContextByName(Name_UicInConsole);

    // Action: Toggle debug screen
    {
        LInputAction* CurMapping = ContextMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        ContextMyWorld->MapKey(CurMapping, EKeys::F3, EInputActionTrigger::Triggered, this, &JCoreInputSubsystem::OnDebugScreenToggle);
    }

    // Action: Toggle pause menu
    {
        LInputAction* CurMapping = UserInput->RegisterAction(LInputAction(EInputActionCategory::Boolean));
        ContextMyWorld->MapAction(CurMapping);
        ContextMyWorld->MapKey
        (
            CurMapping, EKeys::Escape, EInputActionTrigger::Triggered,
            [this, UserInput] (LInputActionValue& InValue)
            {
                UserInput->DeactivateAllContexts();
                UserInput->ActivateContext("InPause");
                this->GetLocalEgo()->GetPrimarySurface()->SetInputMode(EInputMode::Both, ShowMouseCursor);
                (void)this->GetLocalEgo()->GetHud()->ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::IntransitiveHitTestInvisible);
            }
        );
        ContextInPause->MapAction(CurMapping);
        ContextInPause->MapKey
        (
            CurMapping, EKeys::Escape, EInputActionTrigger::Triggered,
            [this, UserInput] (LInputActionValue& InValue)
            {
                UserInput->DeactivateAllContexts();
                UserInput->ActivateContext("InMyWorld");
                this->GetLocalEgo()->GetPrimarySurface()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
                (void)this->GetLocalEgo()->GetHud()->ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::Collapsed);
            }
        );
    }

    // Action: Break to debugger
    {
        LInputAction* CurMapping = ContextMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        ContextMyWorld->MapKey
        (
            CurMapping, EKeys::P, EInputActionTrigger::Triggered,
            [] (LInputActionValue& InValue)
            {
                CONDITIONALLY_BREAK()
            }
        );
    }

    // Action: Switch polygon mode to wireframe
    {
        LInputAction* CurMapping = ContextMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        ContextMyWorld->MapKey
        (
            CurMapping, EKeys::F1, EInputActionTrigger::Triggered,
            [] (LInputActionValue& InValue)
            {
                GetMutableDefault<JUserPreferences>()->SetPolygonMode(EPolygonMode::Wireframe);
            }
        );
    }

    // Action: Switch polygon mode to fill
    {
        LInputAction* CurMapping = ContextMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        ContextMyWorld->MapKey
        (
            CurMapping, EKeys::F2, EInputActionTrigger::Triggered,
            [] (LInputActionValue& InValue)
            {
                GetMutableDefault<JUserPreferences>()->SetPolygonMode(EPolygonMode::Fill);
            }
        );
    }

    // Action: Toggle console
    {
        LInputAction* CurMapping = UserInput->RegisterAction(LInputAction(EInputActionCategory::Boolean));
        ContextMyWorld->MapAction(CurMapping);
        ContextMyWorld->MapKey
        (
            CurMapping, EKeys::T, EInputActionTrigger::Triggered,
            [this] (LInputActionValue& InValue)
            {
                this->GetLocalEgo()->GetHud()->GetTopLevelWidgetByClass<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::Show);
            }
        );
        ContextInConsole->MapAction(CurMapping);
        ContextInConsole->MapKey
        (
            CurMapping, EKeys::Escape, EInputActionTrigger::Triggered,
            [this] (LInputActionValue& InValue)
            {
                this->GetLocalEgo()->GetHud()->GetTopLevelWidgetByClass<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::Hide);
            }
        );
    }

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
    LUserInputContext* ContextInMyWorld = UserInput->GetCheckedContextByName(Name_UicInMyWorld);

    LInputAction* CurMapping = nullptr;
    LInputActionMappedKey* CurMappedKey = nullptr;

    CurMapping = ContextInMyWorld->MapAction(LInputAction(EInputActionCategory::Axis3D), UserInput);
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
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing, InNew, &APawn::OnOngoingMovementInput);

    CurMapping = ContextInMyWorld->MapAction(LInputAction(EInputActionCategory::Axis2D), UserInput);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::MouseXY);
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing, InNew, &APawn::OnOngoingRotationInput);

    CurMapping = ContextInMyWorld->MapAction(LInputAction(EInputActionCategory::Axis1D), UserInput);
    CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::MouseWheelAxis);
    ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Ongoing, InNew, &APawn::OnOngoingVelocityChange);

    {
        CurMapping   = ContextInMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::LeftMouseButton);
        ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Triggered, InNew, &APawn::OnOngoingPrimaryInput);
    }

    {
        CurMapping   = ContextInMyWorld->MapAction(LInputAction(EInputActionCategory::Boolean), UserInput);
        CurMappedKey = ContextInMyWorld->MapKey(CurMapping, EKeys::RightMouseButton);
        ContextInMyWorld->MapCallback(CurMapping, EInputActionTrigger::Triggered, InNew, &APawn::OnOngoingSecondaryInput);
    }

    return;
}

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LInputActionValue& InValue)
{
    WDebugScreen* Screen = this->GetLocalEgo()->GetHud()->GetCheckedTopLevelWidgetByClass<WDebugScreen>();

    Screen->SetVisibility(Screen->GetVisibility() == EWidgetVisibility::Visible
        ? EWidgetVisibility::Collapsed
        : EWidgetVisibility::Visible
    );

    return;
}
