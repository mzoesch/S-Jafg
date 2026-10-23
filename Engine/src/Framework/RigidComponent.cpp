// Copyright mzoesch. All rights reserved.

#include "Framework/RigidComponent.h"
#include "Framework/Actor.h"
#include "PhysicsForeignCore.h"

static_assert(std::to_underlying(Jafg::EBodyType::Rigid) == std::to_underlying(JPH::EBodyType::RigidBody));
static_assert(std::to_underlying(Jafg::EBodyType::Soft) == std::to_underlying(JPH::EBodyType::SoftBody));
static_assert(std::to_underlying(Jafg::EBodyType::Count) == JPH::cBodyTypeCount);
static_assert(std::is_same_v<std::underlying_type_t<Jafg::EBodyType>, std::underlying_type_t<JPH::EBodyType>>);

static_assert(Jafg::LPhysicsBodyId::cInvalidBodyID == JPH::BodyID::cInvalidBodyID);
static_assert(Jafg::LPhysicsBodyId::cBroadPhaseBit == JPH::BodyID::cBroadPhaseBit);
static_assert(Jafg::LPhysicsBodyId::cMaxBodyIndex == JPH::BodyID::cMaxBodyIndex);
static_assert(Jafg::LPhysicsBodyId::cMaxSequenceNumber == JPH::BodyID::cMaxSequenceNumber);
static_assert(Jafg::LPhysicsBodyId::cSequenceNumberShift == JPH::BodyID::cSequenceNumberShift);

static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cInvalidBodyID), decltype(JPH::BodyID::cInvalidBodyID)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cBroadPhaseBit), decltype(JPH::BodyID::cBroadPhaseBit)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cMaxBodyIndex), decltype(JPH::BodyID::cMaxBodyIndex)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cMaxSequenceNumber), decltype(JPH::BodyID::cMaxSequenceNumber)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cSequenceNumberShift), decltype(JPH::BodyID::cSequenceNumberShift)>);

static_assert(std::to_underlying(Jafg::ECanSleep::Cannot) == std::to_underlying(JPH::ECanSleep::CannotSleep));
static_assert(std::to_underlying(Jafg::ECanSleep::Can) == std::to_underlying(JPH::ECanSleep::CanSleep));
static_assert(std::is_same_v<std::underlying_type_t<Jafg::ECanSleep>, std::underlying_type_t<JPH::ECanSleep>>);

static_assert(std::is_same_v<decltype(Jafg::LPhysicsBody::cInactiveIndex), decltype(JPH::MotionProperties::cInactiveIndex)>);
static_assert(Jafg::LPhysicsBody::cInactiveIndex == JPH::MotionProperties::cInactiveIndex);

Jafg::LPhysicsBodyId Jafg::LPhysicsBody::GetID() const
{
    return LPhysicsBodyId{this->GetBody().GetID().GetIndexAndSequenceNumber()};
}

Jafg::EBodyType Jafg::LPhysicsBody::GetBodyType() const
{
    return static_cast<EBodyType>(this->GetBody().GetBodyType());
}

bool Jafg::LPhysicsBody::IsActive() const
{
    return this->GetBody().IsActive();
}

bool Jafg::LPhysicsBody::IsStatic() const
{
    return this->GetBody().IsStatic();
}

bool Jafg::LPhysicsBody::IsKinematic() const
{
    return this->GetBody().IsKinematic();
}

bool Jafg::LPhysicsBody::IsDynamic() const
{
    return this->GetBody().IsDynamic();
}

bool Jafg::LPhysicsBody::CanBeKinematicOrDynamic() const
{
    return this->GetBody().CanBeKinematicOrDynamic();
}

void Jafg::LPhysicsBody::SetIsSensor(bool bSensor)
{
    this->GetBody().SetIsSensor(bSensor);
}

bool Jafg::LPhysicsBody::IsSensor() const
{
    return this->GetBody().IsSensor();
}

void Jafg::LPhysicsBody::SetCollideKinematicVsNonDynamic(bool bCollide)
{
    this->GetBody().SetCollideKinematicVsNonDynamic(bCollide);
}

bool Jafg::LPhysicsBody::GetCollideKinematicVsNonDynamic() const
{
    return this->GetBody().GetCollideKinematicVsNonDynamic();
}

void Jafg::LPhysicsBody::SetUseManifoldReduction(bool bUseReduction)
{
    this->GetBody().SetUseManifoldReduction(bUseReduction);
}

