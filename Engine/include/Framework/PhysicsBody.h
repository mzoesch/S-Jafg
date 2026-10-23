// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PhysicsCore.h"

namespace JPH
{

class Body;
class BodyInterface;
class MotionProperties;

} /* ~Namespace JPH */

namespace Jafg
{

enum struct EBodyType: u8
{
    //# Rigid body consisting of a rigid shape.
    Rigid,
    //# Soft body consisting of a deformable shape.
    Soft,

    Count,
};

//# ID of a body. This is a way of reasoning about bodies in a multithreaded simulation while avoiding race conditions.
struct LPhysicsBodyId final
{
    //# The value for an invalid body ID.
    static constexpr u32 cInvalidBodyID{ 0xffffffff };
    //# This bit is used by the broadphase.
    static constexpr u32 cBroadPhaseBit{ 0x80000000 };
    //# Maximum value for body index (also the maximum amount of bodies supported - 1).
    static constexpr u32 cMaxBodyIndex{ 0x7fffff };
    //# Maximum value for the sequence number.
    static constexpr u8 cMaxSequenceNumber{ 0xff };
    //# Number of bits to shift to get the sequence number.
    static constexpr u32 cSequenceNumberShift{ 23 };

    constexpr LPhysicsBodyId() noexcept: Underlying{cInvalidBodyID} {}
    //# Construct from index and sequence number combined in a single u32 (use with care!).
    explicit constexpr LPhysicsBodyId(u32 Id) noexcept: Underlying(Id)
    {
        /* Check bit used by broadphase */
        check((Id & cBroadPhaseBit) == 0 || Id == cInvalidBodyID)
    }
    //# Construct from index and sequence number.
    explicit constexpr LPhysicsBodyId(u32 Id, u8 SequenceNumber) noexcept: Underlying{(static_cast<u32>(SequenceNumber) << cSequenceNumberShift) | Id}
    {
        /* Should not overlap with broadphase bit or sequence number. */
        check(Id <= cMaxBodyIndex)
    }

    //# Get index in body array.
    NODISCARD FORCEINLINE constexpr u32 GetIndex() const noexcept
    {
        return this->Underlying & cMaxBodyIndex;
    }

    //# Get sequence number of body.
    //# The sequence number can be used to check if a body ID with the same body index has been reused by another body.
    //# It is mainly used in multithreaded situations where a body is removed and its body index is immediately
    //# reused by a body created from another thread.
    //# Functions querying the broadphase can (after acquiring a body lock) detect that the body has been removed
    //# (we assume that this won't happen more than 128 times in a row).
    NODISCARD FORCEINLINE constexpr u8 GetSequenceNumber() const noexcept
    {
        return static_cast<u8>(this->Underlying >> cSequenceNumberShift);
    }

    //# Returns the index and sequence number combined in an u32.
    NODISCARD FORCEINLINE u32 GetIndexAndSequenceNumber() const noexcept { return this->Underlying; }

    //# Check if the ID is valid.
    NODISCARD FORCEINLINE constexpr bool IsInvalid() const noexcept { return this->Underlying == cInvalidBodyID; }
    NODISCARD FORCEINLINE constexpr bool IsValid() const noexcept { return !this->IsInvalid(); }

    NODISCARD FORCEINLINE constexpr bool operator==(LPhysicsBodyId const& Rhs) const noexcept { return this->Underlying == Rhs.Underlying; }
    NODISCARD FORCEINLINE constexpr bool operator!=(LPhysicsBodyId const& Rhs) const noexcept { return this->Underlying != Rhs.Underlying; }
    NODISCARD FORCEINLINE constexpr bool operator<(LPhysicsBodyId const& Rhs) const noexcept { return this->Underlying < Rhs.Underlying; }
    NODISCARD FORCEINLINE constexpr bool operator>(LPhysicsBodyId const& Rhs) const noexcept { return this->Underlying > Rhs.Underlying; }

private:

