// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"

void Jafg::AActor::BeginLife()
{
    Super::BeginLife();

    /*
     * Components that do not live.
     * But these components may emplace other components on their #OnAttach method that then will live.
     */
    TArray<AActorComponent*> PreComponents; PreComponents.reserve(this->Components.size());
    for (auto& Comp : this->Components)
    {
        PreComponents.emplace_back(&*Comp);
    }
    check(this->bLives == false)
    this->bLives = true;
    for (auto* Comp : PreComponents)
    {
        Comp->OnAttach(*this);
    }

    if (this->GetWorld().IsTimeLinear())
    {
        if (this->CanEverTick())
        {
            this->GetWorld().RegisterTickableObject(this);
        }
        if (this->CanEverPhysicsTick())
        {
            this->GetWorld().RegisterPhysicsTickableObject(this);
        }
    }
    else if (this->GetWorld().IsTimeDesisted())
    {
        if (this->CanEverTickInDormantTimes())
        {
            this->GetWorld().RegisterTickableObject(this);
        }
    }
    else
    {
        std::unreachable();
    }

    return;
}

void Jafg::AActor::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    algo::orphan(&this->Components);

    if ((this->CanEverTick() && this->GetWorld().IsTimeLinear()) || (this->CanEverTickInDormantTimes() && this->GetWorld().IsTimeDesisted()))
    {
        this->GetWorld().UnregisterTickableObject(this);
    }
    else
    {
        check(!algo::contains(this->GetWorld().TickableObjects, static_cast<LTickableObject*>(this)))
    }

    if (this->CanEverPhysicsTick() && this->GetWorld().IsTimeLinear())
    {
        this->GetWorld().UnregisterPhysicsTickableObject(this);
    }
    else
    {
        check(!algo::contains(this->GetWorld().PhysicsTickableObjects, static_cast<LFixedTickableObject*>(this)))
    }

    return;
}

Jafg::AWorldObject& Jafg::AActor::CloneImpl(AWorldObject* Object) const noexcept
{
    auto& Result{Super::CloneImpl(Object).AsStatic<AActor>()};
    check(!Result._HasBegunLife())

    for (auto const& Comp: this->Components)
    {
        if (&this->GetRootComponent() == &*Comp)
        {
            check(Comp->IsA<ASceneComponent>())
            LOG_TRACE(LogJxx, "[{}]: Cloning root component.", Comp->GetVirtualTable().GetFullyQualifiedName())
            Result.EmplaceRootComponent(TSubclassOf<ASceneComponent>{Comp->GetVirtualTable()}, [&](AActorComponent& To)
            {
                auto& From{*StaticCastChecked<ASceneComponent>(&*Comp)};
                for (auto& Field: From.GetVirtualTable().FieldIter())
                {
                    if (!(Field.Flags & EJxxFieldBits::Transient))
                    {
                        Field.FastClone(From, &To);
                    }
                }
                check(From.GetChildren().empty() && "TODO")
            });
        }
        else
        {
            check(!Comp->IsA<ASceneComponent>())
            LOG_TRACE(LogJxx, "[{}]: Cloning component.", Comp->GetVirtualTable().GetFullyQualifiedName())
            if (Comp->IsRuntimeComponent())
            {
                Result.EmplaceComponent(TSubclassOf<AActorComponent>{Comp->GetVirtualTable()}, [&](AActorComponent& To)
                {
                    (void)Comp->CloneImpl(&To);
                });
            }
            else
            {
                AActorComponent* Target{};
                for (auto& DefaultComp: Result.GetComponents())
                {
                    /* Exact match; no inheritance. */
                    if (&Comp->GetVirtualTable() == &DefaultComp->GetVirtualTable())
                    {
                        if (Target)
                        {
                            LOG_FATAL(LogJxx, "[{}]: Found multiple default components of the same type [{}] during clone. This is ambiguous."
                                , Result.GetVirtualTable().GetFullyQualifiedName(), Comp->GetVirtualTable().GetFullyQualifiedName())
                        }
                        Target = &*DefaultComp;
                    }
                }
                if (!Target)
                {
                    LOG_FATAL(LogJxx, "[{}]: Could not find default component of type [{}] during clone."
                        , Result.GetVirtualTable().GetFullyQualifiedName(), Comp->GetVirtualTable().GetFullyQualifiedName())
                }
                (void)Comp->CloneImpl(Target);
            }
        }
    }

    return Result;
}

void Jafg::AActor::Tick(f32 Dt)
{
    if (auto const& World{this->GetWorld()}; this->ShouldTick())
    {
        if (World.IsTimeLinear())
        {
            check(this->CanEverTick())
            if (World.IsLinearWorldDormant())
            {
                if (this->CanEverTickInDormantTimes())
                {
                    this->WorldTick(Dt);
                }
            }
            else
            {
                this->WorldTick(Dt);
            }
        }
        else
        {
            check(World.IsTimeDesisted())
            check(this->CanEverTickInDormantTimes())
            this->WorldTick(Dt);
        }
    }
}

void Jafg::AActor::FixedTick(f32 Dt, bool bFollowup)
{
    this->PhysicsTick(Dt, bFollowup);
}

#if JAFG_DO_CHECKS
bool Jafg::AActor::_check_IsComponentAttachedTransitive(AActorComponent const& Who) const noexcept
{
    bool bSceneComp{Who.IsA<ASceneComponent>()};
    for (auto& Comp: this->Components)
    {
        check(Comp.get())
        check(!Comp->IsOwningActorValid() || &Comp->GetOwningActor() == this)

        if (&*Comp == &Who)
        {
            return true;
        }

        if (bSceneComp)
        {
            if (auto* SceneComp{Comp->As<ASceneComponent>()})
            {
                auto IsAttachedInChild{[this](this auto&& Self, ASceneComponent const& Comp, ASceneComponent const& Who) noexcept -> bool
                {
                    check(!Comp.IsOwningActorValid() || &Comp.GetOwningActor() == this)
                    check(&Comp != &Who)
                    for (auto& Child: Comp.GetChildren())
                    {
                        check(Child.get())
                        check(!Comp.IsOwningActorValid() || &Child->GetOwningActor() == this)
                        if (&*Child == &Who)
                        {
                            return true;
                        }
                        if (Self(*Child, Who))
                        {
                            return true;
                        }
                    }

                    return false;
                }};
                if (IsAttachedInChild(*SceneComp, Who.AsStatic<ASceneComponent>()))
                {
                    return true;
                }
            }
        }
    }
    return false;
}
#endif /* JAFG_DO_CHECKS */