bool Jafg::LPhysicsBody::GetUseManifoldReduction() const
{
    return this->GetBody().GetUseManifoldReduction();
}

bool Jafg::LPhysicsBody::GetUseManifoldReductionWithBody(LPhysicsBody const& Body2) const
{
    return this->GetBody().GetUseManifoldReductionWithBody(Body2.GetBody());
}

void Jafg::LPhysicsBody::SetApplyGyroscopicForce(bool bApply)
{
    this->GetBody().SetApplyGyroscopicForce(bApply);
}

bool Jafg::LPhysicsBody::GetApplyGyroscopicForce() const
{
    return this->GetBody().GetApplyGyroscopicForce();
}

void Jafg::LPhysicsBody::SetEnhancedInternalEdgeRemoval(bool bApply)
{
    this->GetBody().SetEnhancedInternalEdgeRemoval(bApply);
}

bool Jafg::LPhysicsBody::GetEnhancedInternalEdgeRemoval() const
{
    return this->GetBody().GetEnhancedInternalEdgeRemoval();
}

bool Jafg::LPhysicsBody::GetEnhancedInternalEdgeRemovalWithBody(LPhysicsBody const& Body2) const
{
    return this->GetBody().GetEnhancedInternalEdgeRemovalWithBody(Body2.GetBody());
}

Jafg::EPhysicsMotion Jafg::LPhysicsBody::GetMotionType() const
{
    return static_cast<EPhysicsMotion>(this->GetBody().GetMotionType());
}

void Jafg::LPhysicsBody::SetMotionType(EPhysicsMotion Motion)
{
    this->GetBody().SetMotionType(static_cast<JPH::EMotionType>(Motion));
}

Jafg::EPhysicsBroadPhase::value_type Jafg::LPhysicsBody::GetBroadPhaseLayer() const
{
    return static_cast<EPhysicsBroadPhase::value_type>(this->GetBody().GetBroadPhaseLayer().GetValue());
}

Jafg::EPhysicsLayer::value_type Jafg::LPhysicsBody::GetObjectLayer() const
{
    return static_cast<EPhysicsLayer::value_type>(this->GetBody().GetObjectLayer());
}

bool Jafg::LPhysicsBody::GetAllowSleeping() const
{
    return this->GetBody().GetAllowSleeping();
}

void Jafg::LPhysicsBody::SetAllowSleeping(bool bAllow)
{
    this->GetBody().SetAllowSleeping(bAllow);
}

void Jafg::LPhysicsBody::ResetSleepTimer()
{
    this->GetBody().ResetSleepTimer();
}

f32 Jafg::LPhysicsBody::GetFriction() const
{
    return this->GetBody().GetFriction();
}

void Jafg::LPhysicsBody::SetFriction(f32 Friction)
{
    this->GetBody().SetFriction(Friction);
}

f32 Jafg::LPhysicsBody::GetRestitution() const
{
    return this->GetBody().GetRestitution();
}

void Jafg::LPhysicsBody::SetRestitution(f32 Restitution)
{
    this->GetBody().SetRestitution(Restitution);
}

