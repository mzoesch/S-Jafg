// Copyright mzoesch. All rights reserved.

#include "Framework/RigidComponent.h"
#include "Framework/Actor.h"
#include "PhysicsForeignCore.h"

#define INTERFACE this->GetParentPhysicsSystem()->GetBodyInterfaceNoLock()

#if JAFG_DO_CHECKS
void Jafg::ARigidComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    check(&this->GetOwningActor().GetRootComponent() == this && "Currently this is a limitation that we want to maybe resolve later.")
}

void Jafg::ARigidComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->RigidObject.IsValid())
    {
        auto& Interface{INTERFACE};
        if (Interface.IsAdded(this->RigidObject->GetID()))
        {
            Interface.RemoveBody(this->RigidObject->GetID());
        }
        Interface.DestroyBody(this->RigidObject->GetID());
        this->RigidObject = nullptr;
        this->RemoveFromWorldRigidComponents();
    }
}
#endif /* JAFG_DO_CHECKS */

void Jafg::ARigidComponent::SetPhysicsLayer(EPhysicsLayer::value_type Layer)
{
    check(!this->RigidObject.IsValid() && "Not implemented yet.")
    this->Layer = Layer;
}

void Jafg::ARigidComponent::SetPhysicsMotion(EPhysicsMotion Motion)
{
    check(!this->RigidObject.IsValid() && "Not implemented yet.")
    this->Motion = Motion;
}

