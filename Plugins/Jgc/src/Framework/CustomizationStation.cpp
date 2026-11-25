// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/CustomizationStation.h"
#include "Engine/World.h"
#include "Framework/Actor.h"
#include "Framework/AvatarComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/Frontend.h"
#include "Framework/GimbalComponent.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/MovementComponent.h"
#include "Framework/PersonaController.h"
#include "Framework/RigidComponent.h"
#include "Framework/StaticMeshComponent.h"
#include "Jgc/Framework/JgcCameraBoomComponent.h"
#include "Jgc/Framework/JgcCustomizationInputComponent.h"

void Jgc::ACustomizationStationComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    if (Reason == Jafg::EJxxRecordTearDownReason::Default)
    {
        if (this->HasOccupant())
        {
            this->Leave(false);
        }
    }
    /* Everything else is torn down as well. */
    this->Occupant = nullptr;
    this->StationAvatar = nullptr;
    this->OriginalShape.reset();

    Super::OnGarbage(Reason);
}

bool Jgc::ACustomizationStationComponent::CanInteract(Jafg::AAvatarComponent const& Instigator) const
{
    return Super::CanInteract(Instigator)
        && !this->HasOccupant()
        && this->GetOwningAvatar()
        && Instigator.IsOwningActorValid()
        && Instigator.GetOwningActor().GetComponent<AJgcHumanoidShapeComponent>();
}

bool Jgc::ACustomizationStationComponent::Interact(Jafg::AAvatarComponent& Instigator)
{
    return this->Enter(Instigator);
}

LString Jgc::ACustomizationStationComponent::GetInteractionText() const
{
    return "Customize";
}

Jafg::AAvatarComponent* Jgc::ACustomizationStationComponent::GetOccupant() const noexcept
{
    return this->Occupant && Jafg::IsValidFast(this->GetOuter(), this->Occupant) ? this->Occupant : nullptr;
}

Jgc::AJgcHumanoidShapeComponent* Jgc::ACustomizationStationComponent::GetOccupantShape() const noexcept
{
    Jafg::AAvatarComponent* const Avatar{this->GetOccupant()};
    return Avatar && Avatar->IsOwningActorValid() ? Avatar->GetOwningActor().GetComponent<AJgcHumanoidShapeComponent>() : nullptr;
}

bool Jgc::ACustomizationStationComponent::Enter(Jafg::AAvatarComponent& NewOccupant)
{
    if (this->HasOccupant())
    {
        LOG_VERBOSE(LogJgc, "[{}]: [{}] cannot enter. The station is taken by [{}]."
            , this->GetNameAsString(), NewOccupant.GetOwningActor().GetNameAsString(), this->Occupant->GetOwningActor().GetNameAsString()
            )
        return false;
    }

    Jafg::AAvatarComponent* const Avatar{this->GetOwningAvatar()};
    if (!Avatar)
    {
        LOG_WARNING(LogJgc, "[{}]: Cannot be entered. [{}] has no [{}]."
            , this->GetNameAsString(), this->GetOwningActor().GetNameAsString(), algo::type_name<Jafg::AAvatarComponent>()
            )
        return false;
    }

    auto& Actor{NewOccupant.GetOwningActor()};
    auto* Shape{Actor.GetComponent<AJgcHumanoidShapeComponent>()};
    if (!Shape)
    {
        LOG_VERBOSE(LogJgc, "[{}]: [{}] cannot enter. It has no [{}]."
            , this->GetNameAsString(), Actor.GetNameAsString(), algo::type_name<AJgcHumanoidShapeComponent>()
            )
        return false;
    }

    LOG_VERBOSE(LogJgc, "[{}]: [{}] enters.", this->GetNameAsString(), Actor.GetNameAsString())
    this->Occupant = &NewOccupant;
    this->StationAvatar = Avatar;
    this->OriginalShape = Shape->GetShape();

    /* In the center, facing forward. Slightly above the platform, so that it lands on it. */
    LWorldVec3 const Location{this->GetStandLocation() + maths::up_vector<LWorldVec3> * 0.01_r};
    LWorldQuat const Rotation{this->GetOwningActor().GetRootComponent().GetWorldTransformSlow().r};
    if (auto* Movement{Actor.GetComponent<Jafg::AMovementComponent>()})
    {
        Movement->ClearIntent();
        Movement->SetVelocity(LVec3F{0.0f});
        Movement->Teleport(Location, Rotation);
    }
    else
    {
        Actor.GetRootComponent().SetWorldTranslationByTeleport(Location);
        Actor.GetRootComponent().SetWorldRotatorByTeleport(Rotation);
    }

    /* The occupant hands its controls over to the station. */
    if (auto* Ctrl{NewOccupant.GetController()})
    {
        Ctrl->Possess(Avatar);
    }
    /* Facing forward includes the head. After the possession, which cleared the intent of the occupant. */
    if (auto* Character{Actor.GetComponent<Jafg::ACharacterMovementComponent>()})
    {
        Character->AddLookInput(LVec3F{0.0f, -Character->GetViewPitch(), 0.0f});
    }

    return true;
}

