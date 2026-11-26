// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/CustomizationInputComponent.h"
#include "Framework/AvatarComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/GimbalComponent.h"
#include "Framework/Lackey.h"
#include "Framework/PersonaController.h"
#include "Framework/Surface.h"
#include "User/Input/InputActionValue.h"
#include "User/Input/UserInput.h"
#include "Jgc/Framework/CustomizationStation.h"
#include "Jgc/User/Frontend/CustomizationPanel.h"

void Jgc::ACustomizationInputComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* Also when the world goes away. The panel lives in the world viewport, which stays. */
    this->ClosePanel();
    Super::OnGarbage(Reason);
}

void Jgc::ACustomizationInputComponent::OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Super::OnPossessionChanged(Previous, New);

    this->Orbit.Reset();
    this->LastCursorLocation.reset();
    this->ClosePanel();

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
            this->OpenPanel(*Lackey);
        }
        return;
    }

    /* Taken away while customizing (e.g., something else was possessed). Nothing was applied. */
    if (auto* Station{this->GetStation()}; Station && Station->HasOccupant())
    {
        Station->Leave(false);
    }
}

void Jgc::ACustomizationInputComponent::ParentTick(f32 Dt)
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
        this->ClosePanel();
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

bool Jgc::ACustomizationInputComponent::ActivateUserInputContext() noexcept
{
    if (auto* Ctrl{this->GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            auto& UserInput{Lackey->GetUserInput()};
            UserInput.DeactivateAllContexts();
            bool const bActivated{UserInput.ActivateContext(Jafg::LUserInputTag::AsTagChecked("Jgc.Customization"))};
            /* The cursor is always shown, for the panel. */
            UserInput.SetConsumeMouse(false);
            UserInput.SetConfineMouse(false);
            return bActivated;
        }
    }

    return false;
}

Jgc::ACustomizationStationComponent* Jgc::ACustomizationInputComponent::GetStation() const noexcept
{
    return this->GetOwningActor().GetComponent<ACustomizationStationComponent>();
}

Jafg::AGimbalComponent* Jgc::ACustomizationInputComponent::GetOrbitGimbal() const noexcept
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

void Jgc::ACustomizationInputComponent::OnOrbitHeld() noexcept
{
    /* Pressing on the panel is not meant for the view. Once orbiting, the panel does not stop it. */
    if (this->Orbit.IsActive() || !this->IsCursorOverPanel())
    {
        this->Orbit.OnHeld();
    }
}

void Jgc::ACustomizationInputComponent::OnOrbitZoom(Jafg::LInputActionValue const& Value) noexcept
{
    if (!this->IsCursorOverPanel())
    {
        this->Orbit.OnZoom(Value.GetAxis1DValue());
    }
}

void Jgc::ACustomizationInputComponent::Apply()
{
    if (auto* Station{this->GetStation()})
    {
        Station->Leave(true);
    }
}

void Jgc::ACustomizationInputComponent::Discard()
{
    if (auto* Station{this->GetStation()})
    {
        Station->Leave(false);
    }
}

bool Jgc::ACustomizationInputComponent::IsPanelOpen() const noexcept
{
    return this->Panel.IsValidSlow();
}

void Jgc::ACustomizationInputComponent::AimEye() noexcept
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

void Jgc::ACustomizationInputComponent::OpenPanel(Jafg::LLocalLackey& Lackey)
{
    auto* Station{this->GetStation()};
    auto* Shape{Station ? Station->GetOccupantShape() : nullptr};
    if (!Shape)
    {
        LOG_WARNING(LogJgcInput, "[{}]: Possessed without an occupant to customize.", this->GetNameAsString())
        return;
    }

    /* UI of the game. Like the world, it stays inside of the world view, as in a game without the editor. */
    Jafg::LViewport* const WorldViewport{Lackey.GetWorldViewport()};
    if (!WorldViewport)
    {
        LOG_WARNING(LogJgcInput, "[{}]: Possessed by a lackey that does not show a world.", this->GetNameAsString())
        return;
    }

    Jafg::WUserWidget& NewPanel{OpenCustomizationPanel(*WorldViewport, {
        .Shape = Shape->GetShape(),
        /* The part of the view left of the band that the occupant is shown in the center of. */
        .Width = 2.0f * maths::clamp(this->OccupantViewX, 0.5f, 1.0f) - 1.0f,
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
    this->Panel = &NewPanel;
}

void Jgc::ACustomizationInputComponent::ClosePanel() noexcept
{
    if (this->Panel.IsValidSlow())
    {
        this->Panel->MarkAsGarbage_v2();
    }
    this->Panel.Reset();
}

std::optional<LVec2F> Jgc::ACustomizationInputComponent::GetCursorLocation() const noexcept
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

bool Jgc::ACustomizationInputComponent::IsCursorOverPanel() const noexcept
{
    if (!this->Panel.IsValidSlow())
    {
        return false;
    }

    Jafg::WUserWidget const& Widget{*this->Panel};
    /* In the world viewport. Empty where something of the editor is over the world view. */
    std::optional<LVec2F> const Cursor{Widget.GetViewport().GetCursorLocation()};
    return Cursor && Widget.AabbTest({.Translation = Widget.GetTranslationFromMostOuter()}, *Cursor);
}
