// Copyright mzoesch. All rights reserved.

#include "User/JgcInputSubsystem.h"
#include "User/Input/CoreInputSubsystem.h"
#include "Components/DebugCameraComponent.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>();

    Jafg::LUserInputRegistry& Registry{this->GetLocalEgo().GetUserInputRegistry()};

    if (Jafg::LUserInputContext* Context{Registry.RegisterContext({Jafg::LUserInputTag::ToTag("DebugCamera"), "Debug Camera"})})
    {
        Context->MapAction(&Registry,
            {Jafg::LUserInputTag::ToTag("Moving"), "Moving", Jafg::EInputActionCategory::Axis3D},
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
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value)
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

                return;
            });

        Context->MapAction(&Registry,
            {Jafg::LUserInputTag::ToTag("Rotating"), "Rotating", Jafg::EInputActionCategory::Axis2D},
            "",
            Jafg::EKeys::MouseXY,
            Jafg::EInputActionTrigger::Triggered,
            {},
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value)
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
            });
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [3D-Vehicle] user input context.")
    }

    return;
}
