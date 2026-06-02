// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"

void Jafg::AActor::BeginLife()
{
    Super::BeginLife();

    /*
     * Components that do not live.
     * But these components may emplace other components on their #OnAttach method that then will live.
     */
    TArray<AActorComponent*> PreComponents; PreComponents.reserve(this->Components.size());
    for (auto& Comp : this->Components)
    {
        PreComponents.emplace_back(&*Comp);
    }
    check(this->bLives == false)
    this->bLives = true;
    for (auto* Comp : PreComponents)
    {
        Comp->OnAttach(*this);
    }

    if (this->CanEverTick())
    {
        this->GetWorld().RegisterTickableObject(this);
    }

    return;
}

void Jafg::AActor::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    algo::orphan(&this->Components);

    if (this->CanEverTick())
    {
        auto& World{this->GetWorld()};
        if (World.IsTickableObjectsPutMutexLocked())
        {
            World.DeletedTickableObjects.emplace_back(static_cast<LTickableObject*>(this));
        }
        else
        {
            algo::erase_exactly_once_checked(&World.TickableObjects, static_cast<LTickableObject*>(this));
        }
    }
    else
    {
        check(!algo::contains(this->GetWorld().TickableObjects, static_cast<LTickableObject*>(this)))
    }

    return;
}
