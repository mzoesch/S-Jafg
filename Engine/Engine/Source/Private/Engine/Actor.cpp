// Copyright mzoesch. All rights reserved.

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

void Jafg::AActor::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    Super::OnGarbage(Reason);

    check( this->GetWorld() )

    if (this->CanEverTick())
    {
        if (this->GetWorld()->IsTickableObjectsPutMutexLocked())
        {
            this->GetWorld()->DeletedTickableObjects.emplace_back(static_cast<LTickableObject*>(this));
        }
        else
        {
            algo::erase_once_checked(&this->GetWorld()->TickableObjects, static_cast<LTickableObject*>(this));
        }
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
