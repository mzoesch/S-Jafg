// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcCharacterInputComponent.h"
#include "Engine/World.h"
#include "Framework/Cursor.h"
#include "Framework/DebugDrawerComponent.h"
#include "Framework/Lackey.h"
#include "Framework/MovementComponent.h"
#include "Framework/PersonaController.h"
#include "Framework/Surface.h"
#include "Framework/TraceUtility.h"
#include "User/LocalEgo.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/InteractableComponent.h"
#include "Jgc/Framework/Vehicle.h"

void Jgc::AJgcCharacterInputComponent::OnAttach(Jafg::AActor& InOwner)
{
    Super::OnAttach(InOwner);
    (void)this->ActivateUserInputContext();
}

void Jgc::AJgcCharacterInputComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* Give the mouse back. Otherwise, the cursor would stay confined for whatever comes next. */
    if (this->bCursorMode && Reason == Jafg::EJxxRecordTearDownReason::Default)
    {
        this->SetCursorMode(false);
    }

    Super::OnGarbage(Reason);
}

void Jgc::AJgcCharacterInputComponent::OnNewPersonaController(Jafg::APersonaController* New)
{
    Super::OnNewPersonaController(New);
    (void)this->ActivateUserInputContext();
}

void Jgc::AJgcCharacterInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    /* Holding is polled, so that a lost release (e.g., focus loss) never leaves the cursor active. */
    if (bool const bWantsCursor{this->bFrameCursorHeld && !this->bInVehicle}; bWantsCursor != this->bCursorMode)
    {
        this->SetCursorMode(bWantsCursor);
    }
    this->UpdateHover();

    if (!this->bInVehicle)
    {
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
    }

    this->ResetFrameInput();
}

