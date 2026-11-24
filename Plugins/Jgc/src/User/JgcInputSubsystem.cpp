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

    if (auto* Context{Registry.RegisterContext("Jgc.Vehicle", "Vehicle")})
    {
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Strafing"),
            .Category = Jafg::EInputActionCategory::Axis3D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Forward",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::W).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Left",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::A).value()},
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Backward",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::S).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Right",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::D).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Up",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::E).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Down",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Q).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleStrafe(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput
                            , "[{}]: Action was triggered but the possessed actor has no [{}]."
                            , Possessed->GetNameAsString(), algo::type_name<AJgcVehicleInputComponent>()
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
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Torque"),
            .Category = Jafg::EInputActionCategory::Axis3D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Positive Pitch",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadEight).value()},
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Negative Pitch",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadFive).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Positive Yaw",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadSix).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Negative Yaw",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadFour).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Positive Roll",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadNine).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Negative Roll",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::NumPadSeven).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleTorque(Value);
                    }
                    else
                    {
                        LOG_WARNING(LogUserInput
                            , "[{}]: Action was triggered but the possessed actor has no [{}]."
                            , Possessed->GetNameAsString(), algo::type_name<AJgcVehicleInputComponent>()
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
            },});

        /* Hold to look around the vehicle with the mouse. Only tapping it centers the view again. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("FreeLook"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftAlt).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnFreeLookHeld();
                    }
                }
                return {};
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Look"),
            .Category = Jafg::EInputActionCategory::Axis2D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseXY)},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleLook(Value);
                    }
                }
                return {};
            },});

        /* While free looking: Pan the center of the view with the mouse. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("FreeLookPan"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::RightMouseButton)},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnFreeLookPanHeld();
                    }
                }
                return {};
            },});

        /* While free looking: Change the distance to the vehicle. Up is positive, down negative. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Zoom"),
            .Category = Jafg::EInputActionCategory::Axis1D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Closer",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseWheelUp)},
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Farther",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseWheelDown)},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->OnVehicleZoom(Value);
                    }
                }
                return {};
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("ToggleDampeners"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Z).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                if (auto* Possessed{Data.Controller.GetPossessedActor()})
                {
                    if (auto* Comp{Possessed->GetComponent<AJgcVehicleInputComponent>()})
                    {
                        Comp->ToggleDampeners();
                    }
                }
                return {};
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("VehicleExit"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::G).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
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
            },});
    }
    else
    {
        LOG_WARNING(LogJgc, "[Jgc.Vehicle]: Failed to register user input context.")
    }

    if (auto* Context{Registry.RegisterContext("Jgc.Character", "Character")})
    {
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Move"),
            .Category = Jafg::EInputActionCategory::Axis3D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Forward",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::W).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Backward",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::S).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Right",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::D).value()},
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Left",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::A).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnMove(Value); });
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Look"),
            .Category = Jafg::EInputActionCategory::Axis2D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::MouseXY)},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnLook(Value); });
            },});

        /* Only has an effect while weightless. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Roll"),
            .Category = Jafg::EInputActionCategory::Axis1D,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Roll Right",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::E).value()},
                    })
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .DisplayName = "Roll Left",
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Q).value()},
                    .Modifiers = Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeInputModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue& Value) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [&Value](AJgcCharacterInputComponent& Comp) { Comp.OnRoll(Value); });
            },});

        /* Jump when grounded, thrust up when flying or weightless. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Jump"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Space).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnJumpPressed(); });
            },});
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("JumpHeld"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Space).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnJumpHeld(); });
            },});

        /* Crouch when grounded, thrust down when flying or weightless. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Crouch"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftControl).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnCrouchHeld(); });
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Sprint"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::LeftShift).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnSprintHeld(); });
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("ToggleJetpack"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::X).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleJetpack(); });
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("ToggleDampeners"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::Z).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleDampeners(); });
            },});

        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("ToggleMagBoots"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::B).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleMagBoots(); });
            },});

        /* Between the head and the view from behind. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("ToggleView"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::V).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.ToggleView(); });
            },});

        /* Hold to show the cursor instead of looking around. Also on the press, so that nothing else takes it. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Cursor"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered | Jafg::EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Frontend.GetPhysicalKey(Jafg::ELogicalKey::F).value()},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                return ::ForCharacterInput(Data, [](AJgcCharacterInputComponent& Comp) { Comp.OnCursorHeld(); });
            },});

        /* Interact with whatever is under the cursor. Only while the cursor is shown. */
        Context->MapAction({
            .Tag = Jafg::LUserInputTag::ToTag("Interact"),
            .Category = Jafg::EInputActionCategory::Boolean,
            .Triggers = TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger::CreateInfo{
                    .TriggerFlags = Jafg::EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Jafg::LPhysicalKey::FromLogical(Jafg::ELogicalKey::LeftMouseButton)},
                    }),
            .Callback = [](Jafg::LInputCallback const& Data, Jafg::LInputActionValue&) -> Jafg::LOnUserInputActionResult
            {
                bool bInteracted{};
                (void)::ForCharacterInput(Data, [&bInteracted](AJgcCharacterInputComponent& Comp)
                {
                    bInteracted = Comp.OnInteractPressed();
                });
                /* Interactions (e.g., entering a vehicle) may swap the active contexts. */
                return {.bDirty = bInteracted};
            },});
    }
    else
    {
        LOG_WARNING(LogJgc, "[Jgc.Character] Failed to register user input context.")
    }
}