void Jafg::ARigidComponent::MakeSphere(f32 Radius)
{
    check(Tasks::IsOnMasterThread())

    auto& Interface{INTERFACE};

    bool bIsRemoved{true};
    if (this->RigidObject.IsValid())
    {
        if (this->IsAddedToSimulation())
        {
            Interface.RemoveBody(this->RigidObject->GetID());
        }
        this->DestroyRigidObjectImpl();
        bIsRemoved = false;
    }
    check(!this->RigidObject.IsValid())

    LWorldTrans WorldTransform{this->GetWorldTransformSlow()};
    check(WorldTransform.s.x == WorldTransform.s.y && WorldTransform.s.x == WorldTransform.s.z)
    JPH::BodyCreationSettings CreateSettings{
        new JPH::SphereShape{Radius * WorldTransform.s.x},
        JPH::RVec3{WorldTransform.t.x, WorldTransform.t.y, WorldTransform.t.z},
        {WorldTransform.r.x, WorldTransform.r.y, WorldTransform.r.z, WorldTransform.r.w},
        JPH::EMotionType{std::to_underlying(this->Motion)},
        this->Layer,
        };
    this->RigidObject = LRigidObject::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (bIsRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

void Jafg::ARigidComponent::MakeBox(LWorldVec3 HalfExtent, f32 ConvexRadius /* = Physx::DefaultConvexRadius */)
{
    check(Tasks::IsOnMasterThread())

    auto& Interface{INTERFACE};

    bool bIsRemoved{true};
    if (this->RigidObject.IsValid())
    {
        if (this->IsAddedToSimulation())
        {
            Interface.RemoveBody(this->RigidObject->GetID());
        }
        this->DestroyRigidObjectImpl();
        bIsRemoved = false;
    }
    check(!this->RigidObject.IsValid())

    LWorldTrans WorldTransform{this->GetWorldTransformSlow()};
    check(WorldTransform.s.x == WorldTransform.s.y && WorldTransform.s.x == WorldTransform.s.z)
    JPH::BodyCreationSettings CreateSettings{
        new JPH::BoxShape{{HalfExtent.x * WorldTransform.s.x, HalfExtent.y * WorldTransform.s.y, HalfExtent.z * WorldTransform.s.z}, ConvexRadius},
        JPH::RVec3{WorldTransform.t.x, WorldTransform.t.y, WorldTransform.t.z},
        {WorldTransform.r.x, WorldTransform.r.y, WorldTransform.r.z, WorldTransform.r.w},
        JPH::EMotionType{std::to_underlying(this->Motion)},
        this->Layer,
        };
    this->RigidObject = LRigidObject::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (bIsRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

bool Jafg::ARigidComponent::IsAddedToSimulation() const noexcept
{
    return INTERFACE.IsAdded(this->RigidObject->GetID());
}

void Jafg::ARigidComponent::AddToSimulation(ESimulationAddingBehavior Behavior)
{
    check(!this->IsAddedToSimulation())
    INTERFACE.AddBody(this->RigidObject->GetID(), JPH::EActivation{std::to_underlying(Behavior)});
}

void Jafg::ARigidComponent::RemoveFromSimulation()
{
    check(this->IsAddedToSimulation())
    INTERFACE.RemoveBody(this->RigidObject->GetID());
}

void Jafg::ARigidComponent::SetLinearVelocity(LWorldVec3 const& Velocity)
{
    check(this->IsAddedToSimulation())
    INTERFACE.SetLinearVelocity(this->RigidObject->GetID(), JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

void Jafg::ARigidComponent::OnTransformChanged(ETransformChangeFlags Change)
{
    Super::OnTransformChanged(Change);

    check(&this->GetOwningActor().GetRootComponent() == this)

    if (!(Change & ETransformChangeBits::PhysicsPoll) && this->RigidObject.IsValid() && this->IsAddedToSimulation())
    {
        auto& Interface{INTERFACE};
        if ((Change & ETransformChangeBits::Translation) && (Change & ETransformChangeBits::Rotator))
        {
            Interface.SetPositionAndRotation(this->RigidObject->GetID()
                , JPH::RVec3{this->GetLocalTranslation().x, this->GetLocalTranslation().y, this->GetLocalTranslation().z}
                , JPH::Quat{this->GetLocalRotator().x, this->GetLocalRotator().y, this->GetLocalRotator().z, this->GetLocalRotator().w}
                , JPH::EActivation::DontActivate
                );
        }
        else if (Change & ETransformChangeBits::Translation)
        {
            Interface.SetPosition(this->RigidObject->GetID()
                , JPH::RVec3{this->GetLocalTranslation().x, this->GetLocalTranslation().y, this->GetLocalTranslation().z}
                , JPH::EActivation::DontActivate
                );
        }
        else if (Change & ETransformChangeBits::Rotator)
        {
            Interface.SetRotation(this->RigidObject->GetID()
                , JPH::Quat{this->GetLocalRotator().x, this->GetLocalRotator().y, this->GetLocalRotator().z, this->GetLocalRotator().w}
                , JPH::EActivation::DontActivate
                );
        }

        if (Change & ETransformChangeBits::Scale)
        {
            checkNoEntry()
            // we should cache the current shape (either outselves or with the jph::sahep) then apply the scale. Maybe only in editor builds?
            // how much does it need to be extra stroage to save?
            // this->GetParentPhysicsSystem()->AddConstraint()
            // this->RigidObject->GetBodyCreationSettings()
            // this->UpdateComponentAabb();
        }
    }
}

void Jafg::ARigidComponent::UpdateComponentAabb() noexcept
{
    if (this->RigidObject.IsValid())
    {
        auto* Shape{this->RigidObject->GetShape()};
        check(Shape)
        // TODO: This is not correct. This does not allow for scaling. We have to divide each axis by our scale.
        check(this->GetLocalScale().x == 1.0f && this->GetLocalScale().y == 1.0f && this->GetLocalScale().z == 1.0f)
        auto Box{Shape->GetLocalBounds()};
        this->Aabb = LWorldAabb3{
            .min = LWorldVec3{Box.mMin.GetX(), Box.mMin.GetY(), Box.mMin.GetZ()},
            .max = LWorldVec3{Box.mMax.GetX(), Box.mMax.GetY(), Box.mMax.GetZ()},
            };
    }
    else
    {
        this->Aabb = maths::identity<LWorldAabb3>;
    }
}

void Jafg::ARigidComponent::AddToWorldRigidComponents() noexcept
{
    check(this->RigidObject.IsValid())
    check(!algo::contains(this->GetWorld().RigidComponents, this))
    this->GetWorld().RigidComponents.emplace_back(this);
}

void Jafg::ARigidComponent::RemoveFromWorldRigidComponents() noexcept
{
    check(!this->RigidObject.IsValid())
    algo::erase_exactly_once_checked(&this->GetWorld().RigidComponents, this);
}

void Jafg::ARigidComponent::DestroyRigidObject() noexcept
{
    this->DestroyRigidObjectImpl();
    this->RemoveFromWorldRigidComponents();
}

void Jafg::ARigidComponent::DestroyRigidObjectImpl() noexcept
{
    check(this->RigidObject.IsValid())
    INTERFACE.DestroyBody(this->RigidObject->GetID());
    this->RigidObject = nullptr;
}

#undef INTERFACE
