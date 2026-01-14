// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"
#include "Components/RenderComponent.h"
#include "Components/NoPhysicsCompontent.h"

void Jafg::AActor::EndLife()
{
    Super::EndLife();
}

void Jafg::AActor::OnGarbage(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter)
{
    Super::OnGarbage(Reason, PreviousOuter);

    check( this->GetWorld() == nullptr )
    check( PreviousOuter.IsWorld() )

    if (this->CanEverTick())
    {
        if (PreviousOuter.AsWorld()->IsTickableObjectsPutMutexLocked())
        {
            PreviousOuter.AsWorld()->DeletedTickableObjects.emplace_back(static_cast<LTickableObject*>(this));
        }
        else
        {
            algo::erase_once_checked(&PreviousOuter.AsWorld()->TickableObjects, static_cast<LTickableObject*>(this));
        }
    }

    return;
}
