// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/InteractableComponent.h"
#include "Framework/Pawn.h"

bool Jgc::AInteractableComponent::CanInteract(Jafg::APawn const& Instigator) const
{
    /* Pawns cannot interact with themselves. */
    return static_cast<Jafg::AActor const*>(&Instigator) != &this->GetOwningActor();
}

LString Jgc::AInteractableComponent::GetInteractionText() const
{
    return "Interact";
}
