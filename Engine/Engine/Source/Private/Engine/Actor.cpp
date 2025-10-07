// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Engine/Actor.h"
#include "Components/RenderComponent.h"
#include "Components/NoPhysicsCompontent.h"

void Jafg::AActor::EndLife()
{
    Super::EndLife();

    this->SetRendererComponent(nullptr, true);
    this->SetPhysicsComponent(nullptr, true);

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
            this->GetWorld()->DeletedTickableObjects.push_back(this);
        }
        else
        {
            algo::erase_once_checked(&this->GetWorld()->TickableObjects, this);
        }
    }

    if (this->bWeakContext == false && this->GetWorld()->GetWorldState() != EWorldState::TearingDown)
    {
        /*
         * Though we keep the pointer to the world to let this actor's children unsubscribe to world-specific
         * delegates - the context on the other hand should now no longer care about this child.
         */
        algo::erase_once_checked(&this->GetWorld()->Actors, this);
    }

    return;
}

Jafg::LPhysicsComponent* Jafg::AActor::GetPhysicsComponent() const
{
    return this->PhysicsComponent ? this->PhysicsComponent : LNoPhysicsComponent::GetUsableClass();
}

void Jafg::AActor::ChangeTransform(const LTransform& InTransform, const EActorSweep::Type SweepType)
{
    this->Transform = InTransform;
}

void Jafg::AActor::AddTranslation(const LVector& InLocation, const EActorSweep::Type SweepType)
{
    this->Transform.Translation += InLocation;
}

void Jafg::AActor::AddRotator(const LRotator& InRotator, const EActorSweep::Type SweepType)
{
    this->Transform.Rotator += InRotator;
}

void Jafg::AActor::AddScale(const LVector& InScale, const EActorSweep::Type SweepType)
{
    this->Transform.Scale += InScale;
}

void Jafg::AActor::SetTranslation(const LVector& InLocation, const EActorSweep::Type SweepType)
{
    this->Transform.Translation = InLocation;
}

void Jafg::AActor::SetRotator(const LRotator& InRotator, const EActorSweep::Type SweepType)
{
    this->Transform.Rotator = InRotator;
}

void Jafg::AActor::SetScale(const LVector& InScale, const EActorSweep::Type SweepType)
{
    this->Transform.Scale = InScale;
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

void Jafg::AActor::SetPhysicsComponent(LPhysicsComponent* InPhysicsComponent, const bool bFreeOld)
{
    if (bFreeOld && this->PhysicsComponent != nullptr)
    {
        delete this->PhysicsComponent;
    }

    this->PhysicsComponent = InPhysicsComponent;

    return;
}
