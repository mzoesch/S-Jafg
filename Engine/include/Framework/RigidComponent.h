// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Framework/PhysicsBody.h"
#include "RigidComponent.generated.h"

namespace Jafg
{

struct LStaticMesh;
class AStaticMeshComponent;

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LPhysicsBody>(TEditorNodeCreateInfo<LPhysicsBody> const& Info) noexcept;

SERDE_DECLARE_TRIVIAL_ENUM(ERigidComponentConvexWalker, u8,
    //#
    //# Find all children and make a convex hull from their meshes, recursively.
    //# Will not walk non-static meshes.
    //#
    DirectRecursive,
    //# Find all direct children and make a convex hull from their meshes.
    Direct,
    //# Find the first mesh and make a convex hull from it.
    First
    )

DECLARE_JAFG_CLASS()
class ENGINE_API ARigidComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ARigidComponent)
    {
        this->SetShouldRender(true);
    }

public:

#if JAFG_DO_CHECKS
    virtual void OnAttach(AActor& InOwner) override;
#endif /* JAFG_DO_CHECKS */

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    void SetPhysicsLayer(EPhysicsLayer Layer);
    NODISCARD FORCEINLINE constexpr EPhysicsLayer GetPhysicsLayer() const noexcept { return this->Layer; }
    void SetPhysicsMotion(EPhysicsMotion Motion);
    NODISCARD FORCEINLINE constexpr EPhysicsMotion GetPhysicsMotion() const noexcept { return this->Motion; }

    void MakeSphere(f32 Radius);
    void MakeBox(LWorldVec3 HalfExtent, f32 ConvexRadius = Physx::DefaultConvexRadius);
    void MakeConvex(LStaticMesh& Mesh, f32 ConvexRadius = Physx::DefaultConvexRadius);
    void MakeConvex(ERigidComponentConvexWalker Walker = ERigidComponentConvexWalker::DirectRecursive, f32 ConvexRadius = Physx::DefaultConvexRadius);
    //#
    //# Makes one convex hull around the meshes of all given components. Their current transforms relative to this
    //# component are respected. They do not have to be children of this component (but usually are).
    //#
    void MakeConvex(std::span<AStaticMeshComponent* const> Meshes, f32 ConvexRadius = Physx::DefaultConvexRadius);

    bool IsAddedToSimulation() const noexcept;
    //# The simulation moves the body in world space. Attached actors cannot be simulated.
    void AddToSimulation(ESimulationAddingBehavior Behavior);
    virtual bool CanFollowAttachParent() const noexcept override { return !this->IsAddedToSimulation(); }
    void RemoveFromSimulation();

    void SetLinearVelocity(LWorldVec3 const& Velocity);

    //#
    //# The shape of a rigid component usually only approximates the meshes attached to it (e.g., a convex hull).
    //# They are traced on their own. Therefore, only a rigid component without any static mesh in its hierarchy (an
    //# invisible collider) is traced; against its shape.
    //#
    NODISCARD virtual std::optional<LWorldReal> TraceNonPhysical(LWorldMagRay3 const& Ray) const override;

    void SetRigidObject(LPhysicsBody RigidObject) noexcept { this->RigidObject = std::move(RigidObject); }
    NODISCARD FORCEINLINE constexpr LPhysicsBody GetRigidObject() const noexcept { return this->RigidObject; }

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) override;

private:

    void UpdateComponentAabb() noexcept;
    void AddToWorldRigidComponents() noexcept;
    void RemoveFromWorldRigidComponents() noexcept;

    void DestroyRigidObject() noexcept;

    //#
    //# (Re)creates the rigid object as a convex hull around the given points.
    //# @param Hull Points relative to the world translation and rotation of this component, but already scaled
    //#             by its world scale.
    //#
    void MakeConvexFromPoints(TArray<LVec3F> const& Hull, f32 ConvexRadius);

    CLASS_FIELD(EditorVisible)
    EPhysicsLayer Layer{ EPhysicsLayer::Static };
    CLASS_FIELD(EditorVisible)
    EPhysicsMotion Motion{ EPhysicsMotion::Static };

    CLASS_FIELD(EditorEditable)
    LPhysicsBody RigidObject;

public:

    //# Public private function. Do not use.
    void _DestroyRigidObjectImpl() noexcept;
};

} /* ~Namespace Jafg */
