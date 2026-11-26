// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/PatrolComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/Actor.h"

DECLARE_INLINE_LOG_CATEGORY(LogPatrol, JAFG_LOG_DEFAULT_VERBOSITY)

void Jgc::APatrolComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    auto* Movement{this->GetOwningActor().GetComponent<Jafg::ACharacterMovementComponent>()};
    if (!Movement)
    {
        LOG_WARNING(LogPatrol, "[{}]: No [{}] component attached."
            , this->GetNameAsString()
            , algo::type_name<Jafg::ACharacterMovementComponent>()
            )
        return;
    }
    if (this->Waypoints.empty())
    {
        LOG_WARNING(LogPatrol, "[{}]: No waypoints set."
            , this->GetNameAsString()
            )
        return;
    }
    this->Target %= this->Waypoints.size();

    LVec3F const Up{Movement->GetUp()};
    LVec3F ToTarget{this->Waypoints[this->Target] - this->GetOwningActor().GetRootComponent().GetWorldTranslationSlow()};
    ToTarget -= Up * glm::dot(ToTarget, Up);
    f32 const Distance{glm::length(ToTarget)};

    if (Distance <= this->AcceptanceRadius)
    {
        Movement->SetMoveDirectionWorld(LVec3F{0.0f});
        Movement->SetSprint(false);
        this->Waited += Dt;
        if (this->Waited >= this->WaitTime)
        {
            this->Waited = 0.0f;
            this->Target = (this->Target + 1) % this->Waypoints.size();
        }
        return;
    }

    LVec3F const Direction{ToTarget / Distance};
    Movement->SetMoveDirectionWorld(Direction * maths::clamp(this->SpeedScale, 0.0f, 1.0f));
    Movement->SetLookAtDirection(Direction);
    Movement->SetSprint(this->bSprintEverySecondWay && this->Target % 2 == 1);
}
