// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"
#include "Framework/Editor.h"
#include "Framework/PhysicsSystem.h"
#include "PhysicsForeignCore.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"

void physx::debug_renderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    check(this->Comp)

    LWorldVec3 From{static_cast<LWorldReal>(inFrom.GetX()), static_cast<LWorldReal>(inFrom.GetY()), static_cast<LWorldReal>(inFrom.GetZ())};
    LWorldVec3 To{static_cast<LWorldReal>(inTo.GetX()), static_cast<LWorldReal>(inTo.GetY()), static_cast<LWorldReal>(inTo.GetZ())};
    if (maths::squared_magnitude(To - From) > 0.001f)
    {
        this->Comp->AddLineNextTick({
            .Tint = Colors::Red,
            .Value = {.begin=From, .end=To,},
            });
    }
}

void physx::debug_renderer::DrawText3D(JPH::RVec3Arg inPosition, std::string_view const& inString, JPH::ColorArg inColor, float inHeight)
{
    check(this->Comp)

    this->Comp->AddTextNextTick({
        .Tint = LColor::FromU32(inColor.GetUInt32()),
        .Height = inHeight,
        .Location = {static_cast<LWorldReal>(inPosition.GetX()), static_cast<LWorldReal>(inPosition.GetY()), static_cast<LWorldReal>(inPosition.GetZ())},
        .Text = LString{inString},
        });
}

