// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PhysicsCore.h"
#include "Framework/PhysicsBody.h"

namespace JPH
{
class ObjectLayerFilter;

class PhysicsSystem;
class BodyInterface;

} /* ~Namespace JPH */

namespace Jafg
{

struct LPhysicsSystemInterface final
{
    LPhysicsSystemInterface() = delete;
    LPhysicsSystemInterface(JPH::BodyInterface& Interface) noexcept: Interface{Interface} {}

    /// Create a rigid body
    /// @return Created body or null when out of bodies
    // Body *                        CreateBody(const BodyCreationSettings &inSettings);
    /// Create a soft body
    /// @return Created body or null when out of bodies
    // Body *                        CreateSoftBody(const SoftBodyCreationSettings &inSettings);
    /// Create a rigid body with specified ID. This function can be used if a simulation is to run in sync between clients or if a simulation needs to be restored exactly.
    /// The ID created on the server can be replicated to the client and used to create a deterministic simulation.
    /// @return Created body or null when the body ID is invalid or a body of the same ID already exists.
    // Body *                        CreateBodyWithID(LPhysicsBodyId const& BodyId, const BodyCreationSettings &inSettings);
    /// Create a soft body with specified ID. See comments at CreateBodyWithID.
    // Body *                        CreateSoftBodyWithID(LPhysicsBodyId const& BodyId, const SoftBodyCreationSettings &inSettings);
    /// Advanced use only. Creates a rigid body without specifying an ID. This body cannot be added to the physics system until it has been assigned a body ID.
    /// This can be used to decouple allocation from registering the body. A call to CreateBodyWithoutID followed by AssignBodyID is equivalent to calling CreateBodyWithID.
    /// @return Created body
    // Body *                        CreateBodyWithoutID(const BodyCreationSettings &inSettings) const;
    /// Advanced use only. Creates a body without specifying an ID. See comments at CreateBodyWithoutID.
    // Body *                        CreateSoftBodyWithoutID(const SoftBodyCreationSettings &inSettings) const;
    /// Advanced use only. Destroy a body previously created with CreateBodyWithoutID that hasn't gotten an ID yet through the AssignBodyID function,
    /// or a body that has had its body ID unassigned through UnassignBodyIDs. Bodies that have an ID should be destroyed through DestroyBody.
    // void                        DestroyBodyWithoutID(Body *inBody) const;
    /// Advanced use only. Assigns the next available body ID to a body that was created using CreateBodyWithoutID. After this call, the body can be added to the physics system.
    /// @return false if the body already has an ID or out of body ids.
    // bool                        AssignBodyID(Body *ioBody);
    /// Advanced use only. Assigns a body ID to a body that was created using CreateBodyWithoutID. After this call, the body can be added to the physics system.
    /// @return false if the body already has an ID or if the ID is not valid.
    // bool                        AssignBodyID(Body *ioBody, LPhysicsBodyId const& BodyId);
    /// Advanced use only. See UnassignBodyIDs. Unassigns the ID of a single body.
    // Body *                        UnassignBodyID(LPhysicsBodyId const& BodyId);
    /// Advanced use only. Removes a number of body IDs from their bodies and returns the body pointers. Before calling this, the body should have been removed from the physics system.
    /// The body can be destroyed through DestroyBodyWithoutID. This can be used to decouple deallocation. A call to UnassignBodyIDs followed by calls to DestroyBodyWithoutID is equivalent to calling DestroyBodies.
    /// @param BodyIds A list of body IDs
    /// @param Number Number of bodies in the list
    /// @param outBodies If not null on input, this will contain a list of body pointers corresponding to BodyIds that can be destroyed afterwards (caller assumes ownership over these).
    // void                        UnassignBodyIDs(const BodyID *BodyIds, int Number, Body **outBodies);
    /// Destroy a body.
    /// Make sure that you remove the body from the physics system using BodyInterface::RemoveBody before calling this function.
    // void                        DestroyBody(LPhysicsBodyId const& BodyId);
    /// Destroy multiple bodies
    /// Make sure that you remove the bodies from the physics system using BodyInterface::RemoveBody before calling this function.
    // void                        DestroyBodies(const BodyID *BodyIds, int Number);