    u32 Underlying;
};

//#
//# Each rigid body has a shape attached that determines its collision.
//# The following are available (in order of computational complexity):
//#
enum struct EShape: u8
{
    //# A sphere centered around zero.
    Sphere,
    //# A box centered around zero.
    Box,
    //# A capsule centered around zero.
    Capsule,
    //# A capsule with different radii at the bottom and top.
    TaperedCapsule,
    //# A cylinder shape. Note that cylinders are the least stable of all shapes, so use another shape if possible.
    Cylinder,
    //# A cylinder with different radii at the bottom and top. Note that cylinders are the least stable of
    //# all shapes, so use another shape if possible.
    TaperedCylinder,
    //# A convex hull defined by a set of points.
    ConvexHull,
    //# A single triangle. Use a MeshShape if you have multiple triangles.
    Triangle,
    //# An infinite plane. Negative half space is considered solid.
    Plane,
    //# A shape containing other shapes. This shape is constructed once and cannot be changed afterwards.
    MutableCompound,
    //# A shape consisting of triangles. They are mostly used for static geometry.
    Mesh,
    //# A shape consisting of NxN points that define the height at each point, very suitable for representing
    //# hilly terrain. Any body that uses this shape needs to be static.
    HeightField,
    //# A shape that collides with nothing and that can be used as a placeholder or for dummy bodies.
    Empty,

    //#
    //# Next to this there are a number of decorator shapes that change the behavior of their children:
    //#

    //# This shape can scale a child shape. Note that if a shape is rotated first and then scaled,
    //# you can introduce shearing which is not supported by the library.
    Scaled,
    //# This shape can rotate and translate a child shape, it can e.g. be used to offset a sphere from the origin.
    RotatedTranslated,
    //# This shape does not change its child shape but it does shift the calculated center of mass for that shape.
    //# It allows you to e.g. shift the center of mass of a vehicle down to improve its handling.
    OffsetCenterOfMass,
};

// enum struct EMotion: u8 // TODO: What is this for?
// {
//     //# Non-movable.
//     Static,
//     //# Child shapes are organized in a tree to speed up collision detection.
//     StaticCompound,
//     //# A shape containing other shapes. This shape can be constructed/changed at runtime and trades
//     //# construction time for runtime performance. Child shapes are organized in a list to make modification easy.
//     //# Movable using velocities only, does not respond to forces.
//     Kinematic,
//     //# Responds to forces as a normal physics object.
//     Dynamic,
// };

enum struct ECanSleep
{
    Cannot = 0,
    Can = 1,
};

//# Determines the behavior of a rigid body when added to a simulation.
enum struct ESimulationAddingBehavior: i32
{
    //# Will always active the new body.
    Activate,
    //# Will leave the activation state as is.
    Preserve,
};

// struct LRigidObjectCreateInfo final // TODO: Why?
// {
//     LWorldVec3 Translation;
//     LWorldQuat Rotation;
//     // EMotion Motion; Not EPhysicsMotion?
//     EPhysicsLayer::value_type Layer;
// };

//#
//# A lightweight object that may represent any physics body in a physics system.
//# This is not a raii data structure. Lifetimes and correct access have to be implemented, monitored, and
//# restricted by a supervisor (e.g. #ARigidComponent).
//#
struct LPhysicsBody final
{
    constexpr LPhysicsBody() noexcept = default;
    constexpr LPhysicsBody(std::nullptr_t) noexcept {}
    constexpr LPhysicsBody(LPhysicsBody const&) noexcept = default;
    constexpr LPhysicsBody(LPhysicsBody&& O) noexcept: Body{std::exchange(O.Body, nullptr)} {}
    constexpr LPhysicsBody& operator=(std::nullptr_t) noexcept { this->Body = nullptr; return *this; }
    constexpr LPhysicsBody& operator=(LPhysicsBody const&) noexcept = default;
    constexpr LPhysicsBody& operator=(LPhysicsBody&& O) noexcept { this->Body = std::exchange(O.Body, nullptr); return *this; }
    constexpr ~LPhysicsBody() noexcept = default;

    NODISCARD static FORCEINLINE constexpr LPhysicsBody FromNative(JPH::Body* Ref) noexcept { return LPhysicsBody{Ref}; }

    NODISCARD FORCEINLINE constexpr bool IsValid() const noexcept { return !!this->Body; }
    NODISCARD FORCEINLINE constexpr explicit operator bool() const noexcept { return !!this->Body; }

    NODISCARD FORCEINLINE constexpr JPH::Body& GetBody() noexcept
    {
        check(this->IsValid())
        return *this->Body;
    }
    NODISCARD FORCEINLINE constexpr JPH::Body const& GetBody() const noexcept
    {
        check(this->IsValid())
        return *this->Body;
    }

