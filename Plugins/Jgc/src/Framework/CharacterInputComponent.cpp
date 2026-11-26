// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/CharacterInputComponent.h"
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
#include "Jgc/User/Frontend/StatusHud.h"
#include "Framework/AvatarComponent.h"
#include "Framework/EyeComponent.h"

void Jgc::ACharacterInputComponent::OnAttach(Jafg::AActor& InOwner)
{
    Super::OnAttach(InOwner);
    (void)this->ActivateUserInputContext();
    /* Already possessed (e.g., emplaced at runtime). */
    if (auto* Lackey{GetLocalLackey(this->GetOwningController())})
    {
        this->ShowHud(Lackey);
    }
}

void Jgc::ACharacterInputComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* Give the mouse back. Otherwise, the cursor would stay confined for whatever comes next. */
    if (this->bCursorMode && Reason == Jafg::EJxxRecordTearDownReason::Default)
    {
        this->SetCursorMode(false, GetLocalLackey(this->GetOwningController()));
    }
    /* Also when the world goes away. The HUD lives in the viewport, which stays. */
    this->ShowHud(nullptr);

    Super::OnGarbage(Reason);
}

void Jgc::ACharacterInputComponent::OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Super::OnPossessionChanged(Previous, New);

    if (New)
    {
        (void)this->ActivateUserInputContext();
        this->ShowHud(GetLocalLackey(New));
        return;
    }

    this->ShowHud(nullptr);

    /* Whatever possesses next (e.g., a vehicle) owns the input now. Leave nothing behind. */
    if (this->bCursorMode)
    {
        this->SetCursorMode(false, GetLocalLackey(Previous));
    }
    this->ResetFrameInput();
    if (auto* Movement{this->GetMovementComponent()})
    {
        Movement->ClearIntent();
    }
}

void Jgc::ACharacterInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (!this->GetOwningController())
    {
        this->ResetFrameInput();
        return;
    }

    /* Holding is polled, so that a lost release (e.g., focus loss) never leaves the cursor active. */
    if (this->bFrameCursorHeld != this->bCursorMode)
    {
        this->SetCursorMode(this->bFrameCursorHeld, GetLocalLackey(this->GetOwningController()));
    }
    this->UpdateHover();

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

    this->ResetFrameInput();
}

bool Jgc::ACharacterInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Lackey{GetLocalLackey(this->GetOwningController())})
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

bool Jgc::ACharacterInputComponent::OnInteractPressed() noexcept
{
    auto* Avatar{this->GetOwningAvatar()};
    if (!this->bCursorMode || !Avatar)
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
        return Target->Interactable->Interact(*Avatar);
    }

    return false;
}

std::optional<Jgc::LInteractionTarget> Jgc::ACharacterInputComponent::TraceInteractable() noexcept
{
    if (!this->bCursorMode)
    {
        return {};
    }

    auto* Ctrl{this->GetOwningController()};
    auto* Avatar{this->GetOwningAvatar()};
    auto* Lackey{GetLocalLackey(Ctrl)};
    std::optional<LWorldEye> const View{Ctrl ? Ctrl->GetView() : std::nullopt};
    if (!Lackey || !Avatar || !View)
    {
        return {};
    }

    LVec2F const Extent{Lackey->GetWorldViewExtent()};
    std::optional<LVec2F> const Cursor{Lackey->GetCursorLocationInWorldView()};
    if (!Cursor || Extent.x < 1.0f || Extent.y < 1.0f || this->InteractionTraceLength <= 0.0f)
    {
        return {};
    }

    /* The cursor is over what the controller sees. */
    LWorldEye const& Eye{*View};
    auto const Ray{maths::screen_to_world_space(Extent, *Cursor, Eye)};

    TArray<Jafg::LHitResult> Hits;
    Jafg::LCollisionQueryParams Params{.bSingleHit = true};
    Params.IgnoredActors.emplace_back(&this->GetOwningActor());
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
    if (!Interactable->CanInteract(*Avatar))
    {
        return {};
    }

    return LInteractionTarget{.Interactable = Interactable, .Location = Hit.GlobalWorldLocation};
}

Jafg::LLocalLackey* Jgc::ACharacterInputComponent::GetLocalLackey(Jafg::APersonaController* Ctrl) noexcept
{
    return Ctrl ? Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>() : nullptr;
}

void Jgc::ACharacterInputComponent::SetCursorMode(bool bEnabled, Jafg::LLocalLackey* Lackey) noexcept
{
    LOG_VERBOSE(LogJgcInput, "[{}]: {} cursor mode.", this->GetNameAsString(), bEnabled ? "Entering" : "Leaving")
    this->bCursorMode = bEnabled;

    if (Lackey)
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
        this->SetHoversInteractable(false, Lackey);
    }
}