void Jgc::ACustomizationStationComponent::Leave(bool bApply)
{
    Jafg::AAvatarComponent* const OldOccupant{this->GetOccupant()};
    this->Occupant = nullptr;
    std::optional<LHumanoidShape> const Original{std::exchange(this->OriginalShape, std::nullopt)};
    Jafg::AAvatarComponent* const Avatar{std::exchange(this->StationAvatar, nullptr)};
    if (!OldOccupant)
    {
        return;
    }

    auto& Actor{OldOccupant->GetOwningActor()};
    LOG_VERBOSE(LogJgc, "[{}]: [{}] leaves. The changes are {}."
        , this->GetNameAsString(), Actor.GetNameAsString(), bApply ? "applied" : "discarded")

    if (!bApply && Original)
    {
        if (auto* Shape{Actor.GetComponent<AJgcHumanoidShapeComponent>()})
        {
            Shape->SetShape(*Original);
        }
    }

    if (Avatar && Jafg::IsValidFast(this->GetOuter(), Avatar))
    {
        if (auto* Ctrl{Avatar->GetController()})
        {
            Ctrl->Possess(OldOccupant);
        }
    }
}

LWorldVec3 Jgc::ACustomizationStationComponent::GetStandLocation() const noexcept
{
    auto const& Root{this->GetOwningActor().GetRootComponent()};

    /* The bounds of the platform. Not of everything else (e.g., the eye in front of it). */
    LWorldAabb3 Bounds{maths::identity<LWorldAabb3>};
    for (auto const& Child: Root.GetChildren())
    {
        if (Child->IsA<Jafg::AStaticMeshComponent>() && !Child->GetAabbForThisComponentOnly().empty())
        {
            Bounds.inline_merge(Child->GetAabbForThisComponentOnly().apply(Child->GetWorldTransformSlow()));
        }
    }
    if (Bounds.empty())
    {
        return Root.GetWorldTranslationSlow();
    }

    LWorldVec3 const Center{(Bounds.min + Bounds.max) * 0.5_r};
    return LWorldVec3{Center.x, Bounds.max.y, Center.z};
}

Jafg::AActor& Jgc::SpawnCustomizationStation(Jafg::LWorld& World, LWorldVec3 const& Where, LWorldQuat const& Orientation /* = maths::identity<LWorldQuat> */)
{
    auto& Actor{*Jafg::SpawnDeferredObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})([&](Jafg::AActor& Station)
    {
        Station.EditorName = "Character Customization";
        /* The gimbal and the camera boom tick. */
        Station.SetCanEverTick(true);

        auto& Rigid{Station.EmplaceRootComponent<Jafg::ARigidComponent>()};
        Rigid.SetLocalTranslationByTeleport(Where);
        Rigid.SetLocalRotatorByTeleport(Orientation);
        Station.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [&](Jafg::AStaticMeshComponent& Comp)
        {
            Comp.SetMesh(JgcInfo, "Interactables/CharacterCustomization.glb");
            Comp.SetPbrMaterialInstance(World.GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                ->GetPbrInstance(JgcInfo, "Materials/MetalPlates001"));
        });

        /*
         * The view: From the front, at the height of the chest of the occupant, looking back at it. The gimbal turns
         * the view around the occupant (see #AJgcCustomizationInputComponent). The boom is turned around, so that
         * its arm points forward. It pulls the eye in where the ground or walls are in the way, but not for
         * characters: Its pivot is inside of the occupant. The eye is turned aside by the input component, so that
         * the occupant is shown off-center. A narrow field of view keeps it from being stretched there.
         */
        auto& Gimbal{Station.EmplaceSceneComponentTo<Jafg::AGimbalComponent>(Rigid, [](Jafg::AGimbalComponent& Comp)
        {
            Comp.SetLocalTranslationByTeleport(maths::up_vector<LWorldVec3> * 1.05_r);
            Comp.MaxPitch = 75.0f;
            Comp.MinZoom = 0.4f;
            Comp.MaxZoom = 2.0f;
        })};
        auto& Boom{Station.EmplaceSceneComponentTo<AJgcCameraBoomComponent>(Gimbal, [](AJgcCameraBoomComponent& Comp)
        {
            Comp.SetLocalRotatorByTeleport(maths::radians(LWorldVec3{0, 180, 0}));
            Comp.ArmLength = 4.0f;
            Comp.SocketOffset = LVec3F{0.0f};
            Comp.Channels = Jafg::ECollisionChannelBits::Static | Jafg::ECollisionChannelBits::Dynamic;
        })};
        Station.EmplaceSceneComponentTo<Jafg::AEyeComponent>(Boom, [](Jafg::AEyeComponent& Eye)
        {
            Eye.VertFov = maths::radians(40.0f);
            Eye.BlendInTime = 0.5f;
        });

        Station.EmplaceComponent<ACustomizationStationComponent>();
        /* After the station, so that an occupant leaves before the avatar is torn down. */
        Station.EmplaceComponent<Jafg::AAvatarComponent>();
        Station.EmplaceComponent<AJgcCustomizationInputComponent>();
    })};

    auto& Rigid{Actor.GetRootComponent<Jafg::ARigidComponent>()};
    Rigid.MakeConvex();
    Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);

    return Actor;
}