LVec3F Jafg::LPhysicsBody::GetLinearVelocity() const
{
    JPH::Vec3 Result{this->GetBody().GetLinearVelocity()};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::SetLinearVelocity(LVec3F const& Velocity)
{
    this->GetBody().SetLinearVelocity(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

void Jafg::LPhysicsBody::SetLinearVelocityClamped(LVec3F const& Velocity)
{
    this->GetBody().SetLinearVelocityClamped(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

LVec3F Jafg::LPhysicsBody::GetAngularVelocity() const
{
    JPH::Vec3 Result{this->GetBody().GetAngularVelocity()};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::SetAngularVelocity(LVec3F const& Velocity)
{
    this->GetBody().SetAngularVelocity(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

void Jafg::LPhysicsBody::SetAngularVelocityClamped(LVec3F const& Velocity)
{
    this->GetBody().SetAngularVelocityClamped(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

LVec3F Jafg::LPhysicsBody::GetPointVelocityCOM(LVec3F const& PointRelativeToCOM) const
{
    JPH::Vec3 Result{this->GetBody().GetPointVelocityCOM(JPH::Vec3{PointRelativeToCOM.x, PointRelativeToCOM.y, PointRelativeToCOM.z})};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

LWorldVec3 Jafg::LPhysicsBody::GetPointVelocity(LWorldVec3 const& Point) const
{
    JPH::Vec3 Result{this->GetBody().GetPointVelocity(JPH::RVec3{Point.x, Point.y, Point.z})};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::AddForce(LVec3F const& Force)
{
    this->GetBody().AddForce(JPH::Vec3{Force.x, Force.y, Force.z});
}

void Jafg::LPhysicsBody::AddForce(LVec3F const& Force, LWorldVec3 const& Position)
{
    this->GetBody().AddForce(JPH::Vec3{Force.x, Force.y, Force.z}, JPH::RVec3{Position.x, Position.y, Position.z});
}

LWorldVec3 Jafg::LPhysicsBody::GetAccumulatedForce() const
{
    JPH::Vec3 Result{this->GetBody().GetAccumulatedForce()};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::ResetForce()
{
    this->GetBody().ResetForce();
}

void Jafg::LPhysicsBody::AddTorque(LWorldVec3 const& Torque)
{
    this->GetBody().AddTorque(JPH::Vec3{Torque.x, Torque.y, Torque.z});
}

LWorldVec3 Jafg::LPhysicsBody::GetAccumulatedTorque() const
{
    JPH::Vec3 Result{this->GetBody().GetAccumulatedTorque()};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::ResetTorque()
{
    this->GetBody().ResetTorque();
}

void Jafg::LPhysicsBody::ResetMotion()
{
    this->GetBody().ResetMotion();
}

LMat4F Jafg::LPhysicsBody::GetInverseInertia() const
{
    static_assert(sizeof(f32) == 4);
    static_assert(sizeof(LMat4F) == sizeof(JPH::Mat44));
    //# TODO: This has to be evaluated if it is correct. Especially if we use something like AVX.
    return std::bit_cast<LMat4F>(this->GetBody().GetInverseInertia());
}

void Jafg::LPhysicsBody::AddImpulse(LWorldVec3 const& Impulse)
{
    this->GetBody().AddImpulse(JPH::Vec3{Impulse.x, Impulse.y, Impulse.z});
}

void Jafg::LPhysicsBody::AddImpulse(LWorldVec3 const& Impulse, LWorldVec3 const& Position)
{
    this->GetBody().AddImpulse(JPH::Vec3{Impulse.x, Impulse.y, Impulse.z}, JPH::RVec3{Position.x, Position.y, Position.z});
}

void Jafg::LPhysicsBody::AddAngularImpulse(LWorldVec3 const& AngularImpulse)
{
    this->GetBody().AddAngularImpulse(JPH::Vec3{AngularImpulse.x, AngularImpulse.y, AngularImpulse.z});
}

void Jafg::LPhysicsBody::MoveKinematic(LWorldVec3 const& TargetPosition, LQuatF TargetRotation, float DeltaTime)
{
    this->GetBody().MoveKinematic(
          JPH::RVec3{TargetPosition.x, TargetPosition.y, TargetPosition.z}
        , JPH::Quat{TargetRotation.x, TargetRotation.y, TargetRotation.z, TargetRotation.w}
        , DeltaTime
        );
}

void Jafg::LPhysicsBody::GetSubmergedVolume(
      LWorldVec3 const& SurfacePosition
    , LWorldVec3 const& SurfaceNormal
    , f32& OutTotalVolume
    , f32& OutSubmergedVolume
    , LWorldVec3& OutRelativeCenterOfBuoyancy
    ) const
{
    JPH::Vec3 RelativeCenterOfBuoyancy;
    this->GetBody().GetSubmergedVolume(
          JPH::RVec3{SurfacePosition.x, SurfacePosition.y, SurfacePosition.z}
        , JPH::Vec3{SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z}
        , OutTotalVolume
        , OutSubmergedVolume
        , RelativeCenterOfBuoyancy
        );
    OutRelativeCenterOfBuoyancy = LWorldVec3{RelativeCenterOfBuoyancy.GetX(), RelativeCenterOfBuoyancy.GetY(), RelativeCenterOfBuoyancy.GetZ()};
}

bool Jafg::LPhysicsBody::ApplyBuoyancyImpulse(
      LWorldVec3 const& SurfacePosition
    , LWorldVec3 const& SurfaceNormal
    , f32 Buoyancy
    , f32 LinearDrag
    , f32 AngularDrag
    , LWorldVec3 const& FluidVelocity
    , LWorldVec3 const& Gravity
    , f32 DeltaTime
    )
{
    return this->GetBody().ApplyBuoyancyImpulse(
          JPH::Vec3{SurfacePosition.x, SurfacePosition.y, SurfacePosition.z}
        , JPH::Vec3{SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z}
        , Buoyancy
        , LinearDrag
        , AngularDrag
        , JPH::Vec3{FluidVelocity.x, FluidVelocity.y, FluidVelocity.z}
        , JPH::Vec3{Gravity.x, Gravity.y, Gravity.z}
        , DeltaTime
        );
}

bool Jafg::LPhysicsBody::ApplyBuoyancyImpulse(
      f32 TotalVolume
    , f32 SubmergedVolume
    , LWorldVec3 const& RelativeCenterOfBuoyancy
    , f32 Buoyancy
    , f32 LinearDrag
    , f32 AngularDrag
    , LWorldVec3 const& FluidVelocity
    , LWorldVec3 const& Gravity
    , f32 DeltaTime
    )
{
    return this->GetBody().ApplyBuoyancyImpulse(
          TotalVolume
        , SubmergedVolume
        , JPH::Vec3{RelativeCenterOfBuoyancy.x, RelativeCenterOfBuoyancy.y, RelativeCenterOfBuoyancy.z}
        , Buoyancy
        , LinearDrag
        , AngularDrag
        , JPH::Vec3{FluidVelocity.x, FluidVelocity.y, FluidVelocity.z}
        , JPH::Vec3{Gravity.x, Gravity.y, Gravity.z}
        , DeltaTime
        );
}

bool Jafg::LPhysicsBody::IsInBroadPhase() const
{
    return this->GetBody().IsInBroadPhase();
}

bool Jafg::LPhysicsBody::IsCollisionCacheInvalid() const
{
    return this->GetBody().IsCollisionCacheInvalid();
}

LWorldVec3 Jafg::LPhysicsBody::GetPosition() const
{
    JPH::Vec3 Position{this->GetBody().GetPosition()};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

LWorldQuat Jafg::LPhysicsBody::GetRotation() const
{
    JPH::Quat Rotation{this->GetBody().GetRotation()};
    return LWorldQuat{Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()};
}

LWorldVec3 Jafg::LPhysicsBody::GetCenterOfMassPosition() const
{
    JPH::Vec3 Position{this->GetBody().GetPosition()};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

#if JAFG_DO_CHECKS
void Jafg::LPhysicsBody::ValidateCachedBounds() const
{
#ifdef JPH_ENABLE_ASSERTS
    this->GetBody().ValidateCachedBounds();
#endif /* JPH_ENABLE_ASSERTS */
}

void Jafg::LPhysicsBody::ValidateMotion() const
{
#ifdef JPH_ENABLE_ASSERTS
    this->GetBody().ValidateMotion();
#endif /* JPH_ENABLE_ASSERTS */
}
#endif /* JAFG_DO_CHECKS */

JPH::MotionProperties const& Jafg::LPhysicsBody::GetMotionProperties() const
{
    return *this->GetBody().GetMotionProperties();
}

JPH::MotionProperties& Jafg::LPhysicsBody::GetMotionProperties()
{
    return *this->GetBody().GetMotionProperties();
}

JPH::MotionProperties const* Jafg::LPhysicsBody::GetMotionPropertiesUnchecked() const
{
    return this->GetBody().GetMotionPropertiesUnchecked();
}

JPH::MotionProperties* Jafg::LPhysicsBody::GetMotionPropertiesUnchecked()
{
    return this->GetBody().GetMotionPropertiesUnchecked();
}

u64 Jafg::LPhysicsBody::GetUserData() const
{
    static_assert(sizeof(u64) == sizeof(decltype(this->GetBody().GetUserData())));
    return this->GetBody().GetUserData();
}

void Jafg::LPhysicsBody::SetUserData(u64 UserData)
{
    this->GetBody().SetUserData(UserData);
}

#define INTERFACE this->GetParentPhysicsSystem()->GetBodyInterfaceNoLock()

#if JAFG_DO_CHECKS
void Jafg::ARigidComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    check(&this->GetOwningActor().GetRootComponent() == this && "Currently this is a limitation that we want to maybe resolve later.")
}
#endif /* JAFG_DO_CHECKS */

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
    this->RigidObject = LPhysicsBody::FromNative(Interface.CreateBody(CreateSettings));
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
    this->RigidObject = LPhysicsBody::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (bIsRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

bool Jafg::ARigidComponent::IsAddedToSimulation() const noexcept
{
    return this->RigidObject.IsValid() && INTERFACE.IsAdded(this->RigidObject->GetID());
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
