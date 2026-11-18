// Copyright mzoesch. All rights reserved.

#include "Framework/Actor.h"
#include "Framework/AvatarComponent.h"

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

void Jafg::AActor::WorldTick(f32 Dt)
{
    check(this->bLives && !this->_IsGarbage())
    check(this->bCanEverTick && this->ShouldTick())
    for (auto& Comp: this->Components)
    {
        if (Comp->bTick)
        {
            STAT_CUSTOM_ZONE_FMT("Tick{}", Comp->GetNameAsString())
            Comp->ParentTick(Dt);
        }
    }
}

void Jafg::AActor::PhysicsTick(f32 Dt, bool bFollowup)
{
    check(this->bLives && !this->_IsGarbage())
    check(this->bCanEverPhysicsTick && this->ShouldPhysicsTick())
    for (auto& Comp: this->Components)
    {
        if (Comp->bPhysicsTick)
        {
            STAT_CUSTOM_ZONE_FMT("PhysicsTick{}", Comp->GetNameAsString())
            Comp->ParentPhysicsTick(Dt, bFollowup);
        }
    }
}

#if JAFG_WITH_EDITOR
void Jafg::AActor::SelectedTick(WEditorWorldViewer& Viewer, f32 Dt, AActorComponent* Comp)
{
    check(this->bLives && !this->_IsGarbage())
    if (Comp)
    {
        check(this->_check_IsComponentAttachedTransitive(*Comp))
        STAT_CUSTOM_ZONE_FMT("SelectedTick{}", Comp->GetNameAsString())
        Comp->SelectedTick(Viewer, Dt);
    }
}
#endif /* JAFG_WITH_EDITOR */

void Jafg::AActor::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->HasRootComponent())
    {
        /* Everything else is torn down as well. Then only the links have to go. */
        bool const bKeepWorld{Reason == EJxxRecordTearDownReason::Default};
        if (this->IsAttached())
        {
            auto& Root{*this->RootComponent};
            algo::erase_once_checked(&Root.AttachParent->AttachedActors, this);
            Root.AttachParent = nullptr;
        }
        this->RootComponent->DetachAttachedActors(bKeepWorld, true);
    }

    for (auto Components{std::exchange(this->Components, {})}; auto& Comp: Components)
    {
        check(Comp.get())
        check(&Comp->GetOwningActor() == this)
        Comp.release()->MarkAsGarbage_v2(Reason);
    }

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
                    STAT_CUSTOM_ZONE_FMT("Tick{}", this->GetNameAsString())
                    this->WorldTick(Dt);
                }
            }
            else
            {
                STAT_CUSTOM_ZONE_FMT("Tick{}", this->GetNameAsString())
                this->WorldTick(Dt);
            }
        }
        else
        {
            check(World.IsTimeDesisted())
            check(this->CanEverTickInDormantTimes())
            STAT_CUSTOM_ZONE_FMT("Tick{}", this->GetNameAsString())
            this->WorldTick(Dt);
        }
    }
}

void Jafg::AActor::FixedTick(f32 Dt, bool bFollowup)
{
    if (this->bShouldPhysicsTick)
    {
        STAT_CUSTOM_ZONE_FMT("PhysicsTick{}", this->GetNameAsString())
        this->PhysicsTick(Dt, bFollowup);
    }
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

bool Jafg::AActor::AttachTo(ASceneComponent& Parent, EAttachmentRule Rule /* = EAttachmentRule::KeepWorld */)
{
    check(Tasks::IsOnMasterThread())

    if (!this->HasRootComponent())
    {
        LOG_ERROR(LogWorld, "[{}]: Cannot attach an actor without a root component to [{}].", this->GetNameAsString(), Parent.GetNameAsString())
        return false;
    }
    if (!Parent.IsOwningActorValid())
    {
        LOG_ERROR(LogWorld, "[{}]: Cannot attach to [{}]. It is not part of an actor yet.", this->GetNameAsString(), Parent.GetNameAsString())
        return false;
    }
    for (AActor const* It{&Parent.GetOwningActor()}; It; It = It->GetAttachParentActor())
    {
        if (It == this)
        {
            LOG_ERROR(LogWorld, "[{}]: Cannot attach to [{}] of [{}]. That would create a cycle."
                , this->GetNameAsString(), Parent.GetNameAsString(), Parent.GetOwningActor().GetNameAsString()
                )
            return false;
        }
    }

    auto& Root{*this->RootComponent};
    if (!Root.CanFollowAttachParent())
    {
        LOG_ERROR(LogWorld, "[{}]: Cannot attach to [{}]. The root [{}] cannot follow a parent."
            , this->GetNameAsString(), Parent.GetNameAsString(), Root.GetNameAsString()
            )
        return false;
    }

    if (Root.AttachParent == &Parent)
    {
        return true;
    }

    LWorldTrans const World{Root.GetWorldTransformSlow()};
    if (Root.AttachParent)
    {
        algo::erase_once_checked(&Root.AttachParent->AttachedActors, this);
    }
    Root.AttachParent = &Parent;
    Parent.AttachedActors.emplace_back(this);

    if (Rule == EAttachmentRule::KeepWorld)
    {
        Root.SetWorldTransformByTeleport(World);
    }
    else
    {
        Root._detail_OnTransformChanged(ETransformChangeBits::Translation|ETransformChangeBits::Rotator|ETransformChangeBits::Scale);
    }

    LOG_VERBOSE(LogWorld, "[{}]: Attached to [{}] of [{}]."
        , this->GetNameAsString(), Parent.GetNameAsString(), Parent.GetOwningActor().GetNameAsString()
        )
    return true;
}

void Jafg::AActor::Detach(EAttachmentRule Rule /* = EAttachmentRule::KeepWorld */)
{
    check(Tasks::IsOnMasterThread())

    if (!this->IsAttached())
    {
        return;
    }

    auto& Root{*this->RootComponent};
    LWorldTrans const World{Root.GetWorldTransformSlow()};
    LOG_VERBOSE(LogWorld, "[{}]: Detaching from [{}].", this->GetNameAsString(), Root.AttachParent->GetNameAsString())
    algo::erase_once_checked(&Root.AttachParent->AttachedActors, this);
    Root.AttachParent = nullptr;

    if (Rule == EAttachmentRule::KeepWorld)
    {
        Root.SetLocalTransformByTeleport(World);
    }
    else
    {
        Root._detail_OnTransformChanged(ETransformChangeBits::Translation|ETransformChangeBits::Rotator|ETransformChangeBits::Scale);
    }
}

Jafg::AActor* Jafg::AActor::GetAttachParentActor() const noexcept
{
    if (auto* Parent{this->GetAttachParent()}; Parent && Parent->IsOwningActorValid())
    {
        return &Parent->GetOwningActor();
    }
    return nullptr;
}

Jafg::AAvatarComponent* Jafg::AActor::GetAvatar() noexcept
{
    return this->GetComponent<AAvatarComponent>();
}

Jafg::AAvatarComponent const* Jafg::AActor::GetAvatar() const noexcept
{
    return this->GetComponent<AAvatarComponent>();
}
