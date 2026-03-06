// Copyright mzoesch. All rights reserved.

#include "User/JgcInputSubsystem.h"
#include "Components/DebugCameraComponent.h"
#include "User/LocalEgo.h"
#include "User/Input/CoreInputSubsystem.h"
#include "User/Input/UserInputRegistry.h"
#include "Framework/PersonaController.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>(this);

    Jafg::LUserInputRegistry& Registry{this->GetLocalEgo().GetUserInputRegistry()};

    if (Jafg::LUserInputContext* Context{Registry.RegisterContext(Jafg::LUserInputContext{"DebugCamera"})})
    {
        Context->MapAction(&Registry,
            {"Pause", Jafg::EInputActionCategory::Boolean},
            Jafg::EKeys::Escape, Jafg::EInputActionTrigger::Triggered,
            {},
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                auto& Input{Viewport.GetSurface().GetUserInput()};
                Input.PushContexts(Viewport.GetSurface().GetInputMode());
                Input.ActivateContext("PauseScreen");
                Viewport.GetSurface().SetInputMode(Jafg::EInputModeBits::UserInterface | Jafg::EInputModeBits::InputSubsystem | Jafg::EInputModeBits::ShowMouseCursor);

                return {.bDirty=true};
            });

        Context->MapAction(&Registry,
            {"Moving", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Forward",
                    Jafg::EKeys::W,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Left",
                    Jafg::EKeys::A,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Backwards",
                    Jafg::EKeys::S,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Right",
                    Jafg::EKeys::D,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Up",
                    Jafg::EKeys::E,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())

                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Down",
                    Jafg::EKeys::Q,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                }),
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Ctrl{Viewport.GetSurface().GetOwnedController()})
                {
                    if (auto* Pawn{Ctrl->GetOwnedPawn()})
                    {
                        if (auto* Comp{Pawn->GetComponent<ADebugCameraComponent>()})
                        {
                            Comp->OnMove(Value);
                        }
                        else
                        {
                            LOG_WARNING(LogUserInput,
                                "Debug camera action was triggered but pawn [{}] does not possess a ::Jgc::JDebugCameraComponent.",
                                Pawn->GetNameAsString()
                                )
                        }
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Debug camera action was triggered but controller [{}] does not possess any pawn to move.",
                            Ctrl->GetNameAsString()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Debug camera action was triggered but surface [{}] does not possess any controller to move.",
                        Viewport.GetSurface().GetHumanReadableName()
                        )
                }

                return {};
            });

        Context->MapAction(&Registry,
            {"Rotating", Jafg::EInputActionCategory::Axis2D},
            Jafg::EKeys::MouseXY,
            Jafg::EInputActionTrigger::Triggered,
            {},
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Ctrl{Viewport.GetSurface().GetOwnedController()})
                {
                    if (auto* Pawn{Ctrl->GetOwnedPawn()})
                    {
                        if (auto* Comp{Pawn->GetComponent<ADebugCameraComponent>()})
                        {
                            Comp->OnRotate(Value);
                        }
                        else
                        {
                            LOG_WARNING(LogUserInput,
                                "Debug camera action was triggered but pawn [{}] does not possess a ::Jgc::JDebugCameraComponent.",
                                Pawn->GetNameAsString()
                                )
                        }
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "Debug camera action was triggered but controller [{}] does not possess any pawn to move.",
                            Ctrl->GetNameAsString()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "Debug camera action was triggered but surface [{}] does not possess any controller to move.",
                        Viewport.GetSurface().GetHumanReadableName()
                        )
                }
                return {};
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [DebugCamera] user input context.")
    }

    if (Jafg::LUserInputContext* Context{Registry.RegisterContext(Jafg::LUserInputContext{"PauseScreen"})})
    {
        Context->MapAction(&Registry,
            {"Unpause", Jafg::EInputActionCategory::Boolean},
            Jafg::EKeys::Escape, Jafg::EInputActionTrigger::Triggered,
            {},
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                auto& Input{Viewport.GetSurface().GetUserInput()};
                if (auto InputMode{Input.PopContexts()}; InputMode.has_value())
                {
                    Viewport.GetSurface().SetInputMode(*InputMode);
                }
                else
                {
                    LOG_WARNING(LogUserInput, "Failed to pop any use input contexts.")
                }
                return {.bDirty=true};
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [PauseScreen] user input context.")
    }

    return;
}