void Jafg::LPhysicsSystemInterface::ActivateBody(LPhysicsBodyId const& BodyId)
{
    this->Interface.ActivateBody(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::ActivateBodies(std::span<LPhysicsBodyId const*> BodyIds)
{
    this->Interface.ActivateBodies(std::bit_cast<JPH::BodyID const*>(BodyIds.data()), static_cast<int>(BodyIds.size()));
}

void Jafg::LPhysicsSystemInterface::DeactivateBody(LPhysicsBodyId const& BodyId)
{
    this->Interface.DeactivateBody(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::DeactivateBodies(std::span<LPhysicsBodyId const*> BodyIds)
{
    this->Interface.DeactivateBodies(std::bit_cast<JPH::BodyID const*>(BodyIds.data()), static_cast<int>(BodyIds.size()));
}

bool Jafg::LPhysicsSystemInterface::IsActive(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.IsActive(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::ResetSleepTimer(LPhysicsBodyId const& BodyId)
{
    this->Interface.ResetSleepTimer(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetPositionAndRotation(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.SetPositionAndRotation(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::RVec3{Position.x, Position.y, Position.z}
        , JPH::Quat{Rotation.x, Rotation.y, Rotation.z, Rotation.w}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::SetPositionAndRotationWhenChanged(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.SetPositionAndRotationWhenChanged(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::RVec3{Position.x, Position.y, Position.z}
        , JPH::Quat{Rotation.x, Rotation.y, Rotation.z, Rotation.w}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::GetPositionAndRotation(LPhysicsBodyId const& BodyId, LWorldVec3& OutPosition, LWorldQuat& OutRotation) const
{
    JPH::RVec3 Position;
    JPH::Quat Rotation;
    this->Interface.GetPositionAndRotation(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, Position, Rotation);
    OutPosition = LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
    OutRotation = LWorldQuat{Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()};
}

void Jafg::LPhysicsSystemInterface::SetPosition(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.SetPosition(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::RVec3{Position.x, Position.y, Position.z}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

LWorldVec3 Jafg::LPhysicsSystemInterface::GetPosition(LPhysicsBodyId const& BodyId) const
{
    JPH::RVec3 Position{this->Interface.GetPosition(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()})};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

LWorldVec3 Jafg::LPhysicsSystemInterface::GetCenterOfMassPosition(LPhysicsBodyId const& BodyId) const
{
    JPH::RVec3 Position{this->Interface.GetCenterOfMassPosition(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()})};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

void Jafg::LPhysicsSystemInterface::SetRotation(LPhysicsBodyId const& BodyId, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.SetRotation(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Quat{Rotation.x, Rotation.y, Rotation.z, Rotation.w}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

LWorldQuat Jafg::LPhysicsSystemInterface::GetRotation(LPhysicsBodyId const& BodyId) const
{
    JPH::Quat Rotation{this->Interface.GetRotation(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()})};
    return LWorldQuat{Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()};
}

LWorldMat4 Jafg::LPhysicsSystemInterface::GetWorldTransform(LPhysicsBodyId const& BodyId) const
{
    // TODO: Check for correctness
    return std::bit_cast<LWorldMat4>(this->Interface.GetWorldTransform(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

LWorldMat4 Jafg::LPhysicsSystemInterface::GetCenterOfMassTransform(LPhysicsBodyId const& BodyId) const
{
    // TODO: Check for correctness
    return std::bit_cast<LWorldMat4>(this->Interface.GetCenterOfMassTransform(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

void Jafg::LPhysicsSystemInterface::MoveKinematic(LPhysicsBodyId const& BodyId, LWorldVec3 const& TargetPosition, LWorldQuat const& TargetRotation, f32 DeltaTime)
{
    this->Interface.MoveKinematic(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{TargetPosition.x, TargetPosition.y, TargetPosition.z}
        , JPH::Quat{TargetRotation.x, TargetRotation.y, TargetRotation.z, TargetRotation.w}
        , DeltaTime
        );
}

void Jafg::LPhysicsSystemInterface::SetLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity)
{
    this->Interface.SetLinearAndAngularVelocity(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{LinearVelocity.x, LinearVelocity.y, LinearVelocity.z,}
        , JPH::Vec3{AngularVelocity.x, AngularVelocity.y, AngularVelocity.z,}
        );
}

void Jafg::LPhysicsSystemInterface::GetLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F& OutLinearVelocity, LVec3F& OutAngularVelocity) const
{
    JPH::Vec3 LinearVelocity;
    JPH::Vec3 AngularVelocity;
    this->Interface.GetLinearAndAngularVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, LinearVelocity, AngularVelocity);
    OutLinearVelocity = LWorldVec3{LinearVelocity.GetX(), LinearVelocity.GetY(), LinearVelocity.GetZ(),};
    OutAngularVelocity = LWorldVec3{AngularVelocity.GetX(), AngularVelocity.GetY(), AngularVelocity.GetZ(),};
}

void Jafg::LPhysicsSystemInterface::SetLinearVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity)
{
    this->Interface.SetLinearVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::Vec3{LinearVelocity.x, LinearVelocity.y, LinearVelocity.z,});

}

LVec3F Jafg::LPhysicsSystemInterface::GetLinearVelocity(LPhysicsBodyId const& BodyId) const
{
    JPH::Vec3 Velocity{this->Interface.GetLinearVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()})};
    return LVec3F{Velocity.GetX(), Velocity.GetY(), Velocity.GetZ(),};
}

void Jafg::LPhysicsSystemInterface::AddLinearVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity)
{
    this->Interface.AddLinearVelocity(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{LinearVelocity.x, LinearVelocity.y, LinearVelocity.z,}
        );
}

void Jafg::LPhysicsSystemInterface::AddLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity)
{
    this->Interface.AddLinearAndAngularVelocity(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{LinearVelocity.x, LinearVelocity.y, LinearVelocity.z,}
        , JPH::Vec3{AngularVelocity.x, AngularVelocity.y, AngularVelocity.z,}
        );
}

void Jafg::LPhysicsSystemInterface::SetAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& AngularVelocity)
{
    this->Interface.SetAngularVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::Vec3{AngularVelocity.x, AngularVelocity.y, AngularVelocity.z,});
}

LVec3F Jafg::LPhysicsSystemInterface::GetAngularVelocity(LPhysicsBodyId const& BodyId) const
{
    JPH::Vec3 Velocity{this->Interface.GetAngularVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()})};
    return LVec3F{Velocity.GetX(), Velocity.GetY(), Velocity.GetZ(),};
}

LVec3F Jafg::LPhysicsSystemInterface::GetPointVelocity(LPhysicsBodyId const& BodyId, LWorldVec3 const& Point) const
{
    JPH::Vec3 Velocity{this->Interface.GetPointVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::RVec3{Point.x, Point.y, Point.z,})};
    return LVec3F{Velocity.GetX(), Velocity.GetY(), Velocity.GetZ(),};
}

void Jafg::LPhysicsSystemInterface::SetPositionRotationAndVelocity(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat Rotation, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity)
{
    this->Interface.SetPositionRotationAndVelocity(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::RVec3{Position.x, Position.y, Position.z,}
        , JPH::Quat{Rotation.x, Rotation.y, Rotation.z,Rotation.w,}
        , JPH::Vec3{LinearVelocity.x, LinearVelocity.y, LinearVelocity.z,}
        , JPH::Vec3{AngularVelocity.x, AngularVelocity.y, AngularVelocity.z,}
        );
}

void Jafg::LPhysicsSystemInterface::AddForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, ESimulationAddingBehavior ActivationMode /* = ESimulationAddingBehavior::Activate */)
{
    this->Interface.AddForce(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{Force.x, Force.y, Force.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddLocalForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.AddForce(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , this->Interface.GetRotation(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}) * JPH::Vec3{Force.x, Force.y, Force.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, LWorldVec3 const& Point, ESimulationAddingBehavior ActivationMode /* = ESimulationAddingBehavior::Activate */)
{
    this->Interface.AddForce(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{Force.x, Force.y, Force.z,}
        , JPH::RVec3{Point.x, Point.y, Point.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddTorque(LPhysicsBodyId const& BodyId, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode /* = ESimulationAddingBehavior::Activate */)
{
    this->Interface.AddTorque(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{Torque.x, Torque.y, Torque.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddLocalTorque(LPhysicsBodyId const& BodyId, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.AddTorque(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , this->Interface.GetRotation(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}) * JPH::Vec3{Torque.x, Torque.y, Torque.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddForceAndTorque(LPhysicsBodyId const& BodyId, LVec3F const& Force, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode /* = ESimulationAddingBehavior::Activate */)
{
    this->Interface.AddForceAndTorque(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::Vec3{Force.x, Force.y, Force.z,}
        , JPH::Vec3{Torque.x, Torque.y, Torque.z,}
        , static_cast<JPH::EActivation>(ActivationMode)
        );
}

void Jafg::LPhysicsSystemInterface::AddImpulse(LPhysicsBodyId const& BodyId, LVec3F const& Impulse)
{
    this->Interface.AddImpulse(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::Vec3{Impulse.x, Impulse.y, Impulse.z,});
}

void Jafg::LPhysicsSystemInterface::AddImpulse(LPhysicsBodyId const& BodyId, LVec3F const& Impulse, LWorldVec3 const& Point)
{
    this->Interface.AddImpulse(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::Vec3{Impulse.x, Impulse.y, Impulse.z,}, JPH::RVec3{Point.x, Point.y, Point.z,});
}

void Jafg::LPhysicsSystemInterface::AddAngularImpulse(LPhysicsBodyId const& BodyId, LVec3F const& AngularImpulse)
{
    this->Interface.AddAngularImpulse(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, JPH::Vec3{AngularImpulse.x, AngularImpulse.y, AngularImpulse.z,});
}

bool Jafg::LPhysicsSystemInterface::ApplyBuoyancyImpulse(
      LPhysicsBodyId const& BodyId
    , LWorldVec3 const& SurfacePosition
    , LVec3F const& SurfaceNormal
    , f32 Buoyancy
    , f32 LinearDrag
    , f32 AngularDrag
    , LVec3F const& FluidVelocity
    , LVec3F const& Gravity
    , f32 DeltaTime
    )
{
    return this->Interface.ApplyBuoyancyImpulse(
          JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}
        , JPH::RVec3{SurfacePosition.x, SurfacePosition.y, SurfacePosition.z,}
        , JPH::Vec3{SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z,}
        , Buoyancy
        , LinearDrag
        , AngularDrag
        , JPH::Vec3{FluidVelocity.x, FluidVelocity.y, FluidVelocity.z,}
        , JPH::Vec3{Gravity.x, Gravity.y, Gravity.z,}
        , DeltaTime
        );
}

Jafg::EBodyType Jafg::LPhysicsSystemInterface::GetBodyType(LPhysicsBodyId const& BodyId) const
{
    return static_cast<EBodyType>(this->Interface.GetBodyType(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

void Jafg::LPhysicsSystemInterface::SetMotionType(LPhysicsBodyId const& BodyId, EPhysicsMotion MotionType, ESimulationAddingBehavior ActivationMode)
{
    this->Interface.SetMotionType(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, static_cast<JPH::EMotionType>(MotionType), static_cast<JPH::EActivation>(ActivationMode));
}

Jafg::EPhysicsMotion Jafg::LPhysicsSystemInterface::GetMotionType(LPhysicsBodyId const& BodyId) const
{
    return static_cast<EPhysicsMotion>(this->Interface.GetMotionType(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

void Jafg::LPhysicsSystemInterface::SetMotionQuality(LPhysicsBodyId const& BodyId, EPhysicsMotionQuality Quality)
{
    this->Interface.SetMotionQuality(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, static_cast<JPH::EMotionQuality>(Quality));
}

Jafg::EPhysicsMotionQuality Jafg::LPhysicsSystemInterface::GetMotionQuality(LPhysicsBodyId const& BodyId) const
{
    return static_cast<EPhysicsMotionQuality>(this->Interface.GetMotionQuality(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

LMat4F Jafg::LPhysicsSystemInterface::GetInverseInertia(LPhysicsBodyId const& BodyId) const
{
    static_assert(sizeof(f32) == 4);
    static_assert(sizeof(LMat4F) == sizeof(JPH::Mat44));
    //# TODO: This has to be evaluated if it is correct. Especially if we use something like AVX.
    return std::bit_cast<LMat4F>(this->Interface.GetInverseInertia(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}));
}

void Jafg::LPhysicsSystemInterface::SetRestitution(LPhysicsBodyId const& BodyId, f32 Restitution)
{
    this->Interface.SetRestitution(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, Restitution);
}

f32 Jafg::LPhysicsSystemInterface::GetRestitution(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetRestitution(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetFriction(LPhysicsBodyId const& BodyId, f32 Friction)
{
    this->Interface.SetFriction(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, Friction);
}

f32 Jafg::LPhysicsSystemInterface::GetFriction(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetFriction(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetGravityFactor(LPhysicsBodyId const& BodyId, f32 GravityFactor)
{
    this->Interface.SetGravityFactor(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, GravityFactor);
}

f32 Jafg::LPhysicsSystemInterface::GetGravityFactor(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetGravityFactor(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetMaxLinearVelocity(LPhysicsBodyId const& BodyId, f32 LinearVelocity)
{
    this->Interface.SetMaxLinearVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, LinearVelocity);
}

f32 Jafg::LPhysicsSystemInterface::GetMaxLinearVelocity(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetMaxLinearVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetMaxAngularVelocity(LPhysicsBodyId const& BodyId, f32 AngularVelocity)
{
    this->Interface.SetMaxAngularVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, AngularVelocity);
}

f32 Jafg::LPhysicsSystemInterface::GetMaxAngularVelocity(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetMaxAngularVelocity(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetUseManifoldReduction(LPhysicsBodyId const& BodyId, bool bReduction)
{
    this->Interface.SetUseManifoldReduction(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, bReduction);
}

bool Jafg::LPhysicsSystemInterface::GetUseManifoldReduction(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.GetUseManifoldReduction(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetIsSensor(LPhysicsBodyId const& BodyId, bool bSensor)
{
    this->Interface.SetIsSensor(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, bSensor);
}

bool Jafg::LPhysicsSystemInterface::IsSensor(LPhysicsBodyId const& BodyId) const
{
    return this->Interface.IsSensor(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

u64 Jafg::LPhysicsSystemInterface::GetUserData(LPhysicsBodyId const& BodyId) const
{
    static_assert(sizeof(u64) == sizeof(decltype(this->Interface.GetUserData(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}))));
    return this->Interface.GetUserData(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

void Jafg::LPhysicsSystemInterface::SetUserData(LPhysicsBodyId const& BodyId, u64 UserData) const
{
    this->Interface.SetUserData(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()}, UserData);
}

void Jafg::LPhysicsSystemInterface::InvalidateContactCache(LPhysicsBodyId const& BodyId)
{
    this->Interface.InvalidateContactCache(JPH::BodyID{BodyId.GetIndexAndSequenceNumber()});
}

Jafg::LPhysicsSystem& Jafg::LPhysicsSystem::operator=(LPhysicsSystemCreateInfo Info)
{
    if (this->IsValid())
    {
        unimplemented() // TODO: implement
    }

    if (Info.PhysicsStep <= 0.0_r)
    {
        LOG_FATAL(LogPhysics, "PhysicsStep must be positive: {} <= 0.0"
            , Info.PhysicsStep)
    }
    if (Info.PhysicsStep > static_cast<LWorldReal>(1.0/20.0))
    {
        LOG_FATAL(LogPhysics, "PhysicsStep is too large: {} > 20Hz"
            , Info.PhysicsStep)
    }
    if (Info.AccumulatorLimit <= 0.0_r)
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit must be positive: {} <= 0.0"
            , Info.AccumulatorLimit)
    }
    if (Info.AccumulatorLimit > static_cast<LWorldReal>(1.0/10.0))
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit is too large: {} > 10Hz"
            , Info.AccumulatorLimit)
    }
    if (Info.AccumulatorLimit < Info.PhysicsStep)
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit must be at least PhysicsStep: {} < {}"
            , Info.AccumulatorLimit, Info.PhysicsStep)
    }
    if (Info.PhysicsSteps < 1)
    {
        LOG_FATAL(LogPhysics, "PhysicsSteps must be at least 1: {} < 1"
            , Info.PhysicsSteps)
    }
    if (Info.PhysicsSteps > 8)
    {
        LOG_FATAL(LogPhysics, "PhysicsSteps is too large: {} > 8"
            , Info.PhysicsSteps)
    }

    this->PhysicsStep = Info.PhysicsStep;
    this->Accumulator = 0.0;
    this->AccumulatorLimit = Info.AccumulatorLimit;
    this->PhysicsSteps = Info.PhysicsSteps;
    this->SubSystem = Detail::LPhysicsSubsystem::FromCreateInfo(std::move(Info)).release();

    return *this;
}

Jafg::LPhysicsSystem::~LPhysicsSystem()
{
    delete this->SubSystem;
}

JPH::PhysicsSystem* Jafg::LPhysicsSystem::operator->() noexcept
{
    check(this->IsValid())
    return &this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem const* Jafg::LPhysicsSystem::operator->() const noexcept
{
    check(this->IsValid())
    return &this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem& Jafg::LPhysicsSystem::operator*() noexcept
{
    check(this->IsValid())
    return this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem const& Jafg::LPhysicsSystem::operator*() const noexcept
{
    check(this->IsValid())
    return this->SubSystem->PhysicsSystem;
}

bool Jafg::LPhysicsSystem::TryUpdate(TFunction2<void(f32 PhysicsStep)> Callback)
{
    check(this->IsValid())

    bool Result{};
    while (this->Accumulator >= this->PhysicsStep)
    {
        /* Can only be updated by this thread. */
        check(!this->_check_IsUpdating())
        checkCode(this->_check_bUpdating.store(true))

        if (Callback)
        {
            Callback(static_cast<f32>(this->PhysicsStep));
        }

        this->SubSystem->PhysicsSystem.Update(static_cast<f32>(this->PhysicsStep) , static_cast<int>(this->PhysicsSteps)
            , &this->SubSystem->TempAllocator, &this->SubSystem->JobSystem);
        this->Accumulator -= this->PhysicsStep;

        Result = true;

        checkCode(this->_check_bUpdating.store(false))
    }

    return Result;
}

void Jafg::LPhysicsSystem::Update(std::size_t Steps, TFunction2<void(f32 PhysicsStep)> Callback)
{
    if (Steps > 0)
    {
        /* Can only be updated by this thread. */
        check(!this->_check_IsUpdating())
        checkCode(this->_check_bUpdating.store(true))

        while (Steps-- > 0)
        {
            if (Callback)
            {
                Callback(static_cast<f32>(this->PhysicsStep));
            }

            this->SubSystem->PhysicsSystem.Update(static_cast<f32>(this->PhysicsStep) , static_cast<int>(this->PhysicsSteps)
                , &this->SubSystem->TempAllocator, &this->SubSystem->JobSystem);
        }
        this->Accumulator = 0.0f;

        checkCode(this->_check_bUpdating.store(false))
    }
}

Jafg::LPhysicsSystemInterface Jafg::LPhysicsSystem::GetBodyInterfaceNoLock() noexcept
{
    check(this->IsValid())
    return LPhysicsSystemInterface{this->SubSystem->PhysicsSystem.GetBodyInterfaceNoLock()};
}
