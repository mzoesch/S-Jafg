// Copyright mzoesch. All rights reserved.

#include "Framework/LightComponent.h"
#include "Framework/Actor.h"
#include "Framework/BillboardComponent.h"
#include "Framework/LightSubsystem.h"
#include "Framework/Editor.h"
#include "Nodes/Node.h"

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<Jafg::LLight>(TEditorNodeCreateInfo<LLight> const& Info) noexcept
{
    return
        GetEditorNode<ELight>(ProjectEditorNodeCreateInfo<&LLight::Type>(Info, {
            .RecordName = "Type",
            }))
        + GetEditorNode<bool>(ProjectEditorNodeCreateInfo<&LLight::bEnabled>(Info, {
            .RecordName = "Enabled",
            }, true))
        + GetEditorNode<bool>(ProjectEditorNodeCreateInfo<&LLight::bCastShadows>(Info, {
            .RecordName = "Cast Shadows",
            .RecordTooltip = "Whether this light casts shadows. Directional lights only cast them up to a distance from the camera.",
            }, false))
        + GetEditorNode<LLinearColor3>(ProjectEditorNodeCreateInfo<&LLight::Tint>(Info, {
            .RecordName = "Tint",
            .RecordTooltip = "Tint of the light.",
            }, LinearColors3::White))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Intensity>(Info, {
            .RecordName = "Intensity",
            .RecordTooltip = "Intensity of the light.",
            }, maths::pi_f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SpecularScale>(Info, {
            .RecordName = "Specular Scale",
            .RecordTooltip = "Scale of the specular highlights.",
            }, 1.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Range>(Info, {
            .RecordName = "Range",
            .RecordTooltip = "Distance at which the light is smoothly cut off. Zero means that the light reaches infinitely far. Used by: ELight::Point, ELight::Spot, ELight::Rect",
            }, 8.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SourceRadius>(Info, {
            .RecordName = "Source Radius",
            .RecordTooltip = "Radius of the sphere that emits the light. Used by: ELight::Point, ELight::Spot",
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::SourceAngle>(Info, {
            .RecordName = "Source Angle",
            .RecordTooltip = "Angle of the light's source. Used by: ELight::Directional",
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::InnerConeAngle>(Info, {
            .RecordName = "Inner Cone Angle",
            .RecordTooltip = "Angle between the axis of the cone and where the light starts to fade out. Used by: ELight::Spot",
            }, 0.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::OuterConeAngle>(Info, {
            .RecordName = "Outer Cone Angle",
            .RecordTooltip = "Angle between the axis of the cone and its edge, where the light is faded out entirely. Used by: ELight::Spot",
            }, maths::pi_f * 0.25f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Width>(Info, {
            .RecordName = "Width",
            .RecordTooltip = "Width of the emitting rectangle along its right axis. Used by: ELight::Rect",
            }, 1.0f))
        + GetEditorNode<f32>(ProjectEditorNodeCreateInfo<&LLight::Height>(Info, {
            .RecordName = "Height",
            .RecordTooltip = "Height of the emitting rectangle along its up axis. Used by: ELight::Rect",
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

#if JAFG_WITH_EDITOR
void Jafg::ALightComponent::SelectedTick(WEditorWorldViewer& Viewer, f32 Dt)
{
    Super::SelectedTick(Viewer, Dt);

    if (this->CanEverAffectWorld() && this->IsEnabled())
    {
        if (APersonaController* Pc{Viewer.GetOwnedPersonaController()})
        {
            if (ADebugDrawerComponent* DebugDrawer{Pc->GetComponent<ADebugDrawerComponent>()})
            {
                if (this->GetType() == ELight::Directional)
                {
                    DebugDrawer->AddRayNextTick({
                        .Tint=LColor::FromLinear(this->GetTint()),
                        .Value={
                            .origin = this->Light.Translation,
                            .direction = maths::normalize(this->GetDirection()),
                            .magnitude = 2.0f,
                            },
                        });
                }

                if (this->GetType() == ELight::Point)
                {
                    DebugDrawer->AddSphereNextTick({
                        .Tint=LColor::FromLinear(this->GetTint()),
                        .Center=this->Light.Translation,
                        .Radius=this->Light.Range,
                        });
                }

                LColor const Tint{LColor::FromLinear(this->GetTint())};
                LWorldVec3 const Origin{this->Light.Translation};
                LWorldVec3 const Forward{this->Light.GetDirection()};
                LWorldVec3 const Right{this->Light.GetRight()};
                LWorldVec3 const Up{this->Light.GetUp()};
                //# Lights with an infinite range still need something visible.
                LWorldReal const Length{this->Light.Range > 0.0f ? static_cast<LWorldReal>(this->Light.Range) : 2.0f};

                auto AddLine{[DebugDrawer, &Tint](LWorldVec3 const& Begin, LWorldVec3 const& End)
                {
                    DebugDrawer->AddLineNextTick({.Tint=Tint, .Value={.begin=Begin, .end=End}});
                }};

                if (this->GetType() == ELight::Spot)
                {
                    //# A cone whose slant edges have the length of the range, so that its rim lies on the range sphere.
                    auto AddCone{[&](f32 HalfAngle, u32 SlantCount)
                    {
                        constexpr u32 RimSegments{32};
                        LWorldReal const RimDistance{Length * maths::cos(static_cast<LWorldReal>(HalfAngle))};
                        LWorldReal const RimRadius{Length * maths::sin(static_cast<LWorldReal>(HalfAngle))};
                        auto RimPoint{[&](u32 Segment) -> LWorldVec3
                        {
                            LWorldReal const Phi{maths::two_pi_v<LWorldReal> * static_cast<LWorldReal>(Segment) / static_cast<LWorldReal>(RimSegments)};
                            return Origin + Forward * RimDistance + (Right * maths::cos(Phi) + Up * maths::sin(Phi)) * RimRadius;
                        }};
                        for (u32 Segment{0}; Segment < RimSegments; ++Segment)
                        {
                            AddLine(RimPoint(Segment), RimPoint((Segment + 1) % RimSegments));
                        }
                        for (u32 Slant{0}; Slant < SlantCount; ++Slant)
                        {
                            AddLine(Origin, RimPoint(Slant * (RimSegments / SlantCount)));
                        }
                    }};

                    f32 const Outer{maths::clamp(this->Light.OuterConeAngle, 0.0f, maths::pi_f)};
                    f32 const Inner{maths::clamp(this->Light.InnerConeAngle, 0.0f, Outer)};
                    AddCone(Outer, 8);
                    if (Inner > 0.0f && Inner < Outer)
                    {
                        AddCone(Inner, 4);
                    }
                    AddLine(Origin, Origin + Forward * Length);
                }

                if (this->GetType() == ELight::Rect)
                {
                    LWorldVec3 const HalfRight{Right * static_cast<LWorldReal>(this->Light.Width * 0.5f)};
                    LWorldVec3 const HalfUp{Up * static_cast<LWorldReal>(this->Light.Height * 0.5f)};
                    std::array const Corners{
                        Origin - HalfRight - HalfUp,
                        Origin + HalfRight - HalfUp,
                        Origin + HalfRight + HalfUp,
                        Origin - HalfRight + HalfUp,
                        };
                    for (u64 i{0}; i < Corners.size(); ++i)
                    {
                        AddLine(Corners[i], Corners[(i + 1) % Corners.size()]);
                    }

                    //# Diagonals to make the emitting face readable, and its normal to show which side emits.
                    AddLine(Corners[0], Corners[2]);
                    AddLine(Corners[1], Corners[3]);
                    AddLine(Origin, Origin + Forward * maths::min(Length, static_cast<LWorldReal>(1.0f)));

                    //# The emitting face pushed out to the range along its normal, connected corner to corner.
                    LWorldVec3 const RangeOffset{Forward * Length};
                    for (u64 i{0}; i < Corners.size(); ++i)
                    {
                        AddLine(Corners[i] + RangeOffset, Corners[(i + 1) % Corners.size()] + RangeOffset);
                        AddLine(Corners[i], Corners[i] + RangeOffset);
                    }
                }
            }
        }
    }
}
#endif /* JAFG_WITH_EDITOR */

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
    LPath Path{TypeToTexture(this->Light.Type)};
    this->GetComponentChecked<ABillboardComponent>().SetTexture(LTexture2::FromFile(
          LPath{Path}.replace_extension(), Path
        , {.Format=vk::Format::eR8G8B8A8Srgb,}
        , {.Samples=vk::SampleCountFlagBits::e1,}
        ));
}
#endif /* JAFG_WITH_EDITOR */
