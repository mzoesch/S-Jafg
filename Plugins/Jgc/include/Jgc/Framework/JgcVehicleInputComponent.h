// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "JgcVehicleInputComponent.generated.h"

namespace Jafg
{

struct LInputActionValue;

} /* ~Namespace Jafg */

namespace Jgc
{

class AVehicleComponent;

//#
//# Translates the input of a local human into the controls of the vehicle of the owning actor. Part of the vehicle,
//# next to its #AVehicleComponent. Active while a local controller possesses the vehicle (see
//# #AVehicleComponent::Enter).
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcVehicleInputComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AJgcVehicleInputComponent)

public:

    virtual void OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New) override;

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    //# The vehicle of the owning actor.
    NODISCARD AVehicleComponent* GetVehicle() const noexcept;

    void OnVehicleStrafe(Jafg::LInputActionValue const& Value);
    void OnVehicleTorque(Jafg::LInputActionValue const& Value);
    //# @return Whether the vehicle was left. Leaving gives the controls back to the driver.
    bool OnVehicleExit();
};

} /* ~Namespace Jgc */