void Jgc::ACharacterInputComponent::UpdateHover() noexcept
{
    std::optional<LInteractionTarget> const Target{this->TraceInteractable()};
    this->SetHoversInteractable(Target.has_value(), GetLocalLackey(this->GetOwningController()));

#if JAFG_WITH_EDITOR
    if (Target)
    {
        if (auto* Ctrl{this->GetOwningController()}; Ctrl && Ctrl->GetView())
        {
            if (auto* Drawer{Ctrl->GetComponent<Jafg::ADebugDrawerComponent>()})
            {
                Jafg::AActor const& Actor{Target->Interactable->GetOwningActor()};
                LWorldAabb3 const Aabb{Actor.GetTransformedActorAabb()};
                Drawer->AddAabb({.Tint = Colors::Yellow, .Value = Aabb});

                LWorldReal const Distance{maths::magnitude(Target->Location - Ctrl->GetView()->translation)};
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

void Jgc::ACharacterInputComponent::SetHoversInteractable(bool bHovers, Jafg::LLocalLackey* Lackey) noexcept
{
    if (this->bHoversInteractable == bHovers)
    {
        return;
    }
    this->bHoversInteractable = bHovers;

    if (Lackey)
    {
        Lackey->Viewport.GetSurface()._SetMouseCursor(bHovers ? Jafg::ECursor::Hand : Jafg::ECursor::Default);
    }
}

void Jgc::ACharacterInputComponent::ResetFrameInput() noexcept
{
    this->FrameMove = LVec3F{0.0f};
    this->bFrameJumpHeld = false;
    this->bFrameCrouch = false;
    this->bFrameSprint = false;
    this->bFrameCursorHeld = false;
}

void Jgc::ACharacterInputComponent::OnMove(Jafg::LInputActionValue const& Value) noexcept
{
    this->FrameMove += Value.GetAxis3DValue();
}

void Jgc::ACharacterInputComponent::OnLook(Jafg::LInputActionValue const& Value) noexcept
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

void Jgc::ACharacterInputComponent::OnRoll(Jafg::LInputActionValue const& Value) noexcept
{
    if (auto* Movement{this->GetMovementComponent()})
    {
        f32 const Dt{static_cast<f32>(this->GetWorld().GetDeltaTime())};
        Movement->AddLookInput(LVec3F{0.0f, 0.0f, Value.GetAxis1DValue() * this->RollRate * Dt});
    }
}

void Jgc::ACharacterInputComponent::OnJumpPressed() noexcept
{
    this->bFrameJumpHeld = true;
    if (auto* Movement{this->GetMovementComponent()})
    {
        Movement->Jump();
    }
}

void Jgc::ACharacterInputComponent::ToggleJetpack() noexcept
{
    this->bJetpack = !this->bJetpack;
    this->UpdateHud();
    LOG_VERBOSE(LogJgcInput, "[{}]: Jetpack {}."
        , this->GetNameAsString()
        , this->bJetpack ? "engaged" : "disengaged"
        )
}

void Jgc::ACharacterInputComponent::ToggleDampeners() noexcept
{
    this->bDampeners = !this->bDampeners;
    this->UpdateHud();
    LOG_VERBOSE(LogJgcInput, "[{}]: Inertial dampeners {}."
        , this->GetNameAsString()
        , this->bDampeners ? "engaged" : "disengaged"
        )
}

void Jgc::ACharacterInputComponent::ToggleMagBoots() noexcept
{
    this->bMagBoots = !this->bMagBoots;
    this->UpdateHud();
    LOG_VERBOSE(LogJgcInput, "[{}]: Magnetic boots {}."
        , this->GetNameAsString()
        , this->bMagBoots ? "engaged" : "disengaged"
        )
}

void Jgc::ACharacterInputComponent::ToggleView() noexcept
{
    auto* Avatar{this->GetOwningAvatar()};
    if (!Avatar || !this->GetOwningActor().HasRootComponent())
    {
        return;
    }

    /* The head is the first eye directly below the root (as the movement sees it). */
    Jafg::AEyeComponent* Head{};
    for (auto const& Child: this->GetOwningActor().GetRootComponent().GetChildren())
    {
        if ((Head = Child->As<Jafg::AEyeComponent>()))
        {
            break;
        }
    }
    if (!Head)
    {
        return;
    }

    Jafg::AEyeComponent* Behind{};
    Jafg::AEyeComponent::ForEachEyeIn(*Head, [Head, &Behind](Jafg::AEyeComponent& Eye)
    {
        if (!Behind && &Eye != Head)
        {
            Behind = &Eye;
        }
    });
    if (!Behind)
    {
        LOG_VERBOSE(LogJgcInput, "[{}]: There is no eye below the head to switch to.", this->GetNameAsString())
        return;
    }

    bool const bToBehind{Avatar->GetEye() != Behind};
    Avatar->SetEye(bToBehind ? Behind : Head);
    LOG_VERBOSE(LogJgcInput, "[{}]: View switched to [{}].", this->GetNameAsString(), bToBehind ? "third person" : "first person")
}

Jafg::AMovementComponent* Jgc::ACharacterInputComponent::GetMovementComponent() noexcept
{
    return this->GetOwningActor().GetComponent<Jafg::AMovementComponent>();
}

Jafg::AMovementComponent const* Jgc::ACharacterInputComponent::GetMovementComponent() const noexcept
{
    return this->GetOwningActor().GetComponent<Jafg::AMovementComponent>();
}

void Jgc::ACharacterInputComponent::ShowHud(Jafg::LLocalLackey* Lackey) noexcept
{
    if (!Lackey)
    {
        WStatusHud::Hide(&this->Hud);
        return;
    }

    WStatusHud& NewHud{WStatusHud::Show(*Lackey, &this->Hud)};
    this->HudJetpackRow = NewHud.AddRow("Jetpack");
    this->HudDampenersRow = NewHud.AddRow("Dampeners");
    this->HudMagBootsRow = NewHud.AddRow("Mag boots");
    this->UpdateHud();
}

void Jgc::ACharacterInputComponent::UpdateHud() noexcept
{
    if (this->Hud && !this->Hud->_IsGarbage())
    {
        this->Hud->SetToggle(this->HudJetpackRow, this->bJetpack);
        this->Hud->SetToggle(this->HudDampenersRow, this->bDampeners);
        this->Hud->SetToggle(this->HudMagBootsRow, this->bMagBoots);
    }
}
