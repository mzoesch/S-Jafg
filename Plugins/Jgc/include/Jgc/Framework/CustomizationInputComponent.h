// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Nodes/UserWidget.h"
#include "Jgc/Framework/GimbalFreeLook.h"
#include "CustomizationInputComponent.generated.h"

namespace Jafg
{

struct LInputActionValue;
class LLocalLackey;
class AGimbalComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

class ACustomizationStationComponent;

//#
//# The controls of a customization station for a local human. Part of the station, next to its
//# #ACustomizationStationComponent. Active while a local controller possesses the station (see
//# #ACustomizationStationComponent::Enter).
//#
//# The occupant is shown in the right third of the view (see #OccupantViewX), and a panel in the space left of it,
//# from the top to the bottom of the view, changes its shape (see #OpenCustomizationPanel). The panel applies or
//# discards the changes, which also leaves the station. The mouse cursor is always shown. Over the world:
//#   - Right mouse button held and the cursor dragged: Orbits the view around the occupant (the gimbal above the eye
//#     of the station, see #LGimbalFreeLook).
//#   - Mouse wheel: Changes the distance to the occupant.
//#
DECLARE_JAFG_CLASS()
class JGC_API ACustomizationInputComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ACustomizationInputComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;
    virtual void OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New) override;
    virtual void ParentTick(f32 Dt) override;

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    //# The station of the owning actor.
    NODISCARD ACustomizationStationComponent* GetStation() const noexcept;
    //# The gimbal that orbiting turns: The first one above the eye of the station.
    NODISCARD Jafg::AGimbalComponent* GetOrbitGimbal() const noexcept;

    //# Starts orbiting only if the cursor is not over the panel. The motion of the cursor turns the view.
    void OnOrbitHeld() noexcept;
    //# Positive zooms in (closer to the occupant). Not over the panel.
    void OnOrbitZoom(Jafg::LInputActionValue const& Value) noexcept;

    //# Keeps the changes to the shape and leaves the station.
    void Apply();
    //# Takes the changes back and leaves the station.
    void Discard();

    NODISCARD bool IsPanelOpen() const noexcept;

    //# Radians per pixel that the cursor is dragged.
    CLASS_FIELD(EditorEditable)
    f32 OrbitSensitivity{ 0.005f };
    //# Moving the mouse up turns the view up (the eye orbits below the occupant), unless inverted.
    CLASS_FIELD(EditorEditable)
    bool bInvertOrbitPitch{};
    //# The factor of the distance per notch of the mouse wheel.
    CLASS_FIELD(EditorEditable)
    f32 OrbitZoomStep{ 1.1f };
    //#
    //# Where the occupant is shown across the view, from its left (0) to its right (1). Not less than a half. The
    //# panel takes the space left of the band around it (e.g., the left two thirds for the center of the right
    //# third). The eye stays in front of the occupant and is turned aside instead, wherever it orbits to.
    //#
    CLASS_FIELD(EditorEditable)
    f32 OccupantViewX{ 5.0f / 6.0f };

private:

    //# Turns the eye of the station aside, so that the occupant (at the pivot of the orbit) is at #OccupantViewX.
    void AimEye() noexcept;
    void OpenPanel(Jafg::LLocalLackey& Lackey);
    void ClosePanel() noexcept;
    NODISCARD bool IsCursorOverPanel() const noexcept;
    //# Where the shown cursor is on the surface of the controller. Empty if it is not over it.
    NODISCARD std::optional<LVec2F> GetCursorLocation() const noexcept;

    LGimbalFreeLook Orbit;
    //# Of the previous frame. The motion of the cursor between frames orbits.
    std::optional<LVec2F> LastCursorLocation;
    //# Lives in the world viewport of the lackey, which may be destroyed before this component.
    TClassStorage<Jafg::WUserWidget> Panel;
};

} /* ~Namespace Jgc */
