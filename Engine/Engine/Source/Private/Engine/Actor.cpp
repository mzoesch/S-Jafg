// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Actor.h"
#include "Engine/Components/RenderComponent.h"

void Jafg::AActor::EndLife()
{
    Super::EndLife();

    this->SetRendererComponent(nullptr, true);

    return;
}

void Jafg::AActor::OnGarbage()
{
    Super::OnGarbage();

    check( this->GetWorld() )

    if (this->CanEverTick())
    {
        if (this->GetWorld()->IsTickableObjectsPutMutexLocked())
        {
            this->GetWorld()->DeletedTickableObjects.Add(this);
        }
        else
        {
            this->GetWorld()->TickableObjects.RemoveOnceChecked(this);
        }
    }

    if (this->GetWorld()->GetWorldState() != EWorldState::TearingDown)
    {
        /*
         * Though we keep the pointer to the world to let this actor's children unsubscribe to world-specific
         * delegates - the context on the other hand should now no longer care about this child.
         */
        this->GetWorld()->Actors.RemoveOnceChecked(this);
    }

    return;
}

void Jafg::AActor::SetRendererComponent(LRendererComponent* InRendererComponent, const bool bFreeOld /* = true */)
{
    if (bFreeOld && this->RendererComponent != nullptr)
    {
        delete this->RendererComponent;
    }

    this->RendererComponent = InRendererComponent;

    return;
}
