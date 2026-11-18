// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Framework/PersonaControllerComponent.h"
#include "Framework/AvatarComponent.h"
#include "Framework/EyeComponent.h"
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

void Jafg::APersonaController::BeginLife()
{
    Super::BeginLife();
    this->GetWorld()._RegisterPersonaController(this);
}

void Jafg::APersonaController::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    this->GetWorld()._UnregisterPersonaController(this);
    if (this->IsViewedEyeValid())
    {
        this->ViewedEye->_RemoveViewer(*this);
    }
    this->ViewedEye = nullptr;
    this->ViewBlend.reset();

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

std::optional<LWorldEye> Jafg::APersonaController::GetView() const noexcept
{
    AEyeComponent const* const Eye{this->GetViewEye()};
    if (!Eye)
    {
        return {};
    }
    if (this->View && this->ViewBlend)
    {
        return this->View;
    }
    /* The eye changed since the last update, which starts to blend from what was seen last. Do not jump ahead. */
    if (this->View && this->ViewedEye != Eye && Eye->BlendInTime > 0.0f)
    {
        return this->View;
    }
    /* Fresh. A sprung eye presents what its last update computed. */
    return Eye->GetEye();
}

void Jafg::APersonaController::SnapView() noexcept
{
    this->bSnapNextView = true;
    this->ViewBlend.reset();
    if (this->IsViewedEyeValid())
    {
        this->ViewedEye->SnapPresentation();
        this->View = this->ViewedEye->GetEye();
    }
}

bool Jafg::APersonaController::IsViewedEyeValid() const noexcept
{
    return this->ViewedEye && IsValidFast(this->GetOuter(), this->ViewedEye);
}

void Jafg::APersonaController::ChangeViewedEye(AEyeComponent* NewEye, bool bBlend)
{
    AEyeComponent* const Previous{this->IsViewedEyeValid() ? this->ViewedEye : nullptr};

    /* Blend from wherever the view is right now. That also covers changes in the middle of a blend. */
    if (bBlend && NewEye && this->View && NewEye->BlendInTime > 0.0f)
    {
        this->ViewBlend = LViewBlend{
            .From = *this->View,
            /* A blend that was interrupted has no single eye to follow. */
            .FromEye = this->ViewBlend ? nullptr : Previous,
            .Elapsed = 0.0f,
            .Duration = NewEye->BlendInTime,
            };
    }
    else
    {
        this->ViewBlend.reset();
    }

    if (Previous)
    {
        Previous->_RemoveViewer(*this);
    }
    this->ViewedEye = NewEye;
    if (NewEye)
    {
        NewEye->_AddViewer(*this);
    }

    LOG_VERBOSE(LogEgo, "[{}]: Viewing through [{}]{}."
        , this->GetNameAsString()
        , NewEye ? NewEye->GetNameAsString() : LString{"nothing"}
        , this->ViewBlend ? algo::sprintf(" blending for [{}] seconds", this->ViewBlend->Duration) : LString{}
        )
}

void Jafg::APersonaController::_UpdateView(f32 Dt)
{
    AEyeComponent* const Target{this->GetViewEye()};
    bool const bSnap{std::exchange(this->bSnapNextView, false)};
    if (Target != this->ViewedEye || (Target && !this->IsViewedEyeValid()))
    {
        this->ChangeViewedEye(Target, !bSnap);
    }

    if (!Target)
    {
        this->View.reset();
        this->ViewBlend.reset();
        return;
    }

    Target->UpdatePresentation(Dt);
    LWorldEye const Goal{Target->GetEye()};

    if (!this->ViewBlend)
    {
        this->View = Goal;
        return;
    }

    auto& Blend{*this->ViewBlend};
    if (Blend.FromEye && IsValidFast(this->GetOuter(), Blend.FromEye))
    {
        /* The previous eye is not viewed anymore. Keep it moving, so that the blend starts from a living view. */
        Blend.FromEye->UpdatePresentation(Dt);
        Blend.From = Blend.FromEye->GetEye();
    }
    else
    {
        Blend.FromEye = nullptr;
    }

    Blend.Elapsed += Dt;
    f32 const T{maths::clamp(Blend.Elapsed / maths::max(Blend.Duration, 1.0e-4f), 0.0f, 1.0f)};
    /* Smootherstep: Starts and ends without a jump in speed. */
    f32 const Alpha{T * T * T * (T * (T * 6.0f - 15.0f) + 10.0f)};
    this->View = AEyeComponent::BlendEyes(Blend.From, Goal, Alpha);

    if (T >= 1.0f)
    {
        this->ViewBlend.reset();
    }
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
