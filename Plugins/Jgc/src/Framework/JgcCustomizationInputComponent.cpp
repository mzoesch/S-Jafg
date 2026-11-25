// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcCustomizationInputComponent.h"
#include "Framework/AvatarComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/GimbalComponent.h"
#include "Framework/Lackey.h"
#include "Framework/PersonaController.h"
#include "Framework/Surface.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/CustomizationStation.h"
#include "Jgc/User/Frontend/JgcCustomizationWindow.h"

void Jgc::AJgcCustomizationInputComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* Also when the world goes away. The window lives in the viewport, which stays. */
    this->CloseWindow();
    Super::OnGarbage(Reason);
}

void Jgc::AJgcCustomizationInputComponent::OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Super::OnPossessionChanged(Previous, New);

    this->Orbit.Reset();
    this->LastCursorLocation.reset();
    this->CloseWindow();

    if (New)
    {
        /* Every visit starts in front of the occupant. */
        if (auto* Gimbal{this->GetOrbitGimbal()})
        {
            Gimbal->Recenter(true);
        }
        this->AimEye();
        (void)this->ActivateUserInputContext();
        if (auto* Lackey{New->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            this->OpenWindow(*Lackey);
        }
        return;
    }

    /* Taken away while customizing (e.g., something else was possessed). Nothing was applied. */
    if (auto* Station{this->GetStation()}; Station && Station->HasOccupant())
    {
        Station->Leave(false);
    }
}

void Jgc::AJgcCustomizationInputComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (!this->GetOwningController())
    {
        this->Orbit.Reset();
        this->LastCursorLocation.reset();
        return;
    }

    /* E.g., the occupant was destroyed. There is nothing left to change. */
    if (auto* Station{this->GetStation()}; !Station || !Station->HasOccupant())
    {
        this->CloseWindow();
    }
    /* The view may have been resized. */
    this->AimEye();

    /*
     * The surface only reports the motion of the mouse while the cursor is hidden (#Jafg::LSurfaceBase::Poll). Here
     * it is always shown, so the cursor is dragged instead. Up is positive, as for the motion of a hidden cursor.
     */
    std::optional<LVec2F> const Cursor{this->GetCursorLocation()};
    if (Cursor && this->LastCursorLocation)
    {
        this->Orbit.OnLook(LVec2F{Cursor->x - this->LastCursorLocation->x, this->LastCursorLocation->y - Cursor->y});
    }
    this->LastCursorLocation = Cursor;

    this->Orbit.Apply(this->GetOrbitGimbal(), {
        .Sensitivity = this->OrbitSensitivity,
        /* The eye looks back at the pivot. Turning the gimbal up turns its view down. */
        .bInvertPitch = !this->bInvertOrbitPitch,
        .PanSensitivity = 0.0f,
        .ZoomStep = this->OrbitZoomStep,
        .bZoomOnlyWhileHeld = false,
        .RecenterThreshold = 0.0f,
        });
}

bool Jgc::AJgcCustomizationInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Ctrl{this->GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            auto& UserInput{Lackey->GetUserInput()};
            UserInput.DeactivateAllContexts();
            bool const bActivated{UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Customization"))};
            /* The cursor is always shown, for the window. */
            UserInput.SetConsumeMouse(false);
            UserInput.SetConfineMouse(false);
            return bActivated;
        }
    }

    return false;
}

Jgc::ACustomizationStationComponent* Jgc::AJgcCustomizationInputComponent::GetStation() const noexcept
{
    return this->GetOwningActor().GetComponent<ACustomizationStationComponent>();
}

Jafg::AGimbalComponent* Jgc::AJgcCustomizationInputComponent::GetOrbitGimbal() const noexcept
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

void Jgc::AJgcCustomizationInputComponent::OnOrbitHeld() noexcept
{
    /* Pressing on the window is not meant for the view. Once orbiting, the window does not stop it. */
    if (this->Orbit.IsActive() || !this->IsCursorOverWindow())
    {
        this->Orbit.OnHeld();
    }
}

void Jgc::AJgcCustomizationInputComponent::OnOrbitZoom(Jafg::LInputActionValue const& Value) noexcept
{
    if (!this->IsCursorOverWindow())
    {
        this->Orbit.OnZoom(Value.GetAxis1DValue());
    }
}

void Jgc::AJgcCustomizationInputComponent::Apply()
{
    if (auto* Station{this->GetStation()})
    {
        Station->Leave(true);
    }
}

