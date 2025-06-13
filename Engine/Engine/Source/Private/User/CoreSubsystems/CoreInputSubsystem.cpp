// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "Core/CoreNames.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"
#include "User/Input/UserInput.h"
#include "User/Input/InputAction.h"
#include "Engine/Engine.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    check( this->GetLocalEgo() )
    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();

    LUserInputContext* ContextMyWorld     = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorld, "In My World"});
    LUserInputContext* ContextMyWorldFoot = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorldFoot, "In My World Foot"});
    LUserInputContext* ContextInPause     = UserInput->RegisterContext(LUserInputContext{Name_UicInPause, "In Pause"});
    LUserInputContext* ContextInConsole   = UserInput->RegisterContext(LUserInputContext{Name_UicInConsole, "In Console"});
    check( ContextMyWorld     )
    check( ContextMyWorldFoot )
    check( ContextInPause     )
    check( ContextInConsole   )

    // Action: ToggleDebugScreen
    {
        ContextMyWorld->MapAction
        (
            UserInput,
            {Name_UsrInToggleDebugScreen, "Toggle Debug Screen", EInputActionCategory::Boolean},
            "",
            EKeys::F3,
            EInputActionTrigger::Triggered,
            {
            },
            this,
            &JCoreInputSubsystem::OnDebugScreenToggle
        );
    }

    // Action: Toggle pause menu
    {
        const LInputAction* Action = UserInput->RegisterAction({Name_UsrInTogglePauseScreen, "Toggle Pause Screen", EInputActionCategory::Boolean});
        ContextMyWorld->MapAction
        (
            Action,
            "",
            EKeys::Escape,
            EInputActionTrigger::Triggered,
            {
            },
            [this, UserInput](LInputActionValue& InValue) -> void
            {
                UserInput->DeactivateContext(Name_UicInMyWorldFoot);
                UserInput->DeactivateContext(Name_UicInMyWorld);
                UserInput->ActivateContext(Name_UicInPause);
                this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both, ShowMouseCursor);
                (void)this->GetLocalEgo()->GetFrontend()->ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::IntransitiveHitTestInvisible);
                return;
            }
        );
        ContextInPause->MapAction
        (
            Action,
            "",
            EKeys::Escape,
            EInputActionTrigger::Triggered,
            {
            },
            [this, UserInput](LInputActionValue& InValue) -> void
            {
                UserInput->DeactivateContext(Name_UicInPause);
                UserInput->ActivateContext(Name_UicInMyWorldFoot);
                UserInput->ActivateContext(Name_UicInMyWorld);
                this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
                (void)this->GetLocalEgo()->GetFrontend()->ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::Collapsed);
                return;
            }
        );
    }

    // Action: Trap
    {
        ContextMyWorld->MapAction
        (
            UserInput,
            {Name_UsrInTrap, "Trap", EInputActionCategory::Boolean},
            "",
            EKeys::P,
            EInputActionTrigger::Triggered,
            {
            },
            [](LInputActionValue& InValue) -> void
            {
                CONDITIONALLY_BREAK()
            }
        );
    }

    // Action: RhiPolyToWireframe
    {
        ContextMyWorld->MapAction
        (
            UserInput,
            {Name_UsrInRhiPolyToWireframe, "Rhi Poly To Wireframe", EInputActionCategory::Boolean},
            "",
            EKeys::F1,
            EInputActionTrigger::Triggered,
            {
            },
            [](LInputActionValue& InValue) -> void
            {
                GetMutableDefault<JUserPreferences>()->PolygonMode = EPolygonMode::Wireframe;
            }
        );
    }

    // Action: RhiPolyToFill
    {
        ContextMyWorld->MapAction
        (
            UserInput,
            {Name_UsrInRhiPolyToFill, "Rhi Poly To Fill", EInputActionCategory::Boolean},
            "",
            EKeys::F2,
            EInputActionTrigger::Triggered,
            {
            },
            [](LInputActionValue& InValue) -> void
            {
                GetMutableDefault<JUserPreferences>()->PolygonMode = EPolygonMode::Fill;
            }
        );
    }

    // Action: RhiShowNormals
    {
        ContextMyWorld->MapAction
        (
            UserInput,
            {Name_UsrInRhiShowNormals, "Rhi Show Normals", EInputActionCategory::Boolean},
            "",
            EKeys::F4,
            EInputActionTrigger::Triggered,
            {
            },
            [](LInputActionValue& InValue) -> void
            {
                LEngineShader* Shader = GEngine->GetShader(Name_ShaderChunk);
                if (Shader == nullptr)
                {
                    LOG_ERROR(LogUserInput, "Failed to find shader.")
                    return;
                }

                if (Shader->GetCachedConstants().Contains("DISPLAY_NORMALS"))
                {
                    Shader->Recompile({{"DISPLAY_NORMALS"}}, {});
                }
                else
                {
                    Shader->Recompile({}, {{"DISPLAY_NORMALS", "1"}});
                }

                return;
            }
        );
    }

    // Action: ToggleConsoleScreen
    {
        const LInputAction* Action = UserInput->RegisterAction({Name_UsrInToggleConsoleScreen, "Toggle Console Screen", EInputActionCategory::Boolean});
        ContextMyWorld->MapAction
        (
            Action,
            "",
            EKeys::T,
            EInputActionTrigger::Triggered,
            {
            },
            [this] (LInputActionValue& InValue)
            {
                this->GetLocalEgo()->GetFrontend()->GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::Show);
            }
        );
        ContextInConsole->MapAction
        (
            Action,
            "",
            EKeys::Escape,
            EInputActionTrigger::Triggered,
            {
            },
            [this](LInputActionValue& InValue) -> void
            {
                this->GetLocalEgo()->GetFrontend()->GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::TryPreview);
            }
        );
    }

    // Action: MovePawn
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInMovePawn, "Move Pawn", EInputActionCategory::Axis3D},
            {
                LInputTrigger
                {
                    "Forward",
                    EKeys::W,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
                LInputTrigger
                {
                    "Left",
                    EKeys::A,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeySwizzleModifier>(),
                        MakeModifier<LInputActionMappedKeyNegateModifier>(),
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
                LInputTrigger
                {
                    "Backwards",
                    EKeys::S,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeyNegateModifier>(),
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
                LInputTrigger
                {
                    "Right",
                    EKeys::D,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeySwizzleModifier>(),
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
                LInputTrigger
                {
                    "Up",
                    EKeys::E,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeySwizzleXZModifier>(),
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
                LInputTrigger
                {
                    "Down",
                    EKeys::Q,
                    EInputActionTrigger::Ongoing,
                    {
                        MakeModifier<LInputActionMappedKeySwizzleXZModifier>(),
                        MakeModifier<LInputActionMappedKeyNegateModifier>(),
                        MakeModifier<LInputActionMappedKeyDeltaTimeModifier>(),
                    },
                },
            },
            {
            }
        );
    }

    // Action: RotatePawn
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInRotatePawn, "Rotate Pawn", EInputActionCategory::Axis2D},
            "",
            EKeys::MouseXY,
            EInputActionTrigger::Ongoing,
            {
            },
            {
            }
        );
    }

    // Action: PawnVelocity
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInPawnVelocity, "Pawn Velocity", EInputActionCategory::Axis1D},
            "",
            EKeys::MouseWheelAxis,
            EInputActionTrigger::Ongoing,
            {
            },
            {
            }
        );
    }

    // Action: Primary
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInPrimary, "Primary", EInputActionCategory::Boolean},
            "",
            EKeys::LeftMouseButton,
            EInputActionTrigger::Triggered,
            {
            },
            {
            }
        );
    }

    // Action: Secondary
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInSecondary, "Secondary", EInputActionCategory::Boolean},
            "",
            EKeys::RightMouseButton,
            EInputActionTrigger::Triggered,
            {
            },
            {
            }
        );
    }

    return;
}

