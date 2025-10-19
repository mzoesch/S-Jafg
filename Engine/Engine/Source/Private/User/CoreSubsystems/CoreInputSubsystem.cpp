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
#include "User/Frontend/Osd/DebugMenu.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInput* UserInput{ &this->GetLocalEgo().GetUserInput() };

    LUserInputContext* ContextMyWorld     = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorld, "In My World"});
    LUserInputContext* ContextMyWorldFoot = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorldFoot, "In My World Foot"});
    LUserInputContext* ContextInPause     = UserInput->RegisterContext(LUserInputContext{Name_UicInPause, "In Pause"});
    LUserInputContext* ContextInConsole   = UserInput->RegisterContext(LUserInputContext{Name_UicInConsole, "In Console"});
    LUserInputContext* ContextDebugMenu   = UserInput->RegisterContext(LUserInputContext{Name_UicInDebugMenu, "In Debug Menu"});
    check( ContextMyWorld     )
    check( ContextMyWorldFoot )
    check( ContextInPause     )
    check( ContextInConsole   )
    check( ContextDebugMenu   )

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

    // Action: ToggleDebugMenu
    {
        const LInputAction* Action { UserInput->RegisterAction({Name_UsrInToggleDebugMenu, "Toggle Debug Menu", EInputActionCategory::Boolean}) };
        ContextMyWorld->MapAction
        (
            Action,
            "",
            EKeys::F8,
            EInputActionTrigger::Triggered,
            {
            },
            [this, UserInput](LInputActionValue& InValue) -> void
            {
                if
                (
                       algo::contains(UserInput->GetReferenceContexts(), Name_UicInDebugMenu, &LUserInputContext::GetName)
                    || algo::contains(UserInput->GetActiveContexts(), Name_UicInDebugMenu, &LUserInputContext::GetName)
                )
                {
                    LOG_VERBOSE(LogUserInput, "Aborting request for debug menu activation, already active or in reference.")
                    return;
                }

                UserInput->PushContexts();
                UserInput->ActivateContext(Name_UicInDebugMenu);

                if
                (
                    WDebugMenu* Screen{ this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugMenu>() };
                    ensure(Screen)
                )
                {
                    Screen->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
                }

                return;
            }
        );
        ContextDebugMenu->MapAction
        (
            Action,
            "",
            EKeys::F8,
            EInputActionTrigger::Triggered,
            {
            },
            [this, UserInput](LInputActionValue& InValue) -> void
            {
                ensureDiscard(UserInput->PopContexts());

                if
                (
                    WDebugMenu* Screen { this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugMenu>() };
                    ensure(Screen)
                )
                {
                    Screen->SetVisibility(EWidgetVisibility::Collapsed);
                }

                return;
            }
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
                this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both, ShowMouseCursor);
                (void)this->GetLocalEgo().GetFrontend().ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::IntransitiveHitTestInvisible);
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
                this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
                (void)this->GetLocalEgo().GetFrontend().ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::Collapsed);
                return;
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

                if (algo::contains(Shader->GetCachedConstants(), "DISPLAY_NORMALS", &LShaderCompileTimeConstant::Name))
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
                this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::Show);
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
                this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->OnEscape();
            }
        );
    }

    // Action: MovePawn
    {
        ContextMyWorldFoot->MapAction
        (
            UserInput,
            {Name_UsrInMovePawn, "Move Pawn", EInputActionCategory::Axis3D},
            TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger
                {
                    "Forward",
                    EKeys::W,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(LInputTrigger
                {
                    "Left",
                    EKeys::A,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(LInputTrigger
                {
                    "Backwards",
                    EKeys::S,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(LInputTrigger
                {
                    "Right",
                    EKeys::D,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(LInputTrigger
                {
                    "Up",
                    EKeys::E,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(LInputTrigger
                {
                    "Down",
                    EKeys::Q,
                    EInputActionTrigger::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
                }),
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

    LUserInput& UserInput{ this->GetLocalEgo().GetUserInput() };
    LUserInputContext* ContextInMyWorldFoot{ UserInput.GetContextByNameChecked(Name_UicInMyWorldFoot) };

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
    WDebugScreen* Screen { this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugScreen>() };

    if (Screen)
    {
        Screen->SetVisibility(Screen->GetVisibility() == EWidgetVisibility::Collapsed
            ? EWidgetVisibility::TransitiveHitTestInvisible
            : EWidgetVisibility::Collapsed
        );
    }

    return;
}
