// Copyright mzoesch. All rights reserved.

#include "User/Input/CoreInputSubsystem.h"

#include <Framework/MaterialSubsystem.h>

#include "Core/CoreNames.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/UserInput.h"
#include "User/Input/InputAction.h"
#include "Engine/Engine.h"
#include "User/Frontend/DebugMenu.h"
#include "User/Frontend/ConsoleWindow.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LUserInputRegistry& Registry{this->GetLocalEgo().GetUserInputRegistry()};

    if (LUserInputContext* Context{Registry.RegisterContext({LUserInputTag::ToTag("RhiDebug"), "Rhi Debug"})})
    {
        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("ToggleRhiPolygonMode"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F1, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                if (auto& Prefs{GetMutableSingleton<JUserPreferences>()}; Prefs.PolygonMode == EPolygonMode::Fill)
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to wireframe.")
                    Prefs.PolygonMode = EPolygonMode::Wireframe;
                }
                else
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to fill.")
                    Prefs.PolygonMode = EPolygonMode::Fill;
                }
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("ClearMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F9, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                LOG_VERBOSE(LogUserInput, "Clearing mesh material preference.")
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference.reset();
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetNormalMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F5, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Normal].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Normal");
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetUVMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F6, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.UV].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.UV");
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetTangentMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F7, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Tangent].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Tangent");
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetHandednessMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F8, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&)
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Handedness].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Handedness");
            });
    }

    // if (LUserInputContext* Context{ Registry.RegisterContext(LUserInputContext{Name_UicInOmni, "Omni Context"}) })
    // {
    //     // Action: OpenConsole
    //     Context->MapAction(
    //         &Registry,
    //         {Name_UsrInOpenConsole, "Open Console", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::F9,
    //         EInputActionTrigger::Triggered,
    //         {},
    //         [](LViewport& Viewport, LInputActionValue&)
    //         {
    //             Viewport.GetSurface().SetInputMode(EInputMode::UserInterface | EInputMode::ShowMouseCursor);
    //             Viewport.AddWidget<WConsoleWindow>();
    //         });
    //
    //     for (auto& Surface : this->GetLocalEgo().GetFrontend().GetSurfaces())
    //     {
    //         Surface->GetUserInput().ActivateContext(Context->GetName());
    //     }
    // }
    // else
    // {
    //     LOG_ERROR(LogUserInput, "Failed to register omni input context.")
    // }

    // LUserInputContext* ContextMyWorld     = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorld, "In My World"});
    // LUserInputContext* ContextMyWorldFoot = UserInput->RegisterContext(LUserInputContext{Name_UicInMyWorldFoot, "In My World Foot"});
    // LUserInputContext* ContextInPause     = UserInput->RegisterContext(LUserInputContext{Name_UicInPause, "In Pause"});
    // LUserInputContext* ContextInConsole   = UserInput->RegisterContext(LUserInputContext{Name_UicInConsole, "In Console"});
    // LUserInputContext* ContextDebugMenu   = UserInput->RegisterContext(LUserInputContext{Name_UicInDebugMenu, "In Debug Menu"});
    // check( ContextMyWorld     )
    // check( ContextMyWorldFoot )
    // check( ContextInPause     )
    // check( ContextInConsole   )
    // check( ContextDebugMenu   )
    //
    //
    // // Action: ToggleDebugScreen
    // {
    //     ContextMyWorld->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInToggleDebugScreen, "Toggle Debug Screen", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::F3,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         this,
    //         &JCoreInputSubsystem::OnDebugScreenToggle
    //     );
    // }
    //
    // // Action: ToggleDebugMenu
    // {
    //     const LInputAction* Action { UserInput->RegisterAction({Name_UsrInToggleDebugMenu, "Toggle Debug Menu", EInputActionCategory::Boolean}) };
    //     ContextMyWorld->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::F8,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this, UserInput](LInputActionValue& InValue) -> void
    //         {
    //             if
    //             (
    //                    algo::contains(UserInput->GetReferenceContexts(), Name_UicInDebugMenu, &LUserInputContext::GetName)
    //                 || algo::contains(UserInput->GetActiveContexts(), Name_UicInDebugMenu, &LUserInputContext::GetName)
    //             )
    //             {
    //                 LOG_VERBOSE(LogUserInput, "Aborting request for debug menu activation, already active or in reference.")
    //                 return;
    //             }
    //
    //             UserInput->PushContexts();
    //             UserInput->ActivateContext(Name_UicInDebugMenu);
    //
    //             if
    //             (
    //                 WDebugMenu* Screen{ this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugMenu>() };
    //                 ensure(Screen)
    //             )
    //             {
    //                 Screen->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    //             }
    //
    //             return;
    //         }
    //     );
    //     ContextDebugMenu->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::F8,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this, UserInput](LInputActionValue& InValue) -> void
    //         {
    //             ensureDiscard(UserInput->PopContexts());
    //
    //             if
    //             (
    //                 WDebugMenu* Screen { this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugMenu>() };
    //                 ensure(Screen)
    //             )
    //             {
    //                 Screen->SetVisibility(EWidgetVisibility::Collapsed);
    //             }
    //
    //             return;
    //         }
    //     );
    // }
    //
    // // Action: Toggle pause menu
    // {
    //     const LInputAction* Action = UserInput->RegisterAction({Name_UsrInTogglePauseScreen, "Toggle Pause Screen", EInputActionCategory::Boolean});
    //     ContextMyWorld->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::Escape,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this, UserInput](LInputActionValue& InValue) -> void
    //         {
    //             UserInput->DeactivateContext(Name_UicInMyWorldFoot);
    //             UserInput->DeactivateContext(Name_UicInMyWorld);
    //             UserInput->ActivateContext(Name_UicInPause);
    //             this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::Both, ShowMouseCursor);
    //             (void)this->GetLocalEgo().GetFrontend().ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::IntransitiveHitTestInvisible);
    //             return;
    //         }
    //     );
    //     ContextInPause->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::Escape,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this, UserInput](LInputActionValue& InValue) -> void
    //         {
    //             UserInput->DeactivateContext(Name_UicInPause);
    //             UserInput->ActivateContext(Name_UicInMyWorldFoot);
    //             UserInput->ActivateContext(Name_UicInMyWorld);
    //             this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
    //             (void)this->GetLocalEgo().GetFrontend().ChangeWidgetVisibility<WPauseScreen>(EWidgetVisibility::Collapsed);
    //             return;
    //         }
    //     );
    // }
    //
    // // Action: RhiPolyToWireframe
    // {
    //     ContextMyWorld->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInRhiPolyToWireframe, "Rhi Poly To Wireframe", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::F1,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [](LInputActionValue& InValue) -> void
    //         {
    //             GetMutableDefault<JUserPreferences>()->PolygonMode = EPolygonMode::Wireframe;
    //         }
    //     );
    // }
    //
    // // Action: RhiPolyToFill
    // {
    //     ContextMyWorld->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInRhiPolyToFill, "Rhi Poly To Fill", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::F2,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [](LInputActionValue& InValue) -> void
    //         {
    //             GetMutableDefault<JUserPreferences>()->PolygonMode = EPolygonMode::Fill;
    //         }
    //     );
    // }
    //
    // // Action: RhiShowNormals
    // {
    //     ContextMyWorld->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInRhiShowNormals, "Rhi Show Normals", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::F4,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [](LInputActionValue& InValue) -> void
    //         {
    //             LEngineShader* Shader = GEngine->GetShader(Name_ShaderChunk);
    //             if (Shader == nullptr)
    //             {
    //                 LOG_ERROR(LogUserInput, "Failed to find shader.")
    //                 return;
    //             }
    //
    //             if (algo::contains(Shader->GetCachedConstants(), "DISPLAY_NORMALS", &LShaderCompileTimeConstant::Name))
    //             {
    //                 Shader->Recompile({{"DISPLAY_NORMALS"}}, {});
    //             }
    //             else
    //             {
    //                 Shader->Recompile({}, {{"DISPLAY_NORMALS", "1"}});
    //             }
    //
    //             return;
    //         }
    //     );
    // }
    //
    // // Action: ToggleConsoleScreen
    // {
    //     const LInputAction* Action = UserInput->RegisterAction({Name_UsrInToggleConsoleScreen, "Toggle Console Screen", EInputActionCategory::Boolean});
    //     ContextMyWorld->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::T,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this] (LInputActionValue& InValue)
    //         {
    //             this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->SetConsoleFrontendState(EConsoleScreenState::Show);
    //         }
    //     );
    //     ContextInConsole->MapAction
    //     (
    //         Action,
    //         "",
    //         EKeys::Escape,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         [this](LInputActionValue& InValue) -> void
    //         {
    //             this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClassChecked<WConsoleScreen>()->OnEscape();
    //         }
    //     );
    // }
    //
    // // Action: MovePawn
    // {
    //     ContextMyWorldFoot->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInMovePawn, "Move Pawn", EInputActionCategory::Axis3D},
    //         TArray<LInputTrigger>{}
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Forward",
    //                 EKeys::W,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             })
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Left",
    //                 EKeys::A,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             })
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Backwards",
    //                 EKeys::S,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             })
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Right",
    //                 EKeys::D,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             })
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Up",
    //                 EKeys::E,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleXZModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             })
    //             .reflexive_emplace_back(LInputTrigger
    //             {
    //                 "Down",
    //                 EKeys::Q,
    //                 EInputActionTrigger::Ongoing,
    //                 LInputActionMappedTriggerModifiers{}
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeySwizzleXZModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyNegateModifier>())
    //                     .reflexive_emplace_back(MakeModifier<LInputActionMappedKeyDeltaTimeModifier>())
    //             }),
    //         {
    //         }
    //     );
    // }
    //
    // // Action: RotatePawn
    // {
    //     ContextMyWorldFoot->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInRotatePawn, "Rotate Pawn", EInputActionCategory::Axis2D},
    //         "",
    //         EKeys::MouseXY,
    //         EInputActionTrigger::Ongoing,
    //         {
    //         },
    //         {
    //         }
    //     );
    // }
    //
    // // Action: PawnVelocity
    // {
    //     ContextMyWorldFoot->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInPawnVelocity, "Pawn Velocity", EInputActionCategory::Axis1D},
    //         "",
    //         EKeys::MouseWheelAxis,
    //         EInputActionTrigger::Ongoing,
    //         {
    //         },
    //         {
    //         }
    //     );
    // }
    //
    // // Action: Primary
    // {
    //     ContextMyWorldFoot->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInPrimary, "Primary", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::LeftMouseButton,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         {
    //         }
    //     );
    // }
    //
    // // Action: Secondary
    // {
    //     ContextMyWorldFoot->MapAction
    //     (
    //         UserInput,
    //         {Name_UsrInSecondary, "Secondary", EInputActionCategory::Boolean},
    //         "",
    //         EKeys::RightMouseButton,
    //         EInputActionTrigger::Triggered,
    //         {
    //         },
    //         {
    //         }
    //     );
    // }
    //
    // UserInput->ActivateContext(ContextOmni);

    return;
}

