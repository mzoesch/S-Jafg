// Copyright mzoesch. All rights reserved.

#include "User/JgcInputSubsystem.h"
#include "Components/DebugCameraComponent.h"
#include "User/LocalEgo.h"
#include "User/Input/CoreInputSubsystem.h"
#include "User/Input/UserInputRegistry.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>(this);

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    Jafg::LUserInputRegistry& Registry{this->GetLocalEgo().GetUserInputRegistry()};

    if (Jafg::LUserInputContext* Context{Registry.RegisterContext(Jafg::LUserInputContext{"DebugCamera"})})
    {
        Context->MapAction(&Registry,
            {"Pause", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::Escape).value(), Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                Data.UserInput.PushContexts();
                Data.UserInput.ActivateContext("PauseScreen");
                Data.Viewport.GetSurface().SetInputMode(Jafg::EInputModeBits::ShowMouseCursor);
                return {.bDirty=true};
            });

        Context->MapAction(&Registry,
            {"Moving", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Forward",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::W).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Left",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::A).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Backwards",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::S).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Right",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::D).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Up",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::E).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())

                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Down",
                    {Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::Q).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
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
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"Rotating", Jafg::EInputActionCategory::Axis2D},
            Jafg::LPhysicalKey::FromLogical(Jafg::ENamedPhysicalKey::MouseXY),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
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
                        Data.Controller.GetNameAsString()
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
            Frontend.GetPhysicalKey(Jafg::ENamedPhysicalKey::Escape).value(), Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto InputMode{Data.UserInput.PopContexts()}; InputMode.has_value())
                {
                    Data.Viewport.GetSurface().SetInputMode(*InputMode);
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
