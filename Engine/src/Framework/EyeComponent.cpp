// Copyright mzoesch. All rights reserved.

#include "Framework/EyeComponent.h"

LWorldEye Jafg::AEyeComponent::GetEye() const noexcept
{
    LWorldTrans const World{this->GetWorldTransformSlow()};
    LWorldQuat const Rotation{maths::normalize(World.r)};
    return {
        .vert_fov = this->VertFov,
        .near_frustum = this->NearFrustum,
        .far_frustum = this->FarFrustum,
        .translation = World.t,
        .front = Rotation * maths::forward_vector<LWorldVec3>,
        .up = Rotation * maths::up_vector<LWorldVec3>,
        };
}

Jafg::AEyeComponent* Jafg::AEyeComponent::FindFirstEyeIn(ASceneComponent const& Root) noexcept
{
    if (auto* Eye{const_cast<ASceneComponent&>(Root).As<AEyeComponent>()})
    {
        return Eye;
    }
    for (auto const& Child: Root.GetChildren())
    {
        if (auto* Eye{FindFirstEyeIn(*Child)})
        {
            return Eye;
        }
    }
    return nullptr;
}
