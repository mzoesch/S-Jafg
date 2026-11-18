// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Framework/PersonaControllerComponent.h"
#include "Framework/AvatarComponent.h"
#include "Engine/Engine.h"

#if JAFG_DO_CHECKS
void Jafg::APersonaControllerComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    if (!this->GetOwningActor().IsA<APersonaController>())
    {
        LOG_FATAL(LogUserInput, "Class [{}] requires to be attached a [{}] but is on [{}].",
            this->GetNameAsString(),
            APersonaController::StaticClass().GetFullyQualifiedName(),
            this->GetOwningActor().GetNameAsString()
        )
    }

    return;
}
#endif /* JAFG_DO_CHECKS */

void Jafg::APersonaController::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->IsOwningLackeyValid())
    {
        if (!App::IsTearingDown())
        {
            LOG_WARNING(LogEgo, "[{}]: Ending life while still possessed by a lackey."
                , this->GetNameAsString())
        }

        algo::leak(this->GetOwningBaseLackey().PossessPersonaController(nullptr, true));
    }

    if (this->IsPossessing())
    {
        this->Possess(nullptr);
    }

    for (auto Adopted{std::exchange(this->AdoptedActors, {})}; AActor* Actor: Adopted)
    {
        if (IsValidFast(this->GetOuter(), Actor) && !Actor->_IsGarbage())
        {
            Actor->MarkAsGarbage_v2(Reason);
        }
    }

    return;
}

Jafg::AActor* Jafg::APersonaController::GetPossessedActor() const noexcept
{
    return this->Avatar && this->Avatar->IsOwningActorValid() ? &this->Avatar->GetOwningActor() : nullptr;
}

void Jafg::APersonaController::Possess(AAvatarComponent* New)
{
    check(Tasks::IsOnMasterThread())
    check(this->_Lives())

    if (this->Avatar == New)
    {
        return;
    }

    if (New && New->GetController())
    {
        LOG_VERBOSE(LogEgo, "[{}]: Taking avatar [{}] from [{}]."
            , this->GetNameAsString(), New->GetNameAsString(), New->GetController()->GetNameAsString()
            )
        New->GetController()->Possess(nullptr);
        check(!New->GetController())
    }

    if (AAvatarComponent* Previous{std::exchange(this->Avatar, nullptr)})
    {
        Previous->_SetController(nullptr);
    }

    /* A confined cursor belongs to the gameplay of the previous avatar. */
    if (auto* Local{this->TryGetOwningLackey<ELackey::Local>()})
    {
        Local->GetUserInput().SetConfineMouse(false);
    }

    LOG_VERBOSE(LogEgo, "[{}]: Possessing [{}]."
        , this->GetNameAsString(), New && New->IsOwningActorValid() ? New->GetOwningActor().GetNameAsString() : LString{"nothing"}
        )

    this->Avatar = New;
    if (New)
    {
        New->_SetController(this);
    }
}

Jafg::AEyeComponent* Jafg::APersonaController::GetViewEye() const noexcept
{
    return this->Avatar ? this->Avatar->GetEye() : nullptr;
}

void Jafg::APersonaController::AdoptActor(TJxxUnique<AActor> Actor)
{
    check(Actor)
    check(!algo::contains(this->AdoptedActors, Actor.get()))
    this->AdoptedActors.emplace_back(Actor.release());
}

TJxxUnique<Jafg::AActor> Jafg::APersonaController::AbandonActor(AActor& Actor)
{
    if (auto It{algo::find(this->AdoptedActors, &Actor)}; It != this->AdoptedActors.end())
    {
        this->AdoptedActors.erase(It);
        return TJxxUnique<AActor>{&Actor};
    }
    return {};
}

void Jafg::APersonaController::_OnAvatarGarbage(AAvatarComponent& Garbage) noexcept
{
    check(this->Avatar == &Garbage)
    LOG_VERBOSE(LogEgo, "[{}]: Possessed avatar [{}] is torn down.", this->GetNameAsString(), Garbage.GetNameAsString())

    /* Its actor is torn down as well. The components can no longer be notified. */
    this->Avatar = nullptr;
    Garbage._SetController(nullptr);
    if (Garbage.IsOwningActorValid())
    {
        algo::erase_once(&this->AdoptedActors, &Garbage.GetOwningActor());
    }
}
