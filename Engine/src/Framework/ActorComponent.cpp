// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"
#include "Framework/AvatarComponent.h"
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

Jafg::AAvatarComponent* Jafg::AActorComponent::GetOwningAvatar() noexcept
{
    return this->IsOwningActorValid() ? this->GetOwningActor().GetAvatar() : nullptr;
}

Jafg::AAvatarComponent const* Jafg::AActorComponent::GetOwningAvatar() const noexcept
{
    return this->IsOwningActorValid() ? this->GetOwningActor().GetAvatar() : nullptr;
}

Jafg::APersonaController* Jafg::AActorComponent::GetOwningController() noexcept
{
    auto* Avatar{this->GetOwningAvatar()};
    return Avatar ? Avatar->GetController() : nullptr;
}

Jafg::APersonaController const* Jafg::AActorComponent::GetOwningController() const noexcept
{
    auto* Avatar{this->GetOwningAvatar()};
    return Avatar ? Avatar->GetController() : nullptr;
}