    /// Add body to the physics system.
    /// Note that if you need to add multiple bodies, use the AddBodiesPrepare/AddBodiesFinalize function.
    /// Adding many bodies, one at a time, results in a really inefficient broadphase until PhysicsSystem::OptimizeBroadPhase is called or when PhysicsSystem::Update rebuilds the tree!
    /// After adding, to get a body by ID use the BodyLockRead or BodyLockWrite interface!
    // void                        AddBody(LPhysicsBodyId const& BodyId, EActivation ActivationMode);
    /// Remove body from the physics system. Note that you need to add a body to the physics system before you can remove it.
    // void                        RemoveBody(LPhysicsBodyId const& BodyId);
    /// Check if a body has been added to the physics system.
    // bool                        IsAdded(LPhysicsBodyId const& BodyId) const;
    /// Combines CreateBody and AddBody
    /// @return Created body ID or an invalid ID when out of bodies
    // BodyID                        CreateAndAddBody(const BodyCreationSettings &inSettings, EActivation ActivationMode);
    /// Combines CreateSoftBody and AddBody
    /// @return Created body ID or an invalid ID when out of bodies
    // BodyID                        CreateAndAddSoftBody(const SoftBodyCreationSettings &inSettings, EActivation ActivationMode);

    /// Add state handle, used to keep track of a batch of bodies while adding them to the PhysicsSystem.
    // using AddState = void *;

    ///@name Batch adding interface
    ///@{
    /// Prepare adding Number bodies at ioBodies to the PhysicsSystem, returns a handle that should be used in AddBodiesFinalize/Abort.
    /// This can be done on a background thread without influencing the PhysicsSystem.
    /// ioBodies may be shuffled around by this function and should be kept that way until AddBodiesFinalize/Abort is called.
    // AddState                    AddBodiesPrepare(BodyID *ioBodies, int Number);
    /// Finalize adding bodies to the PhysicsSystem, supply the return value of AddBodiesPrepare in inAddState.
    /// Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.
    // void                        AddBodiesFinalize(BodyID *ioBodies, int Number, AddState inAddState, EActivation ActivationMode);
    /// Abort adding bodies to the PhysicsSystem, supply the return value of AddBodiesPrepare in inAddState.
    /// This can be done on a background thread without influencing the PhysicsSystem.
    /// Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.
    // void                        AddBodiesAbort(BodyID *ioBodies, int Number, AddState inAddState);
    /// Remove Number bodies in ioBodies from the PhysicsSystem. Note that bodies need to be added to the physics system before they can be removed.
    /// ioBodies may be shuffled around by this function.
    // void                        RemoveBodies(BodyID *ioBodies, int Number);
    ///@}

    //# Activate / deactivate a body. Note that you need to add a body to the physics system before you can activate it.
    void ActivateBody(LPhysicsBodyId const& BodyId);
    void ActivateBodies(std::span<LPhysicsBodyId const*> BodyId);
    // void ActivateBodiesInAABox(AABox const& Box, BroadPhaseLayerFilter const& BroadPhaseLayerFilter, JPH::ObjectLayerFilter const& ObjectLayerFilter);
    void DeactivateBody(LPhysicsBodyId const& BodyId);
    void DeactivateBodies(std::span<LPhysicsBodyId const*> BodyIds);
    //# Checks if a body is currently actively simulating
    bool IsActive(LPhysicsBodyId const& BodyId) const;
    //# @see LPhysicsBody::ResetSleepTimer
    void ResetSleepTimer(LPhysicsBodyId const& BodyId);

    //# Create a two body constraint.
    // TwoBodyConstraint* CreateConstraint(const TwoBodyConstraintSettings *inSettings, LPhysicsBodyId const& BodyId1, LPhysicsBodyId const& BodyId2);
    //# Activate non-static bodies attached to a constraint.
    //# Note that the bodies involved in the constraint should be added to the physics system before activating a constraint.
    // void ActivateConstraint(const TwoBodyConstraint *inConstraint);

    //# Get the current shape.
    // RefConst<Shape> GetShape(LPhysicsBodyId const& BodyId) const;
    //# Set a new shape on the body
    //# @param BodyId Body ID of body that had its shape changed
    //# @param Shape The new shape
    //# @param UpdateMassProperties When true, the mass and inertia tensor is recalculated
    //# @param ActivationMode Whether or not to activate the body
    // void SetShape(LPhysicsBodyId const& BodyId, const Shape *Shape, bool UpdateMassProperties, EActivation ActivationMode) const;
    //# Notify all systems to indicate that a shape has changed (usable for MutableCompoundShapes)
    //# @param BodyId Body ID of body that had its shape changed
    //# @param inPreviousCenterOfMass Center of mass of the shape before the alterations
    //# @param UpdateMassProperties When true, the mass and inertia tensor is recalculated
    //# @param ActivationMode Whether or not to activate the body
    // void NotifyShapeChanged(LPhysicsBodyId const& BodyId, Vec3ArgreviousCenterOfMass, bool UpdateMassProperties, EActivation ActivationMode) const;

