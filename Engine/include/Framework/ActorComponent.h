// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldObject.h"
#include "Framework/ActorComponentForward.h"
#include "ActorComponent.generated.h"

namespace Jafg
{

class AActor;
class ASceneComponent;
#if JAFG_WITH_EDITOR
    class WEditorWorldViewer;
#endif /* JAFG_WITH_EDITOR */

//#
//# The origin of the actor.
//#
enum struct EActorComponentOrigin: u8
{
    //#
    //# Before an origin could have been evaluated the component is in this state.
    //# Only very early on in a duration of a component's life.
    //#
    Unknown,

    //#
    //# The component is part of the ctor of its parent (other comp or actor).
    //# It is safe to assume to always use this component without checking. It is guaranteed to always be
    //# part of the actor or parent component.
    //#
    Ctor,

    //#
    //# The component was created because of specific runtime conditions. Generally it is not (depending on
    //# your context) safe to assume that this component is always attached to its parent or actor.
    //#
    //# During actor copying you generally in most cases should be held responsible for recursively coping
    //# runtime components.
    //#
    Runtime,
};

//#
//# An actor component is a composition of an actor.
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API AActorComponent : public AWorldObject
{
    GENERATED_CLASS_BODY()

    friend AActor;
    friend ASceneComponent;

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
    virtual void OnAttach(AActor& Owner)
    {
        check(this->Origin != EActorComponentOrigin::Unknown)
#if JAFG_DO_CHECKS
        check(this->bHasExecutedOnAttach == false)
        this->bHasExecutedOnAttach = true;
        check(this->Owner == nullptr)
#endif /* JAFG_DO_CHECKS */
        this->Owner = &Owner;
    }

    //#
    //# Called from the parent iff the parent is ticked and this component is allowed to tick.
    //# Generally speaking, if the parent is not ticked then one should *not* tick the components attached to it.
    //# If you really need to assert that your component is ticked whenever the owning outer is ticked then you
    //# can simply inherit from #LTickableObject.
    //#
    virtual void ParentTick(f32 Dt) { check(this->Owner && this->bTick) }

    //#
    //# Called from the parent iff the parent is physics ticked and this component is allowed to physics tick.
    //# Generally speaking, if the parent is not ticked then one should *not* tick the components attached to it.
    //# If you really need to assert that your component is ticked whenever the owning outer is ticked then you
    //# can simply inherit from #LFixedTickableObject.
    //#
    virtual void ParentPhysicsTick(f32 Dt, bool bFollowup) { check(this->Owner && this->bPhysicsTick) }

#if JAFG_WITH_EDITOR
    //#
    //# Called if this component is selected in the editor.
    //#
    virtual void SelectedTick(WEditorWorldViewer& Viewer, f32 Dt) {}
#endif /* JAFG_WITH_EDITOR */

    constexpr void SetShouldRender(bool b) noexcept { this->bRender = b; }
    NODISCARD constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LActorRenderInfo const& Info) const {}

    NODISCARD constexpr bool IsOwningActorValid() const noexcept { return this->Owner != nullptr; }
    NODISCARD constexpr AActor& GetOwningActor() noexcept { check(this->IsOwningActorValid()) return *this->Owner; }
    NODISCARD constexpr AActor const& GetOwningActor() const noexcept { check(this->IsOwningActorValid()) return *this->Owner; }

#if JAFG_DO_CHECKS
    NODISCARD FORCEINLINE constexpr bool _check_HasExecutedOnAttach() const noexcept { return this->bHasExecutedOnAttach; }
#endif /* JAFG_DO_CHECKS */

    NODISCARD constexpr EActorComponentOrigin GetOrigin() const noexcept { return this->Origin; }
    NODISCARD constexpr bool IsCtorComponent() const noexcept { return this->Origin == EActorComponentOrigin::Ctor; }
    NODISCARD constexpr bool IsRuntimeComponent() const noexcept { return this->Origin == EActorComponentOrigin::Runtime; }

private:

    AActor* Owner{};

protected:

    CLASS_FIELD(Identity)
    bool bTick:1{};
    CLASS_FIELD(Identity)
    bool bPhysicsTick:1{};

private:

    CLASS_FIELD(Identity)
    bool bRender:1{};

#if JAFG_DO_CHECKS
    bool bHasExecutedOnAttach:1{};
#endif /* JAFG_DO_CHECKS */

    EActorComponentOrigin Origin{ EActorComponentOrigin::Unknown };
};

} /* ~Namespace Jafg */