bool Jgc::AJgcCharacterInputComponent::ActivateUserInputContext() noexcept
{
    if (this->bInVehicle)
    {
        /* The vehicle owns the input until it is left. */
        return false;
    }

    if (auto* Lackey{this->GetLocalLackey()})
    {
        auto& UserInput{Lackey->GetUserInput()};
        UserInput.DeactivateAllContexts();
        bool const bActivated{UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Character"))};
        UserInput.SetConsumeMouse(!this->bCursorMode);
        UserInput.SetConfineMouse(this->bCursorMode);
        return bActivated;
    }

    return false;
}

bool Jgc::AJgcCharacterInputComponent::OnInteractPressed() noexcept
{
    if (!this->bCursorMode || this->bInVehicle)
    {
        return false;
    }

    if (auto Target{this->TraceInteractable()})
    {
        LOG_VERBOSE(LogJgcInput, "[{}]: Interacting with [{}] of [{}]."
            , this->GetNameAsString()
            , Target->Interactable->GetNameAsString()
            , Target->Interactable->GetOwningActor().GetNameAsString()
            )
        return Target->Interactable->Interact(this->GetOwningPawn());
    }

    return false;
}

void Jgc::AJgcCharacterInputComponent::OnEnteredVehicle(AVehicleComponent& Vehicle) noexcept
{
    LOG_VERBOSE(LogJgcInput, "[{}]: Entered vehicle [{}].", this->GetNameAsString(), Vehicle.GetNameAsString())

    if (this->bCursorMode)
    {
        this->SetCursorMode(false);
    }
    this->bInVehicle = true;
    this->ResetFrameInput();
}

void Jgc::AJgcCharacterInputComponent::OnExitedVehicle() noexcept
{
    LOG_VERBOSE(LogJgcInput, "[{}]: Exited vehicle.", this->GetNameAsString())

    this->bInVehicle = false;
    this->ResetFrameInput();
    (void)this->ActivateUserInputContext();
}

std::optional<Jgc::LInteractionTarget> Jgc::AJgcCharacterInputComponent::TraceInteractable() noexcept
{
    if (!this->bCursorMode)
    {
        return {};
    }

    auto* Lackey{this->GetLocalLackey()};
    if (!Lackey)
    {
        return {};
    }

    LVec2F const Extent{Lackey->GetWorldViewExtent()};
    std::optional<LVec2F> const Cursor{Lackey->GetCursorLocationInWorldView()};
    if (!Cursor || Extent.x < 1.0f || Extent.y < 1.0f || this->InteractionTraceLength <= 0.0f)
    {
        return {};
    }

    auto& Pawn{this->GetOwningPawn()};
    auto const Eye{Pawn.GetEye()};
    auto const Ray{maths::screen_to_world_space(Extent, *Cursor, Eye)};

    TArray<Jafg::LHitResult> Hits;
    Jafg::LCollisionQueryParams Params{.bSingleHit = true};
    Params.IgnoredActors.emplace_back(&Pawn);
    LWorldVec3 const Begin{Eye.translation};
    LWorldVec3 const End{Begin + LWorldVec3{Ray.direction} * static_cast<LWorldReal>(this->InteractionTraceLength)};
    if (!this->GetWorld().LineTraceByChannel(&Hits, Begin, End
        , Jafg::ECollisionChannelBits::Static | Jafg::ECollisionChannelBits::Dynamic | Jafg::ECollisionChannelBits::Pawn
        , Params
        ))
    {
        return {};
    }

    /* The closest hit blocks everything behind it. */
    Jafg::LHitResult const& Hit{Hits.front()};
    auto* Interactable{Hit.Actor.GetComponent<AInteractableComponent>()};
    if (!Interactable)
    {
        return {};
    }
    if (maths::magnitude(Hit.GlobalWorldLocation - Begin) > static_cast<LWorldReal>(Interactable->MaxInteractionDistance))
    {
        return {};
    }
    if (!Interactable->CanInteract(Pawn))
    {
        return {};
    }

    return LInteractionTarget{.Interactable = Interactable, .Location = Hit.GlobalWorldLocation};
}

Jafg::LLocalLackey* Jgc::AJgcCharacterInputComponent::GetLocalLackey() noexcept
{
    if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
    {
        return Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>();
    }
    return nullptr;
}

void Jgc::AJgcCharacterInputComponent::SetCursorMode(bool bEnabled) noexcept
{
    LOG_VERBOSE(LogJgcInput, "[{}]: {} cursor mode.", this->GetNameAsString(), bEnabled ? "Entering" : "Leaving")
    this->bCursorMode = bEnabled;

    if (auto* Lackey{this->GetLocalLackey()})
    {
        auto& UserInput{Lackey->GetUserInput()};
        if (bEnabled)
        {
            /* Confine first, so that the cursor never shows up unconfined. */
            UserInput.SetConfineMouse(true);
            UserInput.SetConsumeMouse(false);

            /* Start where the view looks at. */
            if (LVec2F const Extent{Lackey->GetWorldViewExtent()}; Extent.x > 0.0f && Extent.y > 0.0f)
            {
                (void)Lackey->SetCursorLocationInWorldView(Extent * 0.5f);
            }
        }
        else
        {
            UserInput.SetConsumeMouse(true);
            UserInput.SetConfineMouse(false);
        }
    }

    if (!bEnabled)
    {
        this->SetHoversInteractable(false);
    }
}

void Jgc::AJgcCharacterInputComponent::UpdateHover() noexcept
{
    std::optional<LInteractionTarget> const Target{this->TraceInteractable()};
    this->SetHoversInteractable(Target.has_value());

#if JAFG_WITH_EDITOR
    if (Target)
    {
        if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
        {
            if (auto* Drawer{Ctrl->GetComponent<Jafg::ADebugDrawerComponent>()})
            {
                Jafg::AActor const& Actor{Target->Interactable->GetOwningActor()};
                LWorldAabb3 const Aabb{Actor.GetTransformedActorAabb()};
                Drawer->AddAabb({.Tint = Colors::Yellow, .Value = Aabb});

                LWorldReal const Distance{maths::magnitude(Target->Location - this->GetOwningPawn().GetEye().translation)};
                Drawer->AddText({
                    .Tint = Colors::Yellow,
                    .Height = maths::max(0.1f, static_cast<f32>(Distance) * 0.03f),
                    .Location = Target->Location,
                    .Text = Target->Interactable->GetInteractionText(),
                    });
            }
        }
    }
#endif /* JAFG_WITH_EDITOR */
}

void Jgc::AJgcCharacterInputComponent::SetHoversInteractable(bool bHovers) noexcept
{
    if (this->bHoversInteractable == bHovers)
    {
        return;
    }
    this->bHoversInteractable = bHovers;

    if (auto* Lackey{this->GetLocalLackey()})
    {
        Lackey->Viewport.GetSurface()._SetMouseCursor(bHovers ? Jafg::ECursor::Hand : Jafg::ECursor::Default);
    }
}

void Jgc::AJgcCharacterInputComponent::ResetFrameInput() noexcept
{
    this->FrameMove = LVec3F{0.0f};
    this->bFrameJumpHeld = false;
    this->bFrameCrouch = false;
    this->bFrameSprint = false;
    this->bFrameCursorHeld = false;
}

void Jgc::AJgcCharacterInputComponent::OnMove(Jafg::LInputActionValue const& Value) noexcept
{
    this->FrameMove += Value.GetAxis3DValue();
}

void Jgc::AJgcCharacterInputComponent::OnLook(Jafg::LInputActionValue const& Value) noexcept
{
    if (this->bCursorMode)
    {
        /* The mouse moves the cursor. */
        return;
    }

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
