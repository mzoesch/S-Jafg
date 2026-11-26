// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/GimbalFreeLook.h"
#include "Framework/GimbalComponent.h"

void Jgc::LGimbalFreeLook::Apply(Jafg::AGimbalComponent* Gimbal, LGimbalFreeLookSettings const& Settings) noexcept
{
    bool const bHeld{this->bFrameHeld};
    if (bHeld)
    {
        if (!this->bActive)
        {
            this->bActive = true;
            this->Travel = 0.0f;
        }
        if (this->FrameLook != LVec2F{0.0f})
        {
            this->Travel += maths::magnitude(this->FrameLook);
            if (Gimbal && this->bFramePanHeld)
            {
                /* What is looked at follows the mouse: Moving it right pans the view left. */
                LVec2F const Delta{this->FrameLook * (Settings.PanSensitivity * Gimbal->GetZoom())};
                Gimbal->AddPan(LVec2F{-Delta.x, -Delta.y});
            }
            else if (Gimbal)
            {
                LVec2F const Delta{this->FrameLook * Settings.Sensitivity};
                /* Same as looking around with a character (#ACharacterInputComponent::OnLook). */
                Gimbal->AddRotation(Delta.x, Settings.bInvertPitch ? -Delta.y : Delta.y);
            }
        }
    }
    else if (this->bActive)
    {
        this->bActive = false;
        /* Only tapped: Back to the center. */
        if (Gimbal && this->Travel < Settings.RecenterThreshold)
        {
            Gimbal->Recenter();
        }
    }

    if (this->FrameZoom != 0.0f && (bHeld || !Settings.bZoomOnlyWhileHeld))
    {
        /* Changing anything is not a tap. */
        this->Travel += maths::max(Settings.RecenterThreshold, 0.0f);
        if (Gimbal)
        {
            Gimbal->MultiplyZoom(std::pow(maths::max(Settings.ZoomStep, 1.0f), -this->FrameZoom));
        }
    }

    this->bFrameHeld = false;
    this->bFramePanHeld = false;
    this->FrameLook = LVec2F{0.0f};
    this->FrameZoom = 0.0f;
}

void Jgc::LGimbalFreeLook::Reset() noexcept
{
    this->bFrameHeld = false;
    this->bFramePanHeld = false;
    this->FrameLook = LVec2F{0.0f};
    this->FrameZoom = 0.0f;
    this->bActive = false;
    this->Travel = 0.0f;
}