    //# Object layer of a body
    // void SetObjectLayer(LPhysicsBodyId const& BodyId, ObjectLayerayer); ///< The collision layer this body belongs to (determines if two objects can collide)
    // ObjectLayer GetObjectLayer(LPhysicsBodyId const& BodyId) const; //# @see LPhysicsBody::GetObjectLayer

    //# Position and rotation of a body.
    ENGINE_API void SetPositionAndRotation(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode);
    //# Will only update the position/rotation and activate the body when the difference is larger than a very
    //# small number. This avoids updating the broadphase/waking up a body when the resulting position/orientation doesn't really change.
    ENGINE_API void SetPositionAndRotationWhenChanged(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode);
    ENGINE_API void GetPositionAndRotation(LPhysicsBodyId const& BodyId, LWorldVec3& OutPosition, LWorldQuat& OutRotation) const;
    ENGINE_API void SetPosition(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, ESimulationAddingBehavior ActivationMode);
    ENGINE_API NODISCARD LWorldVec3 GetPosition(LPhysicsBodyId const& BodyId) const;
    //# @see LPhysicsBody::GetCenterOfMassPosition.
    ENGINE_API NODISCARD LWorldVec3 GetCenterOfMassPosition(LPhysicsBodyId const& BodyId) const;
    ENGINE_API void SetRotation(LPhysicsBodyId const& BodyId, LWorldQuat const& Rotation, ESimulationAddingBehavior ActivationMode);
    ENGINE_API NODISCARD LWorldQuat GetRotation(LPhysicsBodyId const& BodyId) const;
    //# @see LPhysicsBody::GetWorldTransform.
    ENGINE_API NODISCARD LWorldMat4 GetWorldTransform(LPhysicsBodyId const& BodyId) const;
    //# @see LPhysicsBody::GetCenterOfMassTransform.
    ENGINE_API NODISCARD LWorldMat4 GetCenterOfMassTransform(LPhysicsBodyId const& BodyId) const;

    //# Set velocity of body such that it will be positioned at Target-Position/Rotation
    //# in DeltaTime seconds (will activate body if needed).
    ENGINE_API void MoveKinematic(LPhysicsBodyId const& BodyId, LWorldVec3 const& TargetPosition, LWorldQuat const& TargetRotation, f32 DeltaTime);

