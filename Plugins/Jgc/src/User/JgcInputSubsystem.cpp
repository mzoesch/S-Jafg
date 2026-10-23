// Copyright mzoesch. All rights reserved.

#include "Jgc/User/JgcInputSubsystem.h"
#include "User/LocalEgo.h"
#include "User/Input/CoreInputSubsystem.h"
#include "User/Input/UserInputRegistry.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/JgcInputComponent.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>(this);

    LOG_VERBOSE(LogJgc, "Initializing jgc core inputs contexts.")

    auto& Frontend{this->GetFrontend()};
    Jafg::LUserInputRegistry& Registry{this->GetMutableLocalEgo().GetUserInputRegistry()};

    if (auto* Context{Registry.RegisterContext(Jafg::LUserInputContext{"Jgc.Vehicle"})})
    {
        Context->MapAction(&Registry,
            {"Strafing", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Forward",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::W).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Left",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::A).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Backward",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::S).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Right",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::D).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Up",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::E).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Down",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Q).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AJgcInputComponent>()})
                    {
                        Comp->OnVehicleStrafe(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "[{}]: Action was triggered but pawn does not posses a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AJgcInputComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "[{}]: Action was triggered but controller does not possess any pawn.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"Torque", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Positive Pitch",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadEight).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Negative Pitch",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadFive).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Positive Yaw",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadSix).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Negative Yaw",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadFour).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Positive Roll",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadNine).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Negative Roll",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadSeven).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Pawn{Data.Controller.GetOwnedPawn()})
                {
                    if (auto* Comp{Pawn->GetComponent<AJgcInputComponent>()})
                    {
                        Comp->OnVehicleTorque(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "[{}]: Action was triggered but pawn does not posses a [{}].",
                            Pawn->GetNameAsString(), algo::type_name<AJgcInputComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "[{}]: Action was triggered but controller does not possess any pawn.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });
    }
}
