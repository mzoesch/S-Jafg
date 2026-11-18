// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PawnComponent.h"
#include "JgcInputComponent.generated.h"

namespace Jgc
{

class AVehicleComponent;

//#
//# The core input component for jgc.
//# TODO: Deprecated. Make new class for vehicles only!
//#
//# Idle until a vehicle is possessed (see #AVehicleComponent::Enter). Then it owns the input of the local human.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcInputComponent : public Jafg::APawnComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcInputComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnAttach(Jafg::AActor& InOwner) override
    {
        Super::OnAttach(InOwner);
        if (this->IsVehicleValid())
        {
            (void)this->ActivateUserInputContext();
        }
    }
    virtual void OnNewPersonaController(Jafg::APersonaController* New) override
    {
        Super::OnNewPersonaController(New);
        if (this->IsVehicleValid())
        {
            (void)this->ActivateUserInputContext();
        }
    }

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    virtual void ParentTick(f32 Dt) override;

    NODISCARD FORCEINLINE constexpr bool IsVehicleValid() const noexcept { return this->Vehicle != nullptr; }
    NODISCARD FORCEINLINE constexpr AVehicleComponent* GetVehicle() const noexcept { return this->Vehicle; }
    NODISCARD FORCEINLINE constexpr AVehicleComponent& GetVehicleChecked() const noexcept { check(this->IsVehicleValid()) return *this->Vehicle; }
    //#
    //# Take over the controls of a vehicle, or give them up with nullptr. Use #AVehicleComponent::Enter and
    //# #AVehicleComponent::Exit instead, they call this.
    //# @return The previously possessed vehicle.
    //#
    AVehicleComponent* PossessVehicle(AVehicleComponent* VehicleComponent) noexcept;

    void OnVehicleStrafe(Jafg::LInputActionValue const& Value);
    void OnVehicleTorque(Jafg::LInputActionValue const& Value);
    //# @return Whether a vehicle was left.
    bool OnVehicleExit();

private:

    void ForcePawnToVehicle(AVehicleComponent const& VehicleComponent) noexcept;

    AVehicleComponent* Vehicle{};
};

} /* ~Namespace Jgc */
