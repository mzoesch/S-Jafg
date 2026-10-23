// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PawnComponent.h"
#include "JgcInputComponent.generated.h"

namespace Jgc
{

class AVehicleComponent;

//#
//# The core input component for jgc.
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
        (void)this->ActivateUserInputContext();
    }
    virtual void OnNewPersonaController(Jafg::APersonaController* New) override
    {
        Super::OnNewPersonaController(New);
        (void)this->ActivateUserInputContext();
    }

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    virtual void ParentTick(f32 Dt) override;

    NODISCARD FORCEINLINE constexpr bool IsVehicleValid() const noexcept { return this->Vehicle != nullptr; }
    NODISCARD FORCEINLINE constexpr AVehicleComponent* GetVehicle() const noexcept { return this->Vehicle; }
    NODISCARD FORCEINLINE constexpr AVehicleComponent& GetVehicleChecked() const noexcept { check(this->IsVehicleValid()) return *this->Vehicle; }
    AVehicleComponent* PossessVehicle(AVehicleComponent* VehicleComponent) noexcept;

    void OnVehicleStrafe(Jafg::LInputActionValue const& Value);
    void OnVehicleTorque(Jafg::LInputActionValue const& Value);

private:

    void ForcePawnToVehicle(AVehicleComponent const& VehicleComponent) noexcept;

    AVehicleComponent* Vehicle{};
};

} /* ~Namespace Jgc */
