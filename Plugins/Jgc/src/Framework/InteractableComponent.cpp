// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/InteractableComponent.h"
#include "Framework/Actor.h"
#include "Framework/AvatarComponent.h"

bool Jgc::AInteractableComponent::CanInteract(Jafg::AAvatarComponent const& Instigator) const
{
    /* Nothing interacts with itself. */
    return !Instigator.IsOwningActorValid() || &Instigator.GetOwningActor() != &this->GetOwningActor();
}

LString Jgc::AInteractableComponent::GetInteractionText() const
{
    return "Interact";
}
