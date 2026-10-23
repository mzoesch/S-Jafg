// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcInputComponent.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/Vehicle.h"

bool Jgc::AJgcInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            auto& UserInput{Lackey->GetUserInput()};

            UserInput.DeactivateAllContexts();
            UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Vehicle"));
            UserInput.SetConsumeMouse(true);
            return true;
        }
    }

    return false;
}

void Jgc::AJgcInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (this->IsVehicleValid())
    {
        this->ForcePawnToVehicle(*this->Vehicle);
    }
}

Jgc::AVehicleComponent* Jgc::AJgcInputComponent::PossessVehicle(AVehicleComponent* VehicleComponent) noexcept
{
    auto* Result{std::exchange(this->Vehicle, VehicleComponent)};

    if (this->IsVehicleValid())
    {
        this->ForcePawnToVehicle(*this->Vehicle);
    }

    return Result;
}

void Jgc::AJgcInputComponent::OnVehicleStrafe(Jafg::LInputActionValue const& Value)
{
    check(this->IsVehicleValid())
    check(Jafg::IsValidFast(this->GetOuter(), this->Vehicle))
    auto VelocityMultiplier{10_r};
    this->Vehicle->AddThrust(
        Value.GetAxis3DValue() * static_cast<f32>(this->GetWorld().GetDeltaTime()) * VelocityMultiplier * 90'000'000.0f
        , true
        );
}

void Jgc::AJgcInputComponent::OnVehicleTorque(Jafg::LInputActionValue const& Value)
{
    check(this->IsVehicleValid())
    check(Jafg::IsValidFast(this->GetOuter(), this->Vehicle))
    auto TorqueMultiplier{10_r};
    this->Vehicle->AddTorque(
        Value.GetAxis3DValue() * static_cast<f32>(this->GetWorld().GetDeltaTime()) * TorqueMultiplier * 20'000'000.0f
        , true
        );
}

void Jgc::AJgcInputComponent::ForcePawnToVehicle(AVehicleComponent const& VehicleComponent) noexcept
{
    auto& LocalRoot{this->GetOwningActor().GetRootComponent()};
    auto& RemoteRoot{VehicleComponent.GetOwningActor().GetRootComponent()};
    LocalRoot.SetLocalTranslationByTeleport(
        RemoteRoot.GetLocalTranslation()
        - RemoteRoot.GetLocalRotator()*maths::forward_vector<LVec3F> * 30.0f
        );
    LocalRoot.SetLocalRotatorByTeleport(RemoteRoot.GetLocalRotator());
}
