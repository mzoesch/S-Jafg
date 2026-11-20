// Copyright mzoesch. All rights reserved.

#include "Framework/SceneComponent.h"
#include "Framework/Actor.h"

void Jafg::ASceneComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    /* Usually already done by the actor. But components may also be removed on their own. */
    this->DetachAttachedActors(Reason == EJxxRecordTearDownReason::Default, false);

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

void Jafg::ASceneComponent::OnAttach(AActor& InOwner)
{
    /*
     * Components that do not live.
     * But these components may emplace other components on their #OnAttach method that then will live.
     */
    TArray<ASceneComponent*> PreChildren; PreChildren.reserve(this->Children.size());
    for (auto& Child: this->Children)
    {
        PreChildren.emplace_back(&*Child);
    }

    Super::OnAttach(InOwner);

#if JAFG_DO_CHECKS
    if (this->Parent)
    {
        check(algo::contains(this->Parent->GetChildren(), this, algo::unique_raw))
    }
    else
    {
        check(&this->GetOwningActor().GetRootComponent() == this)
    }
#endif /* JAFG_DO_CHECKS */

    for (auto* Child: PreChildren)
    {
        Child->OnAttach(InOwner);
    }
}

std::optional<LWorldReal> Jafg::ASceneComponent::TraceNonPhysical(LWorldMagRay3 const& Ray) const
{
    if (this->Aabb.empty())
    {
        return {};
    }
    if (auto const Hit{maths::aabb_intersect_ray(Ray, this->Aabb.apply(this->GetWorldTransformSlow()))}; Hit.bHit)
    {
        return Hit.Enter;
    }
    return {};
}

Jafg::AWorldObject& Jafg::ASceneComponent::CloneImpl(AWorldObject* Object) const noexcept
{
    auto& Result{Super::CloneImpl(Object).AsStatic<AActor>()};
    check(!Result._HasBegunLife())

    check(this->Children.empty() && "Not yet implemented.")

    return Result;
}

#if JAFG_DO_CHECKS
void Jafg::ASceneComponent::_check_EmplaceRuntimeComponentPreCondition()
{
    check(this->IsOwningActorValid())
    check(this->GetOwningActor()._Lives())
}
#endif /* JAFG_DO_CHECKS */

void Jafg::ASceneComponent::DetachAttachedActors(bool bKeepWorld, bool bTransitive)
{
    for (auto Attached{std::exchange(this->AttachedActors, {})}; AActor* Actor: Attached)
    {
        check(Actor && Actor->HasRootComponent())
        check(Actor->GetRootComponent().AttachParent == this)
        auto& Root{Actor->GetRootComponent()};
        LWorldTrans const World{Root.GetWorldTransformSlow()};
        Root.AttachParent = nullptr;
        if (bKeepWorld)
        {
            Root.SetLocalTransformByTeleport(World);
        }
        LOG_VERBOSE(LogWorld, "[{}]: Detached [{}] because the parent goes away.", this->GetNameAsString(), Actor->GetNameAsString())
    }

    if (bTransitive)
    {
        for (auto& Child: this->Children)
        {
            Child->DetachAttachedActors(bKeepWorld, true);
        }
    }
}

void Jafg::ASceneComponent::_TickHierarchy(f32 Dt)
{
    if (this->bTick)
    {
        this->ParentTick(Dt);
    }
    /* Indices, because components may be emplaced while ticking. */
    for (std::size_t Idx{0}; Idx < this->Children.size(); ++Idx)
    {
        this->Children[Idx]->_TickHierarchy(Dt);
    }
}
