// Copyright mzoesch. All rights reserved.

#include "Framework/PointLightComponent.h"
#include "Framework/Actor.h"
#include "Framework/BillboardComponent.h"

#if JAFG_WITH_EDITOR
void Jafg::APointLightComponent::OnAttach(AActor& Owner)
{
    Super::OnAttach(Owner);

    Owner.EmplaceSceneComponentTo<ABillboardComponent>(*this, [](ABillboardComponent& Comp)
    {
        Comp.SetTexture(LTexture2::FromFile(JafgInfo/"Textures/Editor/PointLight.png", {
            .Format=vk::Format::eR8G8B8A8Srgb,
        }, {
            .Samples=vk::SampleCountFlagBits::e1,
        }));
    });

    constexpr LWorldReal HalfSize{0.25f};
    this->Aabb = LWorldAabb3{.min=LWorldVec3{-HalfSize}, .max=LWorldVec3{HalfSize}};
}
#endif /* JAFG_WITH_EDITOR */