void Jgc::AJgcCustomizationInputComponent::Discard()
{
    if (auto* Station{this->GetStation()})
    {
        Station->Leave(false);
    }
}

bool Jgc::AJgcCustomizationInputComponent::IsWindowOpen() const noexcept
{
    return this->Window.IsValidSlow();
}

void Jgc::AJgcCustomizationInputComponent::AimEye() noexcept
{
    auto* Ctrl{this->GetOwningController()};
    auto* Lackey{Ctrl ? Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>() : nullptr};
    auto* Avatar{this->GetOwningAvatar()};
    auto* Eye{Avatar ? Avatar->GetEye() : nullptr};
    if (!Lackey || !Eye)
    {
        return;
    }
    LVec2F const Extent{Lackey->GetWorldViewExtent()};
    if (Extent.x < 1.0f || Extent.y < 1.0f)
    {
        return;
    }

    /*
     * The pivot is straight ahead of the eye. Turned by an angle, it is seen at tan(angle) / tan(half the horizontal
     * field of view) from the center to the edge of the view. Turning left (positive around up) moves it right.
     */
    f32 const HalfWidth{std::tan(Eye->VertFov * 0.5f) * Extent.x / Extent.y};
    f32 const FromCenter{2.0f * maths::clamp(this->OccupantViewX, 0.5f, 1.0f) - 1.0f};
    LWorldQuat const Rotation{maths::angle_axis(std::atan(FromCenter * HalfWidth), maths::up_vector<LVec3F>)};
    if (Eye->GetLocalRotator() != Rotation)
    {
        Eye->SetLocalRotatorByTeleport(Rotation);
    }
}

void Jgc::AJgcCustomizationInputComponent::OpenWindow(Jafg::LLocalLackey& Lackey)
{
    auto* Station{this->GetStation()};
    auto* Shape{Station ? Station->GetOccupantShape() : nullptr};
    if (!Shape)
    {
        LOG_WARNING(LogJgcInput, "[{}]: Possessed without an occupant to customize.", this->GetNameAsString())
        return;
    }

    /* The part of the world view left of the band that the occupant is shown in the center of. */
    std::optional<LRect2F> Area{Lackey.GetWorldViewRectInHostViewport()};
    if (Area)
    {
        Area->extent.x *= 2.0f * maths::clamp(this->OccupantViewX, 0.5f, 1.0f) - 1.0f;
    }

    /* The window takes clicks. The overlay of the world view would only draw it. */
    Jafg::WFloatingWidget& NewWindow{OpenCustomizationWindow(Lackey.GetHostViewport(), {
        .Shape = Shape->GetShape(),
        .Area = Area,
        .OnChanged = [this](LHumanoidShape const& Changed)
        {
            if (auto* Station{this->GetStation()})
            {
                if (auto* Occupant{Station->GetOccupantShape()})
                {
                    Occupant->SetShape(Changed);
                }
            }
        },
        .OnApply = [this]{ this->Apply(); },
        .OnDiscard = [this]{ this->Discard(); },
        })};
    this->Window = &NewWindow;
}

void Jgc::AJgcCustomizationInputComponent::CloseWindow() noexcept
{
    if (this->Window.IsValidSlow())
    {
        this->Window->MarkAsGarbage_v2();
    }
    this->Window.Reset();
}

std::optional<LVec2F> Jgc::AJgcCustomizationInputComponent::GetCursorLocation() const noexcept
{
    auto* Ctrl{this->GetOwningController()};
    auto* Lackey{Ctrl ? Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>() : nullptr};
    if (!Lackey)
    {
        return {};
    }
    /* Every viewport of the lackey is on the same surface. */
    auto const& Surface{Lackey->Viewport.GetSurface()};
    return Surface.HasMouseLocationForOrtho() ? std::optional<LVec2F>{Surface.GetMouseLocationValue()} : std::nullopt;
}

bool Jgc::AJgcCustomizationInputComponent::IsCursorOverWindow() const noexcept
{
    if (!this->Window.IsValidSlow())
    {
        return false;
    }

    Jafg::WFloatingWidget const& FloatingWidget{*this->Window};
    auto const& Surface{FloatingWidget.GetViewport().GetSurface()};
    if (!Surface.HasMouseLocationForOrtho())
    {
        return false;
    }
    Jafg::WNode const& Content{FloatingWidget.GetWindow()};
    return Content.AabbTest({.Translation = Content.GetTranslationFromMostOuter()}, Surface.GetMouseLocationValue());
}
