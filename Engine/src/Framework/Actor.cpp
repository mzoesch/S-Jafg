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

    if (this->CanEverTick())
    {
        this->GetWorld().RegisterTickableObject(this);
    }

    return;
}

void Jafg::AActor::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    algo::orphan(&this->Components);

    if (this->CanEverTick())
    {
        auto& World{this->GetWorld()};
        if (World.IsTickableObjectsPutMutexLocked())
        {
            World.DeletedTickableObjects.emplace_back(static_cast<LTickableObject*>(this));
        }
        else
        {
            algo::erase_exactly_once_checked(&World.TickableObjects, static_cast<LTickableObject*>(this));
        }
    }
    else
    {
        check(!algo::contains(this->GetWorld().TickableObjects, static_cast<LTickableObject*>(this)))
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
