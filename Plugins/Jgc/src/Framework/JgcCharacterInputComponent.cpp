// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcCharacterInputComponent.h"
#include "Engine/World.h"
#include "Framework/MovementComponent.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"

void Jgc::AJgcCharacterInputComponent::OnAttach(Jafg::AActor& InOwner)
{
    Super::OnAttach(InOwner);
    (void)this->ActivateUserInputContext();
}

void Jgc::AJgcCharacterInputComponent::OnNewPersonaController(Jafg::APersonaController* New)
{
    Super::OnNewPersonaController(New);
    (void)this->ActivateUserInputContext();
}

void Jgc::AJgcCharacterInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (auto* Movement{this->GetMovementComponent()})
    {
        Movement->SetMoveInput(this->FrameMove);
        Movement->SetJumpHeld(this->bFrameJumpHeld);
        Movement->SetCrouch(this->bFrameCrouch);
        Movement->SetSprint(this->bFrameSprint);
        Movement->SetJetpackEngaged(this->bJetpack);
        Movement->SetDampenersEngaged(this->bDampeners);
        Movement->SetMagBootsEngaged(this->bMagBoots);
    }

    this->FrameMove = LVec3F{0.0f};
    this->bFrameJumpHeld = false;
    this->bFrameCrouch = false;
    this->bFrameSprint = false;
}

bool Jgc::AJgcCharacterInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            auto& UserInput{Lackey->GetUserInput()};
            UserInput.DeactivateAllContexts();
            bool const bActivated{UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Character"))};
            UserInput.SetConsumeMouse(true);
            return bActivated;
        }
    }

    return false;
}

void Jgc::AJgcCharacterInputComponent::OnMove(Jafg::LInputActionValue const& Value) noexcept
{
    this->FrameMove += Value.GetAxis3DValue();
}

void Jgc::AJgcCharacterInputComponent::OnLook(Jafg::LInputActionValue const& Value) noexcept
{
    if (auto* Movement{this->GetMovementComponent()})
    {
        LVec2F const Delta{Value.GetAxis2DValue() * this->LookSensitivity};
        Movement->AddLookInput(LVec3F{Delta.x, this->bInvertPitch ? -Delta.y : Delta.y, 0.0f});
    }
}

void Jgc::AJgcCharacterInputComponent::OnRoll(Jafg::LInputActionValue const& Value) noexcept
{
    if (auto* Movement{this->GetMovementComponent()})
    {
        f32 const Dt{static_cast<f32>(this->GetWorld().GetDeltaTime())};
        Movement->AddLookInput(LVec3F{0.0f, 0.0f, Value.GetAxis1DValue() * this->RollRate * Dt});
    }
}

void Jgc::AJgcCharacterInputComponent::OnJumpPressed() noexcept
{
    this->bFrameJumpHeld = true;
    if (auto* Movement{this->GetMovementComponent()})
    {
        Movement->Jump();
    }
}

void Jgc::AJgcCharacterInputComponent::ToggleJetpack() noexcept
{
    this->bJetpack = !this->bJetpack;
    LOG_VERBOSE(LogJgcInput, "[{}]: Jetpack {}."
        , this->GetNameAsString()
        , this->bJetpack ? "engaged" : "disengaged"
        )
}

void Jgc::AJgcCharacterInputComponent::ToggleDampeners() noexcept
{
    this->bDampeners = !this->bDampeners;
    LOG_VERBOSE(LogJgcInput, "[{}]: Inertial dampeners {}."
        , this->GetNameAsString()
        , this->bDampeners ? "engaged" : "disengaged"
        )
}

void Jgc::AJgcCharacterInputComponent::ToggleMagBoots() noexcept
{
    this->bMagBoots = !this->bMagBoots;
    LOG_VERBOSE(LogJgcInput, "[{}]: Magnetic boots {}."
        , this->GetNameAsString()
        , this->bMagBoots ? "engaged" : "disengaged"
        )
}

Jafg::AMovementComponent* Jgc::AJgcCharacterInputComponent::GetMovementComponent() noexcept
{
    return this->GetOwningActor().GetComponent<Jafg::AMovementComponent>();
}

Jafg::AMovementComponent const* Jgc::AJgcCharacterInputComponent::GetMovementComponent() const noexcept
{
    return this->GetOwningActor().GetComponent<Jafg::AMovementComponent>();
}
