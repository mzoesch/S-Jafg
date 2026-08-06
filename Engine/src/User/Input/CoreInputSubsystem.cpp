// Copyright mzoesch. All rights reserved.

#include "User/Input/CoreInputSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/InputAction.h"
#include "Engine/Engine.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"
#include "Framework/Editor.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    LUserInputRegistry& Registry{this->GetMutableLocalEgo().GetUserInputRegistry()};

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    if (LUserInputContext* Context{Registry.RegisterContext({LUserInputTag::ToTag("RhiDebug"), "Rhi Debug"})})
    {
        Context->MapAction(&Registry,
            {"ToggleRhiPolygonMode", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F1).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
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
                return {};
            });

        Context->MapAction(&Registry,
            {"ClearMeshMaterialPreference", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F9).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Clearing mesh material preference.")
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference.reset();
                return {};
            });

        Context->MapAction(&Registry,
            {"SetNormalMeshMaterialPreference", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F5).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Normal].")
                check(Detail::GMutableEngine)
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference =
                    Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Normal");
                return {};
            });

        Context->MapAction(&Registry,
            {"SetUVMeshMaterialPreference", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F6).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.UV].")
                check(Detail::GMutableEngine)
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference =
                    Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.UV");
                return {};
            });

        Context->MapAction(&Registry,
            {"SetTangentMaterialPreference", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F7).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Tangent].")
                check(Detail::GMutableEngine)
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference =
                    Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Tangent");
                return {};
            });

        Context->MapAction(&Registry,
            {"SetHandednessMaterialPreference", EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(ELogicalKey::F8).value(), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Handedness].")
                check(Detail::GMutableEngine)
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference =
                    Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Handedness");
                return {};
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [RhiDebug] user input context.")
    }

    if (LUserInputContext* Context{Registry.RegisterContext(LUserInputContext{"EditorCameraCapturer"})})
    {
        Context->MapAction(&Registry,
            {"Mouse Capture", EInputActionCategory::Boolean},
            LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton), EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const& Data, LInputActionValue&) -> LOnUserInputActionResult
            {
                Data.UserInput.ActivateContext("EditorCamera");
                Data.UserInput.SetConsumeMouse(true);
                return {.bDirty=true};
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [EditorCameraCapturer] user input context.")
    }

    if (LUserInputContext* Context{Registry.RegisterContext(LUserInputContext{"EditorCamera"})})
    {
        Context->MapAction(&Registry,
            {"Mouse Uncapture", EInputActionCategory::Boolean},
            LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton), EInputActionTriggerBits::Completed,
            {},
            [](LInputCallback const& Data, LInputActionValue&) -> LOnUserInputActionResult
            {
                Data.UserInput.DeactivateContext("EditorCamera");
                Data.UserInput.SetConsumeMouse(false);
                return {.bDirty=true};
            });

        Context->MapAction(&Registry,
            {"Moving", EInputActionCategory::Axis3D},
            TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger{
                    "Forward",
                    {Frontend.GetPhysicalKey(ELogicalKey::W).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(LInputTrigger{
                    "Left",
                    {Frontend.GetPhysicalKey(ELogicalKey::A).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger{
                    "Backwards",
                    {Frontend.GetPhysicalKey(ELogicalKey::S).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger{
                    "Right",
                    {Frontend.GetPhysicalKey(ELogicalKey::D).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger{
                    "Up",
                    {Frontend.GetPhysicalKey(ELogicalKey::E).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger{
                    "Down",
                    {Frontend.GetPhysicalKey(ELogicalKey::Q).value()},
                    EInputActionTriggerBits::Ongoing,
                    LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    }),
            [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AEditorCameraComponent>()})
                    {
                        Comp->OnMove(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Editor camera action was triggered but pawn [{}] does not possess a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AEditorCameraComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Editor camera action was triggered but controller [{}] does not possess any pawn to move.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"VelocityIncrease", EInputActionCategory::Axis1D},
            LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp),
            EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AEditorCameraComponent>()})
                    {
                        Comp->OnVelocityMultiplierChange(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Editor camera action was triggered but pawn [{}] does not possess a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AEditorCameraComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Editor camera action was triggered but controller [{}] does not possess any pawn to move.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"VelocityDecrease", EInputActionCategory::Axis1D},
            LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown),
            EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AEditorCameraComponent>()})
                    {
                        Comp->OnVelocityMultiplierChange(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Editor camera action was triggered but pawn [{}] does not possess a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AEditorCameraComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Editor camera action was triggered but controller [{}] does not possess any pawn to move.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });


        Context->MapAction(&Registry,
            {"Rotating", EInputActionCategory::Axis2D},
            LPhysicalKey::FromLogical(ELogicalKey::MouseXY),
            EInputActionTriggerBits::Triggered,
            {},
            [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AEditorCameraComponent>()})
                    {
                        Comp->OnRotate(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Editor camera action was triggered but pawn [{}] does not possess a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AEditorCameraComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Editor camera action was triggered but controller [{}] does not possess any pawn to move.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [EditorCamera] user input context.")
    }

    return;
}