    NODISCARD constexpr explicit operator JPH::Body&() const noexcept { check(this->IsValid()) return *this->Body; }
    NODISCARD constexpr explicit operator JPH::Body*() const noexcept { return this->Body; }
    NODISCARD constexpr JPH::Body& operator*() noexcept { check(this->IsValid()) return *this->Body; }
    NODISCARD constexpr JPH::Body const& operator*() const noexcept { check(this->IsValid()) return *this->Body; }
    NODISCARD constexpr JPH::Body* operator->() noexcept { check(this->IsValid()) return this->Body; }
    NODISCARD constexpr JPH::Body const* operator->() const noexcept { check(this->IsValid()) return this->Body; }

    //# Get the id of this body.
    NODISCARD ENGINE_API LPhysicsBodyId GetID() const;
    //# Constant indicating that body is not active.
    static constexpr u32 cInactiveIndex{ static_cast<u32>(-1) };

    //# Get the type of body (rigid or soft).
    NODISCARD ENGINE_API EBodyType GetBodyType() const;
    //# Check if this body is a rigid body.
    NODISCARD FORCEINLINE bool IsRigidBody() const { return this->GetBodyType() == EBodyType::Rigid; }
    //# Check if this body is a soft body.
    NODISCARD FORCEINLINE bool IsSoftBody() const { return this->GetBodyType() == EBodyType::Soft; }

    //# If this body is currently actively simulating (true) or sleeping (false).
    ENGINE_API NODISCARD bool IsActive() const;
    //# Check if this body is static (not movable).
    ENGINE_API NODISCARD bool IsStatic() const;
    //# Check if this body is kinematic (keyframed), which means that it will move according
    //# to its current velocity, but forces don't affect it.
    ENGINE_API NODISCARD bool IsKinematic() const;
    //# Check if this body is dynamic, which means that it moves and forces can act on it.
    ENGINE_API NODISCARD bool IsDynamic() const;
    //# Check if a body could be made kinematic or dynamic (if it was created dynamic or with
    //# mAllowDynamicOrKinematic set to true).
    ENGINE_API NODISCARD bool CanBeKinematicOrDynamic() const;

    //# Change the body to a sensor. A sensor will receive collision callbacks, but will not cause any collision
    //# responses and can be used as a trigger volume. The cheapest sensor (in terms of CPU usage) is a sensor with
    //# motion type Static (they can be moved around using BodyInterface::SetPosition/SetPositionAndRotation). These
    //# sensors will only detect collisions with active Dynamic or Kinematic bodies. As soon as a body go to sleep,
    //# the contact point with the sensor will be lost. If you make a sensor Dynamic or Kinematic and activate them,
    //# the sensor will be able to detect collisions with sleeping bodies too. An active sensor will never go to
    //# sleep automatically. When you make a Dynamic or Kinematic sensor, make sure it is in an ObjectLayer that does
    //# not collide with Static bodies or other sensors to avoid extra overhead in the broad phase.
    ENGINE_API void SetIsSensor(bool bSensor);
    //# Check if this body is a sensor.
    ENGINE_API NODISCARD bool IsSensor() const;

    //# If kinematic objects can generate contact points against other kinematic or static objects.
    //# Note that turning this on can be CPU intensive as much more collision detection work will be done without
    //# any effect on the simulation (kinematic objects are not affected by other kinematic/static objects). This
    //# can be used to make sensors detect static objects. Note that the sensor must be kinematic and active for it
    //# to detect static objects.
    ENGINE_API void SetCollideKinematicVsNonDynamic(bool bCollide);
    //# Check if kinematic objects can generate contact points against other kinematic or static objects.
    ENGINE_API NODISCARD bool GetCollideKinematicVsNonDynamic() const;

