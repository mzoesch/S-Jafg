// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcVehicleInputComponent.h"
#include "Engine/Engine.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Framework/AvatarComponent.h"
#include "Framework/GimbalComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/User/Frontend/JgcStatusHud.h"

void Jgc::AJgcVehicleInputComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* Also when the world goes away. The HUD lives in the viewport, which stays. */
    WStatusHud::Hide(&this->Hud);
    Super::OnGarbage(Reason);
}

void Jgc::AJgcVehicleInputComponent::OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Super::OnPossessionChanged(Previous, New);

    this->FreeLook.Reset();
    WStatusHud::Hide(&this->Hud);
    if (auto* Lackey{New ? New->TryGetOwningLackey<Jafg::ELackey::Local>() : nullptr})
    {
        WStatusHud& NewHud{WStatusHud::Show(*Lackey, &this->Hud)};
        this->HudSpeedRow = NewHud.AddRow("Speed");
        this->HudDampenersRow = NewHud.AddRow("Dampeners");
        this->UpdateHud();
    }
    if (New)
    {
        /* Every drive starts looking ahead. */
        if (auto* Gimbal{this->GetFreeLookGimbal()})
        {
            Gimbal->Recenter(true);
        }
        (void)this->ActivateUserInputContext();
    }
}

void Jgc::AJgcVehicleInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (!this->GetOwningController())
    {
        this->FreeLook.Reset();
        return;
    }
    this->UpdateHud();

    this->FreeLook.Apply(this->GetFreeLookGimbal(), {
        .Sensitivity = this->FreeLookSensitivity,
        .bInvertPitch = this->bInvertFreeLookPitch,
        .PanSensitivity = this->FreeLookPanSensitivity,
        .ZoomStep = this->FreeLookZoomStep,
        .bZoomOnlyWhileHeld = true,
        .RecenterThreshold = this->FreeLookRecenterThreshold,
        });
}

void Jgc::AJgcVehicleInputComponent::OnVehicleLook(Jafg::LInputActionValue const& Value) noexcept
{
    this->FreeLook.OnLook(Value.GetAxis2DValue());
}

void Jgc::AJgcVehicleInputComponent::OnVehicleZoom(Jafg::LInputActionValue const& Value) noexcept
{
    this->FreeLook.OnZoom(Value.GetAxis1DValue());
}

Jafg::AGimbalComponent* Jgc::AJgcVehicleInputComponent::GetFreeLookGimbal() const noexcept
{
    if (auto* Avatar{this->GetOwningAvatar()})
    {
        if (auto* Eye{Avatar->GetEye()})
        {
            return Jafg::AGimbalComponent::FindAbove(*Eye);
        }
    }
    return nullptr;
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

void Jgc::AJgcVehicleInputComponent::ToggleDampeners() noexcept
{
    if (auto* Vehicle{this->GetVehicle()})
    {
        Vehicle->SetDampenersEngaged(!Vehicle->AreDampenersEngaged());
        LOG_VERBOSE(LogJgcInput, "[{}]: Inertial dampeners of the vehicle {}."
            , this->GetNameAsString(), Vehicle->AreDampenersEngaged() ? "engaged" : "disengaged"
            )
        this->UpdateHud();
    }
}

void Jgc::AJgcVehicleInputComponent::UpdateHud() noexcept
{
    auto* Vehicle{this->GetVehicle()};
    if (!this->Hud || this->Hud->_IsGarbage() || !Vehicle)
    {
        return;
    }
    this->Hud->SetRow(this->HudSpeedRow, algo::sprintf("{:.1f} m/s", Vehicle->GetSpeed()), Colors::White);
    this->Hud->SetToggle(this->HudDampenersRow, Vehicle->AreDampenersEngaged());
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
