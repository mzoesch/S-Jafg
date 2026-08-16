// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Framework/RigidObject.h"
#include "RigidComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API ARigidComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ARigidComponent)

public:

#if JAFG_DO_CHECKS
    virtual void OnAttach(AActor& InOwner) override;
#endif /* JAFG_DO_CHECKS */

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    void SetPhysicsLayer(EPhysicsLayer::value_type Layer);
    NODISCARD FORCEINLINE constexpr EPhysicsLayer::value_type GetPhysicsLayer() const noexcept { return this->Layer; }
    void SetPhysicsMotion(EPhysicsMotion Motion);
    NODISCARD FORCEINLINE constexpr EPhysicsMotion GetPhysicsMotion() const noexcept { return this->Motion; }

    void MakeSphere(f32 Radius);
    void MakeBox(LWorldVec3 HalfExtent, f32 ConvexRadius = Physx::DefaultConvexRadius);

    bool IsAddedToSimulation() const noexcept;
    void AddToSimulation(ESimulationAddingBehavior Behavior);
    void RemoveFromSimulation();

    void SetLinearVelocity(LWorldVec3 const& Velocity);

    void SetRigidObject(LRigidObject RigidObject) noexcept { this->RigidObject = std::move(RigidObject); }
    NODISCARD FORCEINLINE constexpr LRigidObject GetRigidObject() const noexcept { return this->RigidObject; }

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) override;

private:

    void UpdateComponentAabb() noexcept;
    void AddToWorldRigidComponents() noexcept;
    void RemoveFromWorldRigidComponents() noexcept;

    EPhysicsLayer::value_type Layer{ EPhysicsLayer::Static };
    EPhysicsMotion Motion{ EPhysicsMotion::Static };

    LRigidObject RigidObject;
    void DestroyRigidObject() noexcept;
    void DestroyRigidObjectImpl() noexcept;
};

} /* ~Namespace Jafg */
