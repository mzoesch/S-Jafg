// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Framework/ActorComponentForward.h"
#include "ActorComponent.generated.h"

namespace Jafg
{

class AActor;

DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API AActorComponent : public AWorldObject
{
    GENERATED_CLASS_BODY()

    friend AActor;

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AActorComponent)

public:

    //# Do not use. Use #OnAttach.
    virtual void BeginLife() override final { Super::BeginLife(); }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    //#
    //# Called when this component is attached to an actor. This call might be deferred quite a while if the actor
    //# itself is not spawned in.
    //#
    //# Once spawned in, the component cannot change its owner anymore.
    //#
    virtual void OnAttach(AActor& InOwner)
    {
#if JAFG_DO_CHECKS
        check(this->bHasExecutedOnAttach == false)
        this->bHasExecutedOnAttach = true;
        check(this->Owner == nullptr)
#endif /* JAFG_DO_CHECKS */
        this->Owner = &InOwner;
    }

    //#
    //# Called from the parent iff the parent is ticked and this component is allowed to tick.
    //# Generally speaking, if the parent is not ticked then one should *not* tick the components attached to it.
    //# If you really need to assert that your component is ticked whenever the owning outer is ticked then you
    //# can simply inherit from #LTickableObject.
    //#
    virtual void ParentTick(f32 Dt) { check(this->Owner && this->bTick) }

    constexpr void SetShouldRender(bool b) noexcept { this->bRender = b; }
    constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LActorRenderInfo const& Info) const {}

    FORCEINLINE constexpr bool IsOwningActorValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE constexpr AActor& GetOwningActor() const noexcept { check(this->IsOwningActorValid()) return *this->Owner; }

private:

    AActor* Owner{};

protected:

    bool bTick:1{};

private:

    bool bRender:1{};

#if JAFG_DO_CHECKS
    bool bHasExecutedOnAttach:1{};
#endif /* JAFG_DO_CHECKS */
};

} /* ~Namespace Jafg */