void Jafg::JCoreInputSubsystem::OnNewPawnPossessed(APawn* InOld, APawn* InNew)
{
    Super::OnNewPawnPossessed(InOld, InNew);

    LUserInput* UserInput = this->GetLocalEgo()->GetUserInput();
    LUserInputContext* ContextInMyWorldFoot = UserInput->GetContextByNameChecked(Name_UicInMyWorldFoot);

    if (InNew)
    {
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInMovePawn)->Callback = LUserInputActionDelegate::CreateFunction(InNew, &APawn::OnOngoingMovementInput);
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInRotatePawn)->Callback = LUserInputActionDelegate::CreateFunction(InNew, &APawn::OnOngoingRotationInput);
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPawnVelocity)->Callback = LUserInputActionDelegate::CreateFunction(InNew, &APawn::OnOngoingVelocityChange);
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPrimary)->Callback = LUserInputActionDelegate::CreateFunction(InNew, &APawn::OnOngoingPrimaryInput);
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInSecondary)->Callback = LUserInputActionDelegate::CreateFunction(InNew, &APawn::OnOngoingSecondaryInput);
    }
    else
    {
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInMovePawn)->Callback.Unbind();
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInRotatePawn)->Callback.Unbind();
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPawnVelocity)->Callback.Unbind();
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPrimary)->Callback.Unbind();
        ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInSecondary)->Callback.Unbind();

        return;
    }

    return;
}

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LInputActionValue& InValue) const
{
    WDebugScreen* Screen = this->GetLocalEgo()->GetFrontend()->GetFirstTopLevelWidgetByClassChecked<WDebugScreen>();

    Screen->SetVisibility(Screen->GetVisibility() == EWidgetVisibility::Visible
        ? EWidgetVisibility::Collapsed
        : EWidgetVisibility::Visible
    );

    return;
}
