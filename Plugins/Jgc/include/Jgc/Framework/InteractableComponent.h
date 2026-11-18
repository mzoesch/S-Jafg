// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "InteractableComponent.generated.h"

namespace Jafg
{

class AAvatarComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

//#
//# Something that an avatar can interact with, e.g., by pointing at the owning actor with the cursor and clicking it.
//# The interaction is found by tracing the physics of the world. The owning actor therefore needs a physical body.
//#
DECLARE_JAFG_CLASS(::Jafg::EJxxClassBits::Abstract)
class JGC_API AInteractableComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AInteractableComponent)

public:

    //# Whether #Instigator can interact right now (e.g., the seat of a vehicle is still free).
    NODISCARD virtual bool CanInteract(Jafg::AAvatarComponent const& Instigator) const;
    //#
    //# Interact with this component. Only called if #CanInteract allowed it.
    //# @param Instigator The avatar that interacts. Its controller may be null (e.g., a bot without one).
    //# @return Whether the interaction happened.
    //#
    virtual bool Interact(Jafg::AAvatarComponent& Instigator) PURE_VIRTUAL(return false)
    //# Short description of the interaction for the user (e.g., "Enter").
    NODISCARD virtual LString GetInteractionText() const;

    //# How far away from the eye of the instigator the interaction may happen. In meters.
    CLASS_FIELD(EditorEditable)
    f32 MaxInteractionDistance{ 25.0f };
};

} /* ~Namespace Jgc */
