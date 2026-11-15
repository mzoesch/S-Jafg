// Copyright mzoesch. All rights reserved.

#include "Framework/LightComponent.h"
#include "Framework/Actor.h"
#include "Framework/BillboardComponent.h"
#include "Framework/LightSubsystem.h"
#include "Nodes/Node.h"

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<Jafg::LLight>(TEditorNodeCreateInfo<LLight> const& Info) noexcept
{
    TFunction2<void()> Dummy;
    return
        GetEditorNode<ELight>(ProjectEditorNodeCreateInfo<&LLight::Type>(Info, {
            .RecordName = "Type",
            .UpdateValue = Dummy,
            }))
        + GetEditorNode<bool>(ProjectEditorNodeCreateInfo<&LLight::bEnabled>(Info, {
            .RecordName = "Enabled",
            .UpdateValue = Dummy,
            }, true))
        + GetEditorNode<bool>(ProjectEditorNodeCreateInfo<&LLight::bCastShadows>(Info, {
            .RecordName = "Cast Shadows",
            .RecordTooltip = "Whether this light casts shadows. Directional lights only cast them up to a distance from the camera.",
            .UpdateValue = Dummy,
            }, false))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Intensity>(Info, {
            .RecordName = "Intensity",
            .RecordTooltip = "Intensity of the light.",
            .UpdateValue = Dummy,
            }, maths::pi_f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SpecularScale>(Info, {
            .RecordName = "Specular Scale",
            .RecordTooltip = "Scale of the specular highlights.",
            .UpdateValue = Dummy,
            }, 1.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Range>(Info, {
            .RecordName = "Range",
            .RecordTooltip = "Distance at which the light is smoothly cut off. Zero means that the light reaches infinitely far. Used by: ELight::Point, ELight::Spot, ELight::Rect",
            .UpdateValue = Dummy,
            }, 8.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SourceRadius>(Info, {
            .RecordName = "Source Radius",
            .RecordTooltip = "Radius of the sphere that emits the light. Used by: ELight::Point, ELight::Spot",
            .UpdateValue = Dummy,
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SourceAngle>(Info, {
            .RecordName = "Source Angle",
            .RecordTooltip = "Angle of the light's source. Used by: ELight::Directional",
            .UpdateValue = Dummy,
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::InnerConeAngle>(Info, {
            .RecordName = "Inner Cone Angle",
            .RecordTooltip = "Angle between the axis of the cone and where the light starts to fade out. Used by: ELight::Spot",
            .UpdateValue = Dummy,
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::OuterConeAngle>(Info, {
            .RecordName = "Outer Cone Angle",
            .RecordTooltip = "Angle between the axis of the cone and its edge, where the light is faded out entirely. Used by: ELight::Spot",
            .UpdateValue = Dummy,
            }, maths::pi_f * 0.25f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Width>(Info, {
            .RecordName = "Width",
            .RecordTooltip = "Width of the emitting rectangle along its right axis. Used by: ELight::Rect",
            .UpdateValue = Dummy,
            }, 1.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Height>(Info, {
            .RecordName = "Height",
            .RecordTooltip = "Height of the emitting rectangle along its up axis. Used by: ELight::Rect",
            .UpdateValue = Dummy,
            }, 1.0f))
        ;
}

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

void Jafg::ALightComponent::SetDirection(LVec3F const& Direction) noexcept
{
    check(maths::squared_magnitude(Direction) > 0.0f)

    //# Build an upright basis around the direction. Directions along the up axis need another reference axis.
    LVec3F const Forward{maths::normalize(Direction)};
    LVec3F const Reference{maths::abs(Forward.y) < 0.999f ? maths::up_vector<LVec3F> : maths::forward_vector<LVec3F>};
    LVec3F const Right{maths::normalize(maths::cross(Reference, Forward))};
    LVec3F const Up{maths::cross(Forward, Right)};
    LWorldQuat const World{glm::quat_cast(LMat3F{Right, Up, Forward})};

    this->SetLocalRotatorByTeleport(this->HasParent()
        ? maths::inverse(this->GetParent()->GetWorldRotationSlow()) * World
        : World
        );
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
