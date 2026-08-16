// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldObject.h"
#include "Framework/ActorComponentForward.h"
#include "ActorComponent.generated.h"

namespace Jafg
{

class AActor;

enum struct EActorComponentOrigin: u8
{
    Unknown,
    Ctor,
    Runtime,
};

DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API AActorComponent : public AWorldObject
{
    GENERATED_CLASS_BODY()

    friend AActor;

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AActorComponent)

public:

    //# Do not use. Use #OnAttach.
    virtual void BeginLife() override final { check(this->Origin == EActorComponentOrigin::Unknown) Super::BeginLife(); }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    //#
    //# Called when this component is attached to an actor. This call might be deferred quite a while if the actor
    //# itself is not spawned in.
    //#
    //# Once spawned in, the component cannot change its owner anymore.
    //#
    virtual void OnAttach(AActor& InOwner)
    {
        check(this->Origin != EActorComponentOrigin::Unknown)
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
    NODISCARD constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LActorRenderInfo const& Info) const {}

    NODISCARD constexpr bool IsOwningActorValid() const noexcept { return this->Owner != nullptr; }
    NODISCARD constexpr AActor& GetOwningActor() noexcept { check(this->IsOwningActorValid()) return *this->Owner; }
    NODISCARD constexpr AActor const& GetOwningActor() const noexcept { check(this->IsOwningActorValid()) return *this->Owner; }

    NODISCARD constexpr EActorComponentOrigin GetOrigin() const noexcept { return this->Origin; }
    NODISCARD constexpr bool IsCtorComponent() const noexcept { return this->Origin == EActorComponentOrigin::Ctor; }
    NODISCARD constexpr bool IsRuntimeComponent() const noexcept { return this->Origin == EActorComponentOrigin::Runtime; }

private:

    AActor* Owner{};

protected:

    CLASS_FIELD(Identity)
    bool bTick:1{};

private:

    CLASS_FIELD(Identity)
    bool bRender:1{};

#if JAFG_DO_CHECKS
    bool bHasExecutedOnAttach:1{};
#endif /* JAFG_DO_CHECKS */

    EActorComponentOrigin Origin{ EActorComponentOrigin::Unknown };
};

} /* ~Namespace Jafg */
