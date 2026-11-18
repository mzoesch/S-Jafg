// Copyright mzoesch. All rights reserved.

#include "Framework/AvatarComponent.h"
#include "Framework/Actor.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"

namespace
{

//# Every component of the actor, including the hierarchy below the root.
void NotifyPossessionChanged(Jafg::ASceneComponent& Comp, Jafg::APersonaController* Previous, Jafg::APersonaController* New)
{
    Comp.OnPossessionChanged(Previous, New);
    for (auto const& Child: Comp.GetChildren())
    {
        ::NotifyPossessionChanged(*Child, Previous, New);
    }
}

} /* ~Namespace <Anonymous> */

void Jafg::AAvatarComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    std::size_t const Count{static_cast<std::size_t>(std::ranges::count_if(InOwner.GetComponents(), [](auto const& Comp)
    {
        return Comp->template IsA<AAvatarComponent>();
    }))};
    if (Count != 1uz)
    {
        LOG_FATAL(LogWorld, "[{}]: Actor [{}] has [{}] avatars. An actor can have at most one."
            , this->GetNameAsString(), InOwner.GetNameAsString(), Count
            )
    }
}

void Jafg::AAvatarComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    if (this->Controller)
    {
        this->Controller->_OnAvatarGarbage(*this);
        check(!this->Controller)
    }

    Super::OnGarbage(Reason);
}

bool Jafg::AAvatarComponent::IsPossessedLocally() const noexcept
{
    return this->Controller && this->Controller->IsLocallyPossessed();
}

Jafg::LLocalEgo* Jafg::AAvatarComponent::GetLocalEgoIfPossessed() const noexcept
{
    return this->Controller ? &this->Controller->GetMutableLocalEgo() : nullptr;
}

Jafg::ASceneComponent const* Jafg::AAvatarComponent::GetActorRoot() const noexcept
{
    if (this->IsOwningActorValid() && this->GetOwningActor().HasRootComponent())
    {
        return &this->GetOwningActor().GetRootComponent();
    }
    return nullptr;
}

Jafg::AEyeComponent* Jafg::AAvatarComponent::GetEye() const noexcept
{
    if (this->Eye && IsValidFast(this->GetOuter(), this->Eye))
    {
        return this->Eye;
    }
    if (this->IsOwningActorValid() && this->GetOwningActor().HasRootComponent())
    {
        return AEyeComponent::FindFirstEyeIn(this->GetOwningActor().GetRootComponent());
    }
    return nullptr;
}

void Jafg::AAvatarComponent::SetEye(AEyeComponent* NewEye) noexcept
{
    check(!NewEye || !this->IsOwningActorValid() || &NewEye->GetOwningActor() == &this->GetOwningActor())
    this->Eye = NewEye;
}

void Jafg::AAvatarComponent::_SetController(APersonaController* New)
{
    check(this->Controller != New)
    APersonaController* const Previous{std::exchange(this->Controller, New)};

#if JAFG_WITH_LOCAL_LAYER
    if (New && New->IsLocallyPossessed())
    {
        f32 const Near{New->GetLocalEgo().GetVariable_FrustumNearPlane()};
        f32 const Far{New->GetLocalEgo().GetVariable_FrustumFarPlane()};
        this->ForEachEye([Near, Far](AEyeComponent& Eye)
        {
            Eye.NearFrustum = Near;
            Eye.FarFrustum = Far;
        });
    }
#endif /* JAFG_WITH_LOCAL_LAYER */

    if (!this->IsOwningActorValid())
    {
        return;
    }

    /* The root is part of the components. Its hierarchy is notified with it. */
    for (auto const& Comp: this->GetOwningActor().GetComponents())
    {
        if (auto* Scene{Comp->As<ASceneComponent>()})
        {
            ::NotifyPossessionChanged(*Scene, Previous, New);
        }
        else
        {
            Comp->OnPossessionChanged(Previous, New);
        }
    }
}
