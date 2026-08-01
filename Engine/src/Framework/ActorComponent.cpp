// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"
#include "Framework/ActorComponent.h"

void Jafg::AActorComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    checkCode
    (
        if (IsValidFast(this->GetOuter(), this->Owner))
        {
            check(!algo::contains(this->Owner->GetComponents(), this, algo::unique_raw)
                && "Component was not correctly removed with #AActor::RemoveComponent.")
        }
    )

    return;
}
