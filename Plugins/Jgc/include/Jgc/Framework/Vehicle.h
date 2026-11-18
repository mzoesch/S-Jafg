// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Framework/Actor.h"
#include "Framework/RigidComponent.h"
#include "Jgc/Framework/InteractableComponent.h"
#include "Vehicle.generated.h"

namespace Jafg
{

class AStaticMeshComponent;
class APawn;

} /* ~Namespace Jafg */

namespace Jgc
{

enum struct EVehicle
{
    Identity,
    Dynamic3D,
};

//#
//# A vehicle that one pawn can drive. Interacting with it seats the pawn as the driver.
//#
DECLARE_JAFG_CLASS()
class JGC_API AVehicleComponent : public AInteractableComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AVehicleComponent)
    {
        this->bPhysicsTick = true;
    }

public:

    virtual void OnAttach(Jafg::AActor& Owner) override;
    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;
    virtual void ParentPhysicsTick(f32 Dt, bool bFollowup) override;

    virtual bool CanInteract(Jafg::APawn const& Instigator) const override;
    virtual bool Interact(Jafg::APawn& Instigator) override;
    virtual LString GetInteractionText() const override;

    NODISCARD FORCEINLINE bool HasDriver() const noexcept { return !!this->GetDriver(); }
    NODISCARD Jafg::APawn* GetDriver() const noexcept;
    //#
    //# Seat a pawn as the driver. The movement of the pawn is frozen and a local human gets the controls of this
    //# vehicle (see #AJgcInputComponent).
    //# @return False if the seat is already taken.
    //#
    bool Enter(Jafg::APawn& Pawn);
    //#
    //# Let the driver leave. The driver is put back where it entered (relative to this vehicle) and keeps the
    //# velocity of this vehicle.
    //#
    void Exit();

    NODISCARD FORCEINLINE constexpr EVehicle GetVehicleType() const noexcept { return this->Vehicle; }
    //# Can only be set if this component has not yet been initialized.
    FORCEINLINE constexpr void SetVehicleType(EVehicle Type) noexcept
    {
        check(!this->IsOwningActorValid())
        check(this->Vehicle == EVehicle::Identity)
        this->Vehicle = Type;
    }

    NODISCARD Jafg::AStaticMeshComponent const& GetVehicleMesh() const;
    NODISCARD Jafg::AStaticMeshComponent& GetMutableVehicleMesh();

    //# The rigid component that this vehicle manipulates.
    NODISCARD Jafg::ARigidComponent& GetRigidComponent() noexcept
    {
        return this->GetOwningActor().GetRootComponent<Jafg::ARigidComponent>();
    }
    NODISCARD Jafg::ARigidComponent const& GetRigidComponent() const noexcept
    {
        return this->GetOwningActor().GetRootComponent<Jafg::ARigidComponent>();
    }

    void AddThrust(LVec3F const& Thrust, bool bUserInput);
    void AddTorque(LVec3F const& Torque, bool bUserInput);

private:

    EVehicle Vehicle{ EVehicle::Identity };
    std::size_t bLastUserThrust{};
    std::size_t bLastUserTorque{};

    Jafg::APawn* Driver{};
    //# Where the driver entered. Relative to the root of this vehicle.
    LWorldVec3 DriverEntryLocation{ maths::zero_vector<LWorldVec3> };
    LWorldQuat DriverEntryRotation{ maths::identity<LWorldQuat> };
};

} /* ~Namespace Jgc */
