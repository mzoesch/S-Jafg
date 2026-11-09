// Copyright mzoesch. All rights reserved.

#include "Framework/PointLightComponent.h"
#include "Framework/Actor.h"
#include "Framework/BillboardComponent.h"
#include "Framework/LightSubsystem.h"

void Jafg::APointLightComponent::OnAttach(AActor& Owner)
{
    Super::OnAttach(Owner);

    if (this->Light.bCanEverAffectWorld)
    {
        this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->RegisterLight(this->Light);
    }

#if JAFG_WITH_EDITOR
    Owner.EmplaceSceneComponentTo<ABillboardComponent>(*this, [](ABillboardComponent& Comp)
    {
        Comp.SetTexture(LTexture2::FromFile(JafgInfo/"Textures/Editor/PointLight.png", {
            .Format=vk::Format::eR8G8B8A8Srgb,
        }, {
            .Samples=vk::SampleCountFlagBits::e1,
        }));
    });
#endif /* JAFG_WITH_EDITOR */

    // TODO: Is this also editor only?
    constexpr LWorldReal HalfSize{0.25f};
    this->Aabb = LWorldAabb3{.min=LWorldVec3{-HalfSize}, .max=LWorldVec3{HalfSize}};
}

void Jafg::APointLightComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);
    if (this->Light.bCanEverAffectWorld && Reason != EJxxRecordTearDownReason::OuterTearDown)
    {
        this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->UnregisterLight(this->Light);
    }
}

bool Jafg::APointLightComponent::IsRegistered() const noexcept
{
    return algo::contains(this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->GetLights(), &this->Light);
}