    //# If PhysicsSettings::mUseManifoldReduction is true, this allows turning off manifold reduction for this
    //# specific body. Manifold reduction by default will combine contacts with similar normals that come from
    //# different SubShapeIDs (e.g. different triangles in a mesh shape or different compound shapes). If the
    //# application requires tracking exactly which SubShapeIDs are in contact, you can turn off manifold reduction.
    //# Note that this comes at a performance cost. Consider using BodyInterface::SetUseManifoldReduction if the
    //# body could already be in contact with other bodies to ensure that the contact cache is invalidated and you
    //# get the correct contact callbacks.
    ENGINE_API void SetUseManifoldReduction(bool bUseReduction);
    //# Check if this body can use manifold reduction.
    ENGINE_API NODISCARD bool GetUseManifoldReduction() const;
    //# Checks if the combination of this body and inBody2 should use manifold reduction.
    ENGINE_API NODISCARD bool GetUseManifoldReductionWithBody(LPhysicsBody const& Body2) const;

    //# Set to indicate that the gyroscopic force should be applied to this body (aka Dzhanibekov effect,
    //# see https://en.wikipedia.org/wiki/Tennis_racket_theorem).
    ENGINE_API void SetApplyGyroscopicForce(bool bApply);
    //# Check if the gyroscopic force is being applied for this body.
    ENGINE_API NODISCARD bool GetApplyGyroscopicForce() const;

    //# Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal
    //# edges of a mesh). This is more expensive but makes bodies move smoother over a mesh with convex edges.
    ENGINE_API void SetEnhancedInternalEdgeRemoval(bool bApply);
    //# Check if enhanced internal edge removal is turned on.
    ENGINE_API NODISCARD bool GetEnhancedInternalEdgeRemoval() const;
    //# Checks if the combination of this body and inBody2 should use enhanced internal edge removal.
    ENGINE_API NODISCARD bool GetEnhancedInternalEdgeRemovalWithBody(LPhysicsBody const& Body2) const;

    //# Get the bodies motion type.
    NODISCARD ENGINE_API EPhysicsMotion GetMotionType() const;
    //# Set the motion type of this body. Consider using BodyInterface::SetMotionType instead of this function
    //# if the body may be active or if it needs to be activated.
    ENGINE_API void SetMotionType(EPhysicsMotion Motion);

    //# Get broadphase layer, this determines in which broad phase sub-tree the object is placed
    NODISCARD ENGINE_API EPhysicsBroadPhase::value_type GetBroadPhaseLayer() const;
    //# Get object layer, this determines which other objects it collides with
    NODISCARD ENGINE_API EPhysicsLayer::value_type GetObjectLayer() const;

    //# Collision group and sub-group ID, determines which other objects it collides with.
    // ENGINE_API NODISCARD CollisionGroup const& GetCollisionGroup() const;
    // ENGINE_API NODISCARD CollisionGroup& GetCollisionGroup();
    // ENGINE_API void SetCollisionGroup(CollisionGroup const& Group);

    //# If this body can go to sleep. Note that disabling sleeping on a sleeping object will not wake it up.
    NODISCARD ENGINE_API bool GetAllowSleeping() const;
    ENGINE_API void SetAllowSleeping(bool bAllow);
    //# Resets the sleep timer. This does not wake up the body if it is sleeping, but allows resetting
    //# the system that detects when a body is sleeping.
    ENGINE_API void ResetSleepTimer();

    //# Friction (dimensionless number, usually between 0 and 1,
    //#     0 = no friction,
    //#     1 = friction force equals force that presses the two bodies together).
    //# Note that bodies can have negative friction but the combined friction
    //# (see PhysicsSystem::SetCombineFriction) should never go below zero.
    NODISCARD ENGINE_API f32 GetFriction() const;
    ENGINE_API void SetFriction(f32 Friction);

    //# Restitution (dimensionless number, usually between 0 and 1,
    //#     0 = completely inelastic collision response,
    //#     1 = completely elastic collision response).
    //# Note that bodies can have negative restitution but the combined restitution
    //# (see PhysicsSystem::SetCombineRestitution) should never go below zero.
    NODISCARD ENGINE_API f32 GetRestitution() const;
    ENGINE_API void SetRestitution(f32 Restitution);

    //# Get world space linear velocity of the center of mass (unit: m/s).
    ENGINE_API NODISCARD LWorldVec3 GetLinearVelocity() const;
    //# Set world space linear velocity of the center of mass (unit: m/s).
    //# Note that it is illegal to set a non-zero linear velocity on a sleeping body without waking it up afterwards.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::SetLinearVelocity instead.
    ENGINE_API void SetLinearVelocity(LWorldVec3 const& Velocity);
    //# Set world space linear velocity of the center of mass (unit: m/s), will make sure the value is clamped
    //# against the maximum linear velocity.
    //# Note that it is illegal to set a non-zero linear velocity on a sleeping body without waking it up afterwards.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::SetLinearVelocity instead.
    ENGINE_API void SetLinearVelocityClamped(LWorldVec3 const& Velocity);

