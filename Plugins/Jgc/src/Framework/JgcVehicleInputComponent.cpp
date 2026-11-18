// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcVehicleInputComponent.h"
#include "Engine/Engine.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/Vehicle.h"

void Jgc::AJgcVehicleInputComponent::OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Super::OnPossessionChanged(Previous, New);
    if (New)
    {
        (void)this->ActivateUserInputContext();
    }
}

bool Jgc::AJgcVehicleInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Ctrl{this->GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            auto& UserInput{Lackey->GetUserInput()};
            UserInput.DeactivateAllContexts();
            bool const bActivated{UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Vehicle"))};
            UserInput.SetConfineMouse(false);
            UserInput.SetConsumeMouse(true);
            return bActivated;
        }
    }

    return false;
}

Jgc::AVehicleComponent* Jgc::AJgcVehicleInputComponent::GetVehicle() const noexcept
{
    return this->GetOwningActor().GetComponent<AVehicleComponent>();
}

void Jgc::AJgcVehicleInputComponent::OnVehicleStrafe(Jafg::LInputActionValue const& Value)
{
    if (auto* Vehicle{this->GetVehicle()})
    {
        auto VelocityMultiplier{10_r};
        Vehicle->AddThrust(
            Value.GetAxis3DValue() * static_cast<f32>(this->GetWorld().GetDeltaTime()) * VelocityMultiplier * 90'000'000.0f
            , true
            );
    }
}

void Jgc::AJgcVehicleInputComponent::OnVehicleTorque(Jafg::LInputActionValue const& Value)
{
    if (auto* Vehicle{this->GetVehicle()})
    {
        auto TorqueMultiplier{10_r};
        Vehicle->AddTorque(
            Value.GetAxis3DValue() * static_cast<f32>(this->GetWorld().GetDeltaTime()) * TorqueMultiplier * 20'000'000.0f
            , true
            );
    }
}

bool Jgc::AJgcVehicleInputComponent::OnVehicleExit()
{
    if (auto* Vehicle{this->GetVehicle()}; Vehicle && Vehicle->HasDriver())
    {
        Vehicle->Exit();
        return true;
    }
    return false;
}
