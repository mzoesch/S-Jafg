// Copyright mzoesch. All rights reserved.

#include "Framework/GimbalComponent.h"

void Jafg::AGimbalComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    this->Pivot = this->GetLocalTranslation();
    this->Apply();
}

void Jafg::AGimbalComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (!this->bRecentering || Dt <= 0.0f)
    {
        return;
    }

    f32 const Keep{std::exp(-maths::max(this->RecenterSpeed, 0.0f) * Dt)};
    this->Yaw *= Keep;
    this->Pitch *= Keep;
    this->Pan *= Keep;
    /* Logarithmic, so that zooming back in and out takes equally long. */
    this->Zoom = std::exp(std::log(this->Zoom) * Keep);
    if (maths::abs(this->Yaw) < 1.0e-4f && maths::abs(this->Pitch) < 1.0e-4f
        && maths::magnitude(this->Pan) < 1.0e-3f && maths::abs(this->Zoom - 1.0f) < 1.0e-4f)
    {
        this->Yaw = 0.0f;
        this->Pitch = 0.0f;
        this->Pan = LVec2F{0.0f};
        this->Zoom = 1.0f;
        this->bRecentering = false;
    }
    this->Apply();
}

void Jafg::AGimbalComponent::AddRotation(f32 YawDelta, f32 PitchDelta) noexcept
{
    this->bRecentering = false;
    this->Yaw += YawDelta;
    this->Pitch += PitchDelta;
    this->Apply();
}

void Jafg::AGimbalComponent::SetRotation(f32 InYaw, f32 InPitch) noexcept
{
    this->bRecentering = false;
    this->Yaw = InYaw;
    this->Pitch = InPitch;
    this->Apply();
}

void Jafg::AGimbalComponent::AddPan(LVec2F const& Delta) noexcept
{
    this->bRecentering = false;
    this->Pan += Delta;
    this->Apply();
}

void Jafg::AGimbalComponent::MultiplyZoom(f32 Factor) noexcept
{
    this->bRecentering = false;
    this->Zoom *= maths::max(Factor, 1.0e-3f);
    this->Apply();
}

void Jafg::AGimbalComponent::Recenter(bool bInstant /* = false */) noexcept
{
    if (bInstant)
    {
        this->bRecentering = false;
        this->Yaw = 0.0f;
        this->Pitch = 0.0f;
        this->Pan = LVec2F{0.0f};
        this->Zoom = 1.0f;
        this->Apply();
        return;
    }
    this->bRecentering = !this->IsCentered();
}

Jafg::AGimbalComponent* Jafg::AGimbalComponent::FindAbove(ASceneComponent& Comp) noexcept
{
    for (ASceneComponent* It{&Comp}; It; It = It->GetParent())
    {
        if (auto* Gimbal{It->As<AGimbalComponent>()})
        {
            return Gimbal;
        }
    }
    return nullptr;
}

void Jafg::AGimbalComponent::Apply() noexcept
{
    /* Around: Keep the yaw in (-pi, pi], so that recentering takes the short way. */
    if (this->MaxYaw >= 180.0f)
    {
        this->Yaw = std::remainder(this->Yaw, 2.0f * maths::pi_f);
    }
    else
    {
        f32 const Max{maths::radians(maths::max(this->MaxYaw, 0.0f))};
        this->Yaw = maths::clamp(this->Yaw, -Max, Max);
    }
    f32 const MaxPitchRadians{maths::radians(maths::clamp(this->MaxPitch, 0.0f, 89.9f))};
    this->Pitch = maths::clamp(this->Pitch, -MaxPitchRadians, MaxPitchRadians);

    this->Pan = LVec2F{
        maths::clamp(this->Pan.x, -maths::max(this->MaxPanLateral, 0.0f), maths::max(this->MaxPanLateral, 0.0f)),
        maths::clamp(this->Pan.y, -maths::max(this->MaxPanVertical, 0.0f), maths::max(this->MaxPanVertical, 0.0f)),
        };
    f32 const MinZoomClamped{maths::max(this->MinZoom, 1.0e-2f)};
    this->Zoom = maths::clamp(this->Zoom, MinZoomClamped, maths::max(this->MaxZoom, MinZoomClamped));

    LWorldQuat const Rotation{maths::normalize(
          maths::angle_axis(-this->Yaw, maths::up_vector<LVec3F>)
        * maths::angle_axis(-this->Pitch, maths::right_vector<LVec3F>)
        )};
    /* The visual right is cross(forward, up), which is -X. */
    LVec3F const PanLocal{-this->Pan.x, this->Pan.y, 0.0f};
    LWorldTrans const Transform{
        .t = this->Pivot + LWorldVec3{Rotation * PanLocal},
        .r = Rotation,
        .s = LWorldVec3{this->Zoom},
        };
    if (this->GetLocalTranslation() != Transform.t || this->GetLocalRotator() != Transform.r || this->GetLocalScale() != Transform.s)
    {
        this->SetLocalTransformByTeleport(Transform);
    }
}