    //# Get world space angular velocity of the center of mass (unit: rad/s).
    ENGINE_API NODISCARD LWorldVec3 GetAngularVelocity() const;
    //# Set world space angular velocity of the center of mass (unit: rad/s).
    //# Note that it is illegal to set a non-zero angular velocity on a sleeping body without waking it up afterwards.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::SetAngularVelocity instead.
    ENGINE_API void SetAngularVelocity(LWorldVec3 const& Velocity);
    //# Set world space angular velocity of the center of mass (unit: rad/s), will make sure the value is clamped
    //# against the maximum angular velocity.
    //# Note that it is illegal to set a non-zero angular velocity on a sleeping body without waking it up afterwards.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::SetAngularVelocity instead.
    ENGINE_API void SetAngularVelocityClamped(LWorldVec3 const& Velocity);

    //# Velocity of point inPoint (in center of mass space, e.g. on the surface of the body) of the body (unit: m/s).
    ENGINE_API NODISCARD LWorldVec3 GetPointVelocityCOM(LWorldVec3 const& PointRelativeToCOM) const;
    //# Velocity of point inPoint (in world space, e.g. on the surface of the body) of the body (unit: m/s).
    ENGINE_API NODISCARD LWorldVec3 GetPointVelocity(LWorldVec3 const& Point) const;

    //# Add force (unit: N) at center of mass for the next time step, will be reset
    //# after the next call to PhysicsSystem::Update.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddForce instead.
    ENGINE_API void AddForce(LWorldVec3 const& Force);
    //# Add force (unit: N) at world space position inPosition for the next time step, will be reset
    //# after the next call to PhysicsSystem::Update.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddForce instead.
    ENGINE_API void AddForce(LWorldVec3 const& Force, LWorldVec3 const& Position);
    //# Get the total amount of force applied to the center of mass this time step (through AddForce calls).
    //# Note that it will reset to zero after PhysicsSystem::Update.
    ENGINE_API NODISCARD LWorldVec3 GetAccumulatedForce() const;
    //# Reset the total accumulated force, not that this will be done automatically after every time step.
    ENGINE_API void ResetForce();

    //# Add torque (unit: N m) for the next time step, will be reset
    //# after the next call to PhysicsSystem::Update.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddTorque instead.
    ENGINE_API void AddTorque(LWorldVec3 const& Torque);
    //# Get the total amount of torque applied to the center of mass this time step (through AddForce/AddTorque calls).
    //# Note that it will reset to zero after PhysicsSystem::Update.
    ENGINE_API NODISCARD LWorldVec3 GetAccumulatedTorque() const;
    //# Reset the total accumulated torque, not that this will be done automatically after every time step.
    ENGINE_API void ResetTorque();

    //# Reset the current velocity and accumulated force and torque.
    ENGINE_API void ResetMotion();

    //# Get inverse inertia tensor in world space.
    NODISCARD ENGINE_API LMat4F GetInverseInertia() const;

    //# Add impulse to center of mass (unit: kg m/s).
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddImpulse instead.
    ENGINE_API void AddImpulse(LWorldVec3 const& Impulse);
    //# Add impulse to point in world space (unit: kg m/s).
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddImpulse instead.
    ENGINE_API void AddImpulse(LWorldVec3 const& Impulse, LWorldVec3 const& Position);
    //# Add angular impulse in world space (unit: N m s).
    //# If you want the body to wake up when it is sleeping, use BodyInterface::AddAngularImpulse instead.
    ENGINE_API void AddAngularImpulse(LWorldVec3 const& AngularImpulse);

    //# Set velocity of body such that it will be positioned at inTargetPosition/inTargetRotation in inDeltaTime seconds.
    //# Note that it is illegal to provide a new position/orientation on a sleeping body without waking it up afterwards.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::MoveKinematic instead.
    ENGINE_API void MoveKinematic(LWorldVec3 const& TargetPosition, LQuatF TargetRotation, float DeltaTime);

