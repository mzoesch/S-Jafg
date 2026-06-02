// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Components/ActorComponentForward.h"
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

    constexpr void SetShouldRender(bool b) noexcept { this->bRender = b; }
    constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LActorRenderInfo const& Info) noexcept {}

    FORCEINLINE constexpr bool IsOwningActorValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE constexpr AActor& GetOwningActor() const noexcept { check(this->IsOwningActorValid()) return *this->Owner; }

private:

#if JAFG_DO_CHECKS
    bool bHasExecutedOnAttach{};
#endif /* JAFG_DO_CHECKS */

    AActor* Owner{};
    bool bRender:1{};
};

} /* ~Namespace Jafg */
