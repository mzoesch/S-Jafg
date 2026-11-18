// Copyright mzoesch. All rights reserved.

#include "Jgc/User/JgcInputSubsystem.h"
#include "User/LocalEgo.h"
#include "User/Input/CoreInputSubsystem.h"
#include "User/Input/UserInputRegistry.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/JgcVehicleInputComponent.h"
#include "Jgc/Framework/JgcCharacterInputComponent.h"

namespace
{

//# Calls #Functor with the character input component of the actor that the acting controller possesses.
Jafg::LOnUserInputActionResult ForCharacterInput(Jafg::LInputCallback const& Data, auto&& Functor)
{
    if (auto* Possessed{Data.Controller.GetPossessedActor()})
    {
        if (auto* Comp{Possessed->GetComponent<Jgc::AJgcCharacterInputComponent>()})
        {
            Functor(*Comp);
        }
        else
        {
            LOG_WARNING(LogUserInput
                , "[{}]: Action was triggered but the possessed actor has no [{}]."
                , Possessed->GetNameAsString(), algo::type_name<Jgc::AJgcCharacterInputComponent>()
                )
        }
    }
    else
    {
        LOG_WARNING(LogUserInput
            , "[{}]: Action was triggered but controller does not possess anything."
            , Data.Controller.GetNameAsString()
            )
    }
    return {};
}

} /* ~Namespace <Anonymous> */

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
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleStrafe(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "[{}]: Action was triggered but the possessed actor has no [{}].",
                            Possessed->GetNameAsString(), algo::type_name<AJgcVehicleInputComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "[{}]: Action was triggered but controller does not possess anything.",
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
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleTorque(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput,
                            "[{}]: Action was triggered but the possessed actor has no [{}].",
                            Possessed->GetNameAsString(), algo::type_name<AJgcVehicleInputComponent>()
                            )
                    }
                }
                else
                {
                    LOG_WARNING(LogUserInput,
                        "[{}]: Action was triggered but controller does not possess anything.",
                        Data.Controller.GetNameAsString()
                        )
                }
                return {};
            });

        /* Hold to look around the vehicle with the mouse. Only tapping it centers the view again. */
        Context->MapAction(&Registry,
            {"VehicleFreeLook", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftAlt).value(),
            Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnFreeLookHeld();
                    }
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"VehicleLook", Jafg::EInputActionCategory::Axis2D},
            Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseXY),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleLook(Value);
                    }
                }
                return {};
            });

        /* While free looking: Pan the center of the view with the mouse. */
        Context->MapAction(&Registry,
            {"VehicleFreeLookPan", Jafg::EInputActionCategory::Boolean},
            Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::RightMouseButton),
            Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnFreeLookPanHeld();
                    }
                }
                return {};
            });

        /* While free looking: Change the distance to the vehicle. Up is positive, down negative. */
        Context->MapAction(&Registry,
            {"VehicleZoom", Jafg::EInputActionCategory::Axis1D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Closer",
                    {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseWheelUp)},
                    Jafg::EInputActionTriggerBits::Triggered,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Farther",
                    {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseWheelDown)},
                    Jafg::EInputActionTriggerBits::Triggered,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleZoom(Value);
                    }
                }
                return {};
            });

        Context->MapAction(&Registry,
            {"VehicleExit", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::G).value(),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        /* Leaving gives the input back to the character. */
                        return {.bDirty = Comp->OnVehicleExit()};
                    }
                }
                return {};
            });
    }
    else
    {
        LOG_WARNING(LogJgc, "[Jgc.Vehicle]: Failed to register user input context.")
    }

    if (auto* Context{Registry.RegisterContext(Jafg::LUserInputContext{"Jgc.Character"})})
    {
        Context->MapAction(&Registry,
            {"CharacterMove", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Forward",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::W).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
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
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Left",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::A).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnMove(Value); });
            });

        Context->MapAction(&Registry,
            {"CharacterLook", Jafg::EInputActionCategory::Axis2D},
            Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseXY),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnLook(Value); });
            });

        /* Only has an effect while weightless. */
        Context->MapAction(&Registry,
            {"CharacterRoll", Jafg::EInputActionCategory::Axis1D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Roll Right",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::E).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger{
                    "Roll Left",
                    {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Q).value()},
                    Jafg::EInputActionTriggerBits::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnRoll(Value); });
            });

        /* Jump when grounded, thrust up when flying or weightless. */
        Context->MapAction(&Registry,
            {"CharacterJump", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::Space).value(),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnJumpPressed(); });
            });
        Context->MapAction(&Registry,
            {"CharacterJumpHeld", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::Space).value(),
            Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnJumpHeld(); });
            });

        /* Crouch when grounded, thrust down when flying or weightless. */
        Context->MapAction(&Registry,
            {"CharacterCrouch", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftControl).value(),
            Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnCrouchHeld(); });
            });

        Context->MapAction(&Registry,
            {"CharacterSprint", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftShift).value(),
            Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnSprintHeld(); });
            });

        Context->MapAction(&Registry,
            {"CharacterToggleJetpack", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::X).value(),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleJetpack(); });
            });

        Context->MapAction(&Registry,
            {"CharacterToggleDampeners", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::Z).value(),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleDampeners(); });
            });

        Context->MapAction(&Registry,
            {"CharacterToggleMagBoots", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::B).value(),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleMagBoots(); });
            });

        /* Hold to show the cursor instead of looking around. Also on the press, so that nothing else takes it. */
        Context->MapAction(&Registry,
            {"CharacterCursor", Jafg::EInputActionCategory::Boolean},
            Frontend.GetPhysicalKey(Jafg::ELogicalKey::F).value(),
            Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnCursorHeld(); });
            });

        /* Interact with whatever is under the cursor. Only while the cursor is shown. */
        Context->MapAction(&Registry,
            {"CharacterInteract", Jafg::EInputActionCategory::Boolean},
            Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::LeftMouseButton),
            Jafg::EInputActionTriggerBits::Triggered,
            {},
            [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                bool bInteracted{};
                (void)::ForCharacterInput(Data, [&bInteracted](AJgcCharacterInputComponent& Comp)
                {
                    bInteracted = Comp.OnInteractPressed();
                });
                /* Interactions (e.g., entering a vehicle) may swap the active contexts. */
                return {.bDirty = bInteracted};
            });
    }
    else
    {
        LOG_WARNING(LogJgc, "[Jgc.Character] Failed to register user input context.")
    }
}