// void Jafg::JCoreInputSubsystem::OnNewPawnPossessed(APawn* InOld, APawn* InNew)
// {
//     Super::OnNewPawnPossessed(InOld, InNew);

    // LUserInput& UserInput{ this->GetLocalEgo().GetUserInput() };
    // LUserInputContext* ContextInMyWorldFoot{ UserInput.GetContextByNameChecked(Name_UicInMyWorldFoot) };
    //
    // if (InNew)
    // {
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInMovePawn)->Callback = LUserInputActionDelegate::Create(InNew, &APawn::OnOngoingMovementInput);
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInRotatePawn)->Callback = LUserInputActionDelegate::Create(InNew, &APawn::OnOngoingRotationInput);
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPawnVelocity)->Callback = LUserInputActionDelegate::Create(InNew, &APawn::OnOngoingVelocityChange);
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPrimary)->Callback = LUserInputActionDelegate::Create(InNew, &APawn::OnOngoingPrimaryInput);
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInSecondary)->Callback = LUserInputActionDelegate::Create(InNew, &APawn::OnOngoingSecondaryInput);
    // }
    // else
    // {
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInMovePawn)->Callback.Reset();
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInRotatePawn)->Callback.Reset();
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPawnVelocity)->Callback.Reset();
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInPrimary)->Callback.Reset();
    //     ContextInMyWorldFoot->FindMappedActionChecked(Name_UsrInSecondary)->Callback.Reset();
    //
    //     return;
    // }

//     return;
// }

void Jafg::JCoreInputSubsystem::OnDebugScreenToggle(LInputActionValue& InValue) const
{
    // WDebugScreen* Screen { this->GetLocalEgo().GetFrontend().GetFirstTopLevelWidgetByClass<WDebugScreen>() };
    //
    // if (Screen)
    // {
    //     Screen->SetVisibility(Screen->GetVisibility() == EWidgetVisibility::Collapsed
    //         ? EWidgetVisibility::TransitiveHitTestInvisible
    //         : EWidgetVisibility::Collapsed
    //     );
    // }

    return;
}
