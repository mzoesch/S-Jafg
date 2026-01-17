// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"
#include "Components/RenderComponent.h"

void Jafg::AActor::BeginLife()
{
    Super::BeginLife();

    TArray<JActorComponent*> DeferredComponents; DeferredComponents.reserve(algo::size(this->Components));
    for (auto const& Comp : this->Components)
    {
        DeferredComponents.emplace_back(&*Comp);
        MakeDeferredObjectFinal(&*Comp);
    }

    for (auto const& Comp : DeferredComponents)
    {
        Comp->OnAttach(this);
    }

    return;
}

void Jafg::AActor::EndLife()
{
    Super::EndLife();

    this->Components = LComponentArray{};

    return;
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
