// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Jgc/Framework/JgcGimbalFreeLook.h"
#include "JgcVehicleInputComponent.generated.h"

namespace Jafg
{

struct LInputActionValue;
class AGimbalComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

class AVehicleComponent;
class WStatusHud;

//#
//# Translates the input of a local human into the controls of the vehicle of the owning actor. Part of the vehicle,
//# next to its #AVehicleComponent. Active while a local controller possesses the vehicle (see
//# #AVehicleComponent::Enter).
//#
//# Free look: While the free look action is held, the gimbal above the eye of the vehicle can be changed (see
//# #Jafg::AGimbalComponent and #LGimbalFreeLook). The view stays wherever it was moved to.
//#   - Mouse: Turns the view around the vehicle.
//#   - Pan held and mouse: Moves the center that the view turns around (e.g., to have the vehicle on the left).
//#   - Mouse wheel: Changes the distance to the vehicle.
//# Holding the free look action without changing anything centers the view again.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcVehicleInputComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcVehicleInputComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;
    virtual void OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New) override;
    virtual void ParentTick(f32 Dt) override;

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    //# The vehicle of the owning actor.
    NODISCARD AVehicleComponent* GetVehicle() const noexcept;

    void OnVehicleStrafe(Jafg::LInputActionValue const& Value);
    void OnVehicleTorque(Jafg::LInputActionValue const& Value);
    //# @return Whether the vehicle was left. Leaving gives the controls back to the driver.
    bool OnVehicleExit();
    void ToggleDampeners() noexcept;
    void OnFreeLookHeld() noexcept { this->FreeLook.OnHeld(); }
    void OnFreeLookPanHeld() noexcept { this->FreeLook.OnPanHeld(); }
    void OnVehicleLook(Jafg::LInputActionValue const& Value) noexcept;
    //# Positive zooms in (closer to the vehicle).
    void OnVehicleZoom(Jafg::LInputActionValue const& Value) noexcept;

    //# The gimbal that free look turns: The first one above the eye of the vehicle.
    NODISCARD Jafg::AGimbalComponent* GetFreeLookGimbal() const noexcept;
    NODISCARD FORCEINLINE constexpr bool IsFreeLooking() const noexcept { return this->FreeLook.IsActive(); }

    //# Radians per pixel.
    CLASS_FIELD(EditorEditable)
    f32 FreeLookSensitivity{ 0.003f };
    CLASS_FIELD(EditorEditable)
    bool bInvertFreeLookPitch{};
    //# Less mouse movement than this (in pixels) during a free look counts as not moved. Then the view is centered.
    CLASS_FIELD(EditorEditable)
    f32 FreeLookRecenterThreshold{ 3.0f };
    //# Meters per pixel at a zoom of one. Scales with the zoom.
    CLASS_FIELD(EditorEditable)
    f32 FreeLookPanSensitivity{ 0.03f };
    //# The factor of the distance per notch of the mouse wheel.
    CLASS_FIELD(EditorEditable)
    f32 FreeLookZoomStep{ 1.15f };

private:

    void UpdateHud() noexcept;

    //# Shows the speed and the dampeners of the vehicle. Lives in a viewport, not in the world (see #WStatusHud).
    WStatusHud* Hud{};
    std::size_t HudSpeedRow{};
    std::size_t HudDampenersRow{};

    LGimbalFreeLook FreeLook;
};

} /* ~Namespace Jgc */
