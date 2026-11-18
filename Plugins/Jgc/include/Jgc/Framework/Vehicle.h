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
class AAvatarComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

enum struct EVehicle
{
    Identity,
    Dynamic3D,
};

//#
//# A vehicle that one driver can drive. Interacting with it seats the instigator as the driver.
//#
//# The actor of a vehicle is an avatar of its own: It needs an #Jafg::AAvatarComponent, and usually an
//# #Jafg::AEyeComponent (the view while driving) and an #AJgcVehicleInputComponent (the controls). While seated,
//# the actor of the driver is attached to the vehicle and the controller of the driver possesses the vehicle.
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

    virtual bool CanInteract(Jafg::AAvatarComponent const& Instigator) const override;
    virtual bool Interact(Jafg::AAvatarComponent& Instigator) override;
    virtual LString GetInteractionText() const override;

    NODISCARD FORCEINLINE bool HasDriver() const noexcept { return !!this->GetDriver(); }
    //# The avatar of the driver. It is not possessed while driving; its controller possesses this vehicle.
    NODISCARD Jafg::AAvatarComponent* GetDriver() const noexcept;
    //#
    //# Seat an avatar as the driver: Its movement is frozen, its actor is attached to this vehicle (where it is),
    //# and its controller possesses the avatar of this vehicle instead.
    //# @return False if the seat is taken or this vehicle has no avatar.
    //#
    bool Enter(Jafg::AAvatarComponent& NewDriver);
    //#
    //# Let the driver leave. It is detached where it sits (that is where it entered, relative to this vehicle),
    //# keeps the velocity of this vehicle, and its controller possesses it again.
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

    //# Inertial dampeners brake the vehicle while it is not thrusting or turning.
    NODISCARD FORCEINLINE constexpr bool AreDampenersEngaged() const noexcept { return this->bDampeners; }
    FORCEINLINE constexpr void SetDampenersEngaged(bool bEngaged) noexcept { this->bDampeners = bEngaged; }
    //# In meters per second.
    NODISCARD f32 GetSpeed() const noexcept;

private:

    EVehicle Vehicle{ EVehicle::Identity };
    std::size_t bLastUserThrust{};
    std::size_t bLastUserTorque{};

    bool bDampeners{ true };
    Jafg::AAvatarComponent* Driver{};
    //# The avatar of this vehicle while driven. Kept, because the actor no longer finds it while it is torn down.
    Jafg::AAvatarComponent* DrivenAvatar{};
};

} /* ~Namespace Jgc */