    //# Gets the properties needed to do buoyancy calculations
    //# @param inSurfacePosition Position of the fluid surface in world space.
    //# @param inSurfaceNormal Normal of the fluid surface (should point up).
    //# @param outTotalVolume On return this contains the total volume of the shape.
    //# @param outSubmergedVolume On return this contains the submerged volume of the shape.
    //# @param outRelativeCenterOfBuoyancy On return this contains the center of mass of the submerged volume relative to the center of mass of the body.
    ENGINE_API void GetSubmergedVolume(LWorldVec3 const& SurfacePosition, LWorldVec3 const& SurfaceNormal, f32& OutTotalVolume, f32& OutSubmergedVolume, LWorldVec3& OutRelativeCenterOfBuoyancy) const;

    //# Applies an impulse to the body that simulates fluid buoyancy and drag.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::ApplyBuoyancyImpulse instead.
    //# @param inSurfacePosition Position of the fluid surface in world space.
    //# @param inSurfaceNormal Normal of the fluid surface (should point up).
    //# @param inBuoyancy The buoyancy factor for the body. 1 = neutral body, < 1 sinks, > 1 floats. Note that we don't use the fluid density since it is harder to configure than a simple number between [0, 2].
    //# @param inLinearDrag Linear drag factor that slows down the body when in the fluid (approx. 0.5).
    //# @param inAngularDrag Angular drag factor that slows down rotation when the body is in the fluid (approx. 0.01).
    //# @param inFluidVelocity The average velocity of the fluid (in m/s) in which the body resides.
    //# @param inGravity The gravity vector (pointing down).
    //# @param inDeltaTime Delta time of the next simulation step (in s).
    //# @return true if an impulse was applied, false if the body was not in the fluid.
    ENGINE_API bool ApplyBuoyancyImpulse(LWorldVec3 const& SurfacePosition, LWorldVec3 const& SurfaceNormal, f32 Buoyancy, f32 LinearDrag, f32 AngularDrag, LWorldVec3 const& FluidVelocity, LWorldVec3 const& Gravity, f32 DeltaTime);

    //# Applies an impulse to the body that simulates fluid buoyancy and drag.
    //# If you want the body to wake up when it is sleeping, use BodyInterface::ApplyBuoyancyImpulse instead.
    //# @param inTotalVolume Total volume of the shape of this body (m^3).
    //# @param inSubmergedVolume Submerged volume of the shape of this body (m^3).
    //# @param inRelativeCenterOfBuoyancy The center of mass of the submerged volume relative to the center of mass of the body.
    //# @param inBuoyancy The buoyancy factor for the body. 1 = neutral body, < 1 sinks, > 1 floats. Note that we don't use the fluid density since it is harder to configure than a simple number between [0, 2].
    //# @param inLinearDrag Linear drag factor that slows down the body when in the fluid (approx. 0.5).
    //# @param inAngularDrag Angular drag factor that slows down rotation when the body is in the fluid (approx. 0.01).
    //# @param inFluidVelocity The average velocity of the fluid (in m/s) in which the body resides.
    //# @param inGravity The gravity vector (pointing down).
    //# @param inDeltaTime Delta time of the next simulation step (in s).
    //# @return true if an impulse was applied, false if the body was not in the fluid.
    ENGINE_API bool ApplyBuoyancyImpulse(f32 TotalVolume, f32 SubmergedVolume, LWorldVec3 const& RelativeCenterOfBuoyancy, f32 Buoyancy, f32 LinearDrag, f32 AngularDrag, LWorldVec3 const& FluidVelocity, LWorldVec3 const& Gravity, f32 DeltaTime);

    //# Check if this body has been added to the physics system.
    ENGINE_API NODISCARD bool IsInBroadPhase() const;
    //# Check if this body has been changed in such a way that the collision cache should be considered invalid
    //# for any body interacting with this body.
    ENGINE_API NODISCARD bool IsCollisionCacheInvalid() const;

    //# Get the shape of this body.
    // ENGINE_API NODISCARD Shape const* GetShape() const;

    //# World space position of the body.
    ENGINE_API NODISCARD LWorldVec3 GetPosition() const;

    //# World space rotation of the body.
    ENGINE_API NODISCARD LWorldQuat GetRotation() const;

