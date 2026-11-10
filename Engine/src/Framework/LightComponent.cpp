// Copyright mzoesch. All rights reserved.

#include "Framework/LightComponent.h"
#include "Framework/Actor.h"
#include "Framework/BillboardComponent.h"
#include "Framework/LightSubsystem.h"

void Jafg::ALightComponent::OnAttach(AActor& Owner)
{
    Super::OnAttach(Owner);

    if (this->Light.bCanEverAffectWorld)
    {
        this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->RegisterLight(this->Light);
    }

    // TODO: Is this also editor only?
    constexpr LWorldReal HalfSize{0.25f};
    this->Aabb = LWorldAabb3{.min=LWorldVec3{-HalfSize}, .max=LWorldVec3{HalfSize}};
}

void Jafg::ALightComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);
    if (this->Light.bCanEverAffectWorld && Reason != EJxxRecordTearDownReason::OuterTearDown)
    {
        this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->UnregisterLight(this->Light);
    }
}

bool Jafg::ALightComponent::IsRegistered() const noexcept
{
    return algo::contains(this->GetWorld().GetSubsystemChecked<JLightSubsystem>()->GetLights(), &this->Light);
}

void Jafg::ALightComponent::SetLight(LLight const& Light)
{
    this->Light = Light;
    this->UpdateLightTransform();
    this->OnTypeChanged();
}

#if JAFG_WITH_EDITOR
void Jafg::ALightComponent::CreateDefaultComponents()
{
    this->EmplaceDefaultSceneComponent<ABillboardComponent>();
    this->OnTypeChanged();
}

void Jafg::ALightComponent::OnTypeChanged()
{
    auto TypeToTexture{[](ELight Light)
    {
        switch (Light)
        {
            case ELight::Directional: return JafgInfo/"Textures/Editor/DirectionalLight.png";
            case ELight::Point:       return JafgInfo/"Textures/Editor/PointLight.png";
            case ELight::Spot:        return JafgInfo/"Textures/Editor/SpotLight.png";
            case ELight::Rect:        return JafgInfo/"Textures/Editor/RectLight.png";
        }
        std::unreachable();
    }};
    this->GetComponentChecked<ABillboardComponent>().SetTexture(LTexture2::FromFile(
          TypeToTexture(this->Light.Type)
        , {.Format=vk::Format::eR8G8B8A8Srgb,}
        , {.Samples=vk::SampleCountFlagBits::e1,}
        ));
}
#endif /* JAFG_WITH_EDITOR */
