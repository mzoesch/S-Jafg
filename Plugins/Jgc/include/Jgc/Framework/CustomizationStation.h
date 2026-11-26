// Copyright mzoesch. All rights reserved.

#pragma once

#include "Jgc/Framework/InteractableComponent.h"
#include "Jgc/Framework/HumanoidShapeComponent.h"
#include "CustomizationStation.generated.h"

namespace Jafg
{

class AActor;
class LWorld;
class AAvatarComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

//#
//# A place where a humanoid changes the shape of its body (#AHumanoidShapeComponent). Interacting with it puts the
//# instigator into its center, facing forward, and its controller possesses the avatar of the station instead. The
//# view comes from the front (an eye of the station), and the shape is changed with a window
//# (#ACustomizationInputComponent).
//#
//# Changes are shown at once. They are kept if they are applied, and taken back if they are discarded or the
//# occupant leaves in another way (e.g., the station is destroyed).
//#
//# The actor of a station is an avatar of its own: It needs an #Jafg::AAvatarComponent, an #Jafg::AEyeComponent and
//# an #ACustomizationInputComponent. @see #SpawnCustomizationStation
//#
DECLARE_JAFG_CLASS()
class JGC_API ACustomizationStationComponent : public AInteractableComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ACustomizationStationComponent)

public:

    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;

    virtual bool CanInteract(Jafg::AAvatarComponent const& Instigator) const override;
    virtual bool Interact(Jafg::AAvatarComponent& Instigator) override;
    virtual LString GetInteractionText() const override;

    NODISCARD FORCEINLINE bool HasOccupant() const noexcept { return !!this->GetOccupant(); }
    //# The avatar that is customized. It is not possessed meanwhile; its controller possesses this station.
    NODISCARD Jafg::AAvatarComponent* GetOccupant() const noexcept;
    //# The shape of the occupant. Null without an occupant.
    NODISCARD AHumanoidShapeComponent* GetOccupantShape() const noexcept;
    //# The shape of the occupant when it entered. Empty without an occupant.
    NODISCARD FORCEINLINE std::optional<LHumanoidShape> const& GetOriginalShape() const noexcept { return this->OriginalShape; }

    //#
    //# Puts an avatar into the station: It stands in the center, faces forward, and its controller possesses the
    //# avatar of this station instead.
    //# @return False if the station is taken, has no avatar, or #NewOccupant has no shape to change.
    //#
    bool Enter(Jafg::AAvatarComponent& NewOccupant);
    //#
    //# Lets the occupant leave where it stands. Its controller possesses it again.
    //# @param bApply Whether to keep the changes to the shape. Otherwise, the original shape is restored.
    //#
    void Leave(bool bApply);

    //# Where occupants stand: The center of the top of the bounds of the station. In world space.
    NODISCARD LWorldVec3 GetStandLocation() const noexcept;

private:

    Jafg::AAvatarComponent* Occupant{};
    //# The avatar of this station while occupied. Kept, because the actor no longer finds it while it is torn down.
    Jafg::AAvatarComponent* StationAvatar{};
    std::optional<LHumanoidShape> OriginalShape;
};

//#
//# Spawns a customization station: A platform to stand on, with a view from the front that can be orbited.
//# @param Where The center of the bottom of the platform.
//# @param Orientation Occupants face its forward.
//#
JGC_API Jafg::AActor& SpawnCustomizationStation(Jafg::LWorld& World, LWorldVec3 const& Where, LWorldQuat const& Orientation = maths::identity<LWorldQuat>);

} /* ~Namespace Jgc */