    //# Calculates the transform of this body.
    // ENGINE_API NODISCARD RMat44 GetWorldTransform() const;

    //# Gets the world space position of this body's center of mass.
    ENGINE_API NODISCARD LWorldVec3 GetCenterOfMassPosition() const;
    //# Calculates the transform for this body's center of mass.
    // inline RMat44 GetCenterOfMassTransform() const;
    //# Calculates the inverse of the transform for this body's center of mass.
    // inline RMat44 GetInverseCenterOfMassTransform() const;

    //# Get world space bounding box.
    // ENGINE_API NODISCARD AABox const& GetWorldSpaceBounds() const;

#if JAFG_DO_CHECKS
    //# Validate that the cached bounding box of the body matches the actual bounding box of the body.
    //# If this check fails then there are a number of possible causes:
    //# 1. Shape is being modified without notifying the system of the change. E.g. if you modify a MutableCompoundShape
    //# without calling BodyInterface::NotifyShapeChanged then there will be a mismatch between the cached bounding box
    //# in the broad phase and the bounding box of the Shape.
    //# 2. You are calling functions postfixed with 'Internal' which are not meant to be called by the application.
    //# 3. If the actual bounds and cached bounds are very close, it could mean that you have a mismatch in floating
    //# point unit state between threads. E.g. one thread has flush to zero (FTZ) or denormals are zero (DAZ) set and
    //# the other thread does not. Or if the rounding mode differs between threads. This can cause small differences
    //# in floating point calculations. If you are using JobSystemThreadPool you can use JobSystemThreadPool::SetThreadInitFunction
    //# to initialize the floating point unit state.
    ENGINE_API void ValidateCachedBounds() const;
    //# Validates that a body that is sleeping has zero velocity.
    ENGINE_API void ValidateMotion() const;
#endif /* JAFG_DO_CHECKS */

    //# Access to the motion properties. Must be not static.
    ENGINE_API NODISCARD JPH::MotionProperties const& GetMotionProperties() const;
    ENGINE_API NODISCARD JPH::MotionProperties& GetMotionProperties();
    //# Access to the motion properties (version that does not check if the object is kinematic or dynamic).
    ENGINE_API NODISCARD JPH::MotionProperties const* GetMotionPropertiesUnchecked() const;
    ENGINE_API NODISCARD JPH::MotionProperties* GetMotionPropertiesUnchecked();

    //# Access to the user data, can be used for anything by the application.
    ENGINE_API NODISCARD u64 GetUserData() const;
    ENGINE_API void SetUserData(u64 UserData);

    //# Get surface normal of a particular sub shape and its world space surface position on this body
    // inline Vec3 GetWorldSpaceSurfaceNormal(const SubShapeID &inSubShapeID, RVec3Arg inPosition) const;
    //# Get the transformed shape of this body, which can be used to do collision detection outside of a body lock
    // inline TransformedShape GetTransformedShape() const;

    //# Function to convert a body back to a body creation settings object to be able to save/recreate the body later. Can e.g. be used to move a body from one PhysicsSystem to another.
    // BodyCreationSettings GetBodyCreationSettings() const;
    //# Function to overwrite current body state with inBodyCreationSettings. Can only be done when the body is not in the physics system and cannot add MotionProperties if the body was created without one.
    // void ApplyBodyCreationSettings(const BodyCreationSettings &inBodyCreationSettings, const BroadPhaseLayerInterface &inBPLInterface);
    //# Function to convert a soft body back to a body creation settings object to be able to save/recreate the body later. Can e.g. be used to move a body from one PhysicsSystem to another.
    // SoftBodyCreationSettings GetSoftBodyCreationSettings() const;
    //# Function to overwrite current body state with inSoftBodyCreationSettings. Can only be done when the body is not in the physics system and cannot add MotionProperties if the body was created without one.
    // void ApplySoftBodyCreationSettings(const SoftBodyCreationSettings &inSoftBodyCreationSettings, const BroadPhaseLayerInterface &inBPLInterface);

    //# A dummy body that can be used by constraints to attach a constraint to the world instead of another body.
    static LPhysicsBody FixedToWorld;

private:

    constexpr explicit LPhysicsBody(JPH::Body* Ref) noexcept : Body{Ref} {}

    JPH::Body* Body{};
};

} /* ~Namespace Jafg */