    //# Linear or angular velocity (functions will activate body if needed).
    //# Note that the linear velocity is the velocity of the center of mass, which may not coincide with the position
    //# of your object, to correct for this: \f$VelocityCOM = Velocity - AngularVelocity \times ShapeCOM\f$.
    ENGINE_API void SetLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity);
    ENGINE_API void GetLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F &OutLinearVelocity, LVec3F &OutAngularVelocity) const;
    //# @see LPhysicsBody::SetLinearVelocity.
    ENGINE_API void SetLinearVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity);
    //# @see LPhysicsBody::GetLinearVelocity.
    ENGINE_API NODISCARD LVec3F GetLinearVelocity(LPhysicsBodyId const& BodyId) const;
    //# Add velocity to current velocity.
    ENGINE_API void AddLinearVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity);
    //# Add linear and angular to current velocities.
    ENGINE_API void AddLinearAndAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity);
    //# @see LPhysicsBody::SetAngularVelocity
    ENGINE_API void SetAngularVelocity(LPhysicsBodyId const& BodyId, LVec3F const& AngularVelocity);
    //# @see LPhysicsBody::GetAngularVelocity.
    ENGINE_API NODISCARD LVec3F GetAngularVelocity(LPhysicsBodyId const& BodyId) const;
    //# Velocity of point #Point (in world space, e.g. on the surface of the body) of the body. @see Body::GetPointVelocity.
    ENGINE_API NODISCARD LVec3F GetPointVelocity(LPhysicsBodyId const& BodyId, LWorldVec3 const& Point) const;

    //# Set the complete motion state of a body.
    //# Note that the linear velocity is the velocity of the center of mass, which may not coincide with the position
    //# of your object, to correct for this: \f$VelocityCOM = Velocity - AngularVelocity \times ShapeCOM\f$.
    ENGINE_API void SetPositionRotationAndVelocity(LPhysicsBodyId const& BodyId, LWorldVec3 const& Position, LWorldQuat Rotation, LVec3F const& LinearVelocity, LVec3F const& AngularVelocity);

    //# Add forces to the body. Note that you should add a body to the physics system before applying forces or torques.
    //# @see LPhysicsBody::AddForce.
    ENGINE_API void AddForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);
    //# Some as #AddForce but in local space. @see LPhysicsBody::AddTorque.
    ENGINE_API void AddLocalForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);
    //# Applied at world space position #Point. @see LPhysicsBody::AddForce.
    ENGINE_API void AddForce(LPhysicsBodyId const& BodyId, LVec3F const& Force, LWorldVec3 const& Point, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);
    //# @see LPhysicsBody::AddTorque.
    ENGINE_API void AddTorque(LPhysicsBodyId const& BodyId, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);
    //# Some as #AddTorque but in local space. @see LPhysicsBody::AddTorque.
    ENGINE_API void AddLocalTorque(LPhysicsBodyId const& BodyId, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);
    //# A combination of Body::AddForce and Body::AddTorque.
    ENGINE_API void AddForceAndTorque(LPhysicsBodyId const& BodyId, LVec3F const& Force, LVec3F const& Torque, ESimulationAddingBehavior ActivationMode = ESimulationAddingBehavior::Activate);

    //# Add an impulse to the body. Note that you should add a body to the physics system before applying impulses.
    //# Applied at center of mass. @see Body::AddImpulse.
    ENGINE_API void AddImpulse(LPhysicsBodyId const& BodyId, LVec3F const& Impulse);
    //# Applied at world space position #Point. @see Body::AddImpulse.
    ENGINE_API void AddImpulse(LPhysicsBodyId const& BodyId, LVec3F const& Impulse, LWorldVec3 const& Point);
    //# @see LPhysicsBody::AddAngularImpulse.
    ENGINE_API void AddAngularImpulse(LPhysicsBodyId const& BodyId, LVec3F const& AngularImpulse);
    //# @see LPhysicsBody::ApplyBuoyancyImpulse.
    ENGINE_API NODISCARD bool ApplyBuoyancyImpulse(
          LPhysicsBodyId const& BodyId
        , LWorldVec3 const& SurfacePosition
        , LVec3F const& SurfaceNormal
        , f32 Buoyancy
        , f32 LinearDrag
        , f32 AngularDrag
        , LVec3F const& FluidVelocity
        , LVec3F const& Gravity
        , f32 DeltaTime
        );

    //# @see LPhysicsBody::GetBodyType.
    ENGINE_API NODISCARD EBodyType GetBodyType(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetMotionType.
    ENGINE_API void SetMotionType(LPhysicsBodyId const& BodyId, EPhysicsMotion MotionType, ESimulationAddingBehavior ActivationMode);
    //# @see LPhysicsBody::GetMotionType.
    ENGINE_API NODISCARD EPhysicsMotion GetMotionType(LPhysicsBodyId const& BodyId) const;

    //# How well it detects collisions when it has a high velocity.
    ENGINE_API void SetMotionQuality(LPhysicsBodyId const& BodyId, EPhysicsMotionQuality Quality);
    //# @see MotionProperties::GetMotionQuality.
    ENGINE_API NODISCARD EPhysicsMotionQuality GetMotionQuality(LPhysicsBodyId const& BodyId) const;

    //# Get inverse inertia tensor in world space.
    //# @see LPhysicsBody::GetInverseInertia.
    ENGINE_API NODISCARD LMat4F GetInverseInertia(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetRestitution.
    ENGINE_API void SetRestitution(LPhysicsBodyId const& BodyId, f32 Restitution);
    //# @see LPhysicsBody::GetRestitution.
    ENGINE_API NODISCARD f32 GetRestitution(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetFriction.
    ENGINE_API void SetFriction(LPhysicsBodyId const& BodyId, f32 Friction);
    //# @see LPhysicsBody::GetFriction.
    ENGINE_API NODISCARD f32 GetFriction(LPhysicsBodyId const& BodyId) const;

    //# @see MotionProperties::SetGravityFactor.
    ENGINE_API void SetGravityFactor(LPhysicsBodyId const& BodyId, f32 GravityFactor);
    //# @see MotionProperties::GetGravityFactor.
    ENGINE_API NODISCARD f32 GetGravityFactor(LPhysicsBodyId const& BodyId) const;

    //# @see MotionProperties::SetMaxLinearVelocity.
    ENGINE_API void SetMaxLinearVelocity(LPhysicsBodyId const& BodyId, f32 LinearVelocity);
    //# @see MotionProperties::GetMaxLinearVelocity.
    ENGINE_API NODISCARD f32 GetMaxLinearVelocity(LPhysicsBodyId const& BodyId) const;

    //# @see MotionProperties::SetMaxAngularVelocity.
    ENGINE_API void SetMaxAngularVelocity(LPhysicsBodyId const& BodyId, f32 AngularVelocity);
    //# @see MotionProperties::GetMaxAngularVelocity.
    ENGINE_API NODISCARD f32 GetMaxAngularVelocity(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetUseManifoldReduction.
    ENGINE_API void SetUseManifoldReduction(LPhysicsBodyId const& BodyId, bool bReduction);
    //# @see LPhysicsBody::GetUseManifoldReduction.
    ENGINE_API NODISCARD bool GetUseManifoldReduction(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetIsSensor.
    ENGINE_API void SetIsSensor(LPhysicsBodyId const& BodyId, bool bSensor);
    //# @see LPhysicsBody::IsSensor.
    ENGINE_API NODISCARD bool IsSensor(LPhysicsBodyId const& BodyId) const;

    //# @see LPhysicsBody::SetCollisionGroup.
    // void SetCollisionGroup(LPhysicsBodyId const& BodyId, const CollisionGroup &inCollisionGroup);
    //# @see LPhysicsBody::GetCollisionGroup.
    // CollisionGroup const& GetCollisionGroup(LPhysicsBodyId const& BodyId) const;

    //# Get transform and shape for this body, used to perform collision detection.
    // TransformedShape GetTransformedShape(LPhysicsBodyId const& BodyId) const;

    //# Get the user data for a body.
    ENGINE_API NODISCARD u64 GetUserData(LPhysicsBodyId const& BodyId) const;
    ENGINE_API void SetUserData(LPhysicsBodyId const& BodyId, u64 UserData) const;

    //# Get the material for a particular sub shape
    // PhysicsMaterial const* GetMaterial(LPhysicsBodyId const& BodyId, const SubShapeID &inSubShapeID) const;

    //# Set the Body::EFlags::InvalidateContactCache flag for the specified body. This means that the
    //# collision cache is invalid for any body pair involving that body until the next physics step.
    ENGINE_API void InvalidateContactCache(LPhysicsBodyId const& BodyId);

private:

    JPH::BodyInterface& Interface;
};

struct LPhysicsSystemCreateInfo final
{
    std::size_t RigidBodyLimit;
    std::optional<std::size_t> BodyMutexNumber;
    std::size_t BodyPairLimit;
    std::size_t ContactConstraintLimit;

    std::size_t TemporalUpdateStackSizeLimit;

    std::size_t JobLimit;
    std::size_t BarrierLimit;
    std::size_t ThreadLimit;

    f64 PhysicsStep;
    f64 AccumulatorLimit;
    std::size_t PhysicsSteps;

    LVec3F Gravity;
};

class LPhysicsSystem
{
public:

    LPhysicsSystem() = delete;
    ENGINE_API explicit LPhysicsSystem(ESkipInit) noexcept: SubSystem{} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LPhysicsSystem)
    explicit LPhysicsSystem(LPhysicsSystemCreateInfo Info): LPhysicsSystem{ESkipInit::Here} { *this = std::move(Info); }
    ENGINE_API LPhysicsSystem& operator=(LPhysicsSystemCreateInfo Info);
    ENGINE_API virtual ~LPhysicsSystem();

    //# If this returns false. All interactions with this class are UB.
    NODISCARD constexpr bool IsValid() const noexcept { return !!this->SubSystem; }

    NODISCARD ENGINE_API JPH::PhysicsSystem* operator->() noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem const* operator->() const noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem& operator*() noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem const& operator*() const noexcept;

    void Advance(f64 Dt) noexcept
    {
        this->Accumulator += Dt;
        if (this->Accumulator >= this->AccumulatorLimit)
        {
            LOG_WARNING(LogPhysics, "Physics accumulator is too high: {} >= {}."
                , this->Accumulator, this->AccumulatorLimit)
            this->Accumulator = this->AccumulatorLimit;
        }
    }

    //# @return True, if physics were updated at least once.
    ENGINE_API bool TryUpdate(TFunction2<void(f32 PhysicsStep)> Callback);
    ENGINE_API void Update(std::size_t Steps, TFunction2<void(f32 PhysicsStep)> Callback);
    NODISCARD constexpr bool _check_IsUpdating() const noexcept { return this->_check_bUpdating.load(); }

    NODISCARD ENGINE_API LPhysicsSystemInterface GetBodyInterfaceNoLock() noexcept;

private:

#if JAFG_DO_CHECKS
    std::atomic_bool _check_bUpdating;
#endif /* JAFG_DO_CHECKS */

    f64 PhysicsStep;
    f64 Accumulator;
    f64 AccumulatorLimit;
    std::size_t PhysicsSteps;

    Detail::LPhysicsSubsystem* SubSystem;
};

} /* ~Namespace Jafg */
