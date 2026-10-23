// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Framework/Actor.h"
#include "Framework/RigidComponent.h"
#include "Vehicle.generated.h"

namespace Jgc
{

enum struct EVehicle
{
    Identity,
    Dynamic3D,
};

DECLARE_JAFG_CLASS()
class JGC_API AVehicleComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AVehicleComponent)
    {
        this->bPhysicsTick = true;
    }

public:

    virtual void OnAttach(Jafg::AActor& Owner) override;
    virtual void ParentPhysicsTick(f32 Dt, bool bFollowup) override;

    NODISCARD FORCEINLINE constexpr EVehicle GetVehicleType() const noexcept { return this->Vehicle; }
    //# Can only be set if this component has not yet been initialized.
    FORCEINLINE constexpr void SetVehicleType(EVehicle Type) noexcept
    {
        check(!this->IsOwningActorValid())
        check(this->Vehicle == EVehicle::Identity)
        this->Vehicle = Type;
    }

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
};

} /* ~Namespace Jgc */
