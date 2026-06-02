// Copyright mzoesch. All rights reserved.

#include "Components/SceneComponent.h"
#include "Framework/Actor.h"

void Jafg::ASceneComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    checkCode
    (
        if (IsValidFast(this->GetOuter(), &this->GetOwningActor()))
        {
            check(this->GetOwningActor().HasRootComponent())
            if (&this->GetOwningActor().GetRootComponent() == this)
            {
                panicMsgf("[{}]: Root component of actor [{}] is being destroyed."
                    , this->GetNameAsString(), this->GetOwningActor().GetNameAsString()
                    )
            }
        }
    )

    return;
}
