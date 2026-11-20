// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "SceneComponent.generated.h"

namespace Jafg
{

class LPhysicsSystem;

enum struct ETransformChangeBits: u8
{
    Identity = 0x0,
    Translation = 0x1 << 0,
    Rotator = 0x1 << 1,
    Scale = 0x1 << 2,
    //# Whether the change was polled from a physics subsystem.
    PhysicsPoll = 0x1 << 3,
};
ENUM_STRUCT_FLAGS(ETransformChangeBits, ETransformChangeFlags)

//#
//# A scene component is an actor component that has a transform in the world and is recursively attached
//# to other actor components. The transforms are relative to the parent component.
//#
//# An actor may only have one scene component at its root.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API ASceneComponent: public AActorComponent
{
    GENERATED_CLASS_BODY()

    friend AActor;
    friend LWorld;

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASceneComponent)

public:

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    //# Also attaches the children.
    virtual void OnAttach(AActor& InOwner) override;

    //#
    //# Every scene component in the hierarchy of an actor that wants to tick is ticked by the actor (see
    //# #TickHierarchy), parents before children. Even if its parents do not tick.
    //#
    virtual void ParentTick(f32 Dt) override
    {
        Super::ParentTick(Dt);
    }

    //# Jafg internal method. Do not use. Ticks this component (if it wants to) and then its children.
    void _TickHierarchy(f32 Dt);

    virtual void Render(LActorRenderInfo const& Info) const override
    {
        for (auto const& Child: this->Children)
        {
            if (Child->ShouldRender())
            {
                Child->Render(Info);
            }
        }
    }

    //# The component of the same actor that owns this component. Null for root components.
    NODISCARD FORCEINLINE constexpr bool HasParent() const noexcept { return !!this->Parent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent* GetParent() noexcept { return this->Parent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent const* GetParent() const noexcept { return this->Parent; }

    //#
    //# Root components only. The component of another actor that the owning actor is attached to.
    //# @see #AActor::AttachTo
    //#
    NODISCARD FORCEINLINE constexpr bool HasAttachParent() const noexcept { return !!this->AttachParent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent* GetAttachParent() noexcept { return this->AttachParent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent const* GetAttachParent() const noexcept { return this->AttachParent; }
    //# The component that the local transform is relative to. Either the parent or the attach parent.
    NODISCARD FORCEINLINE constexpr ASceneComponent* GetTransformParent() noexcept { return this->Parent ? this->Parent : this->AttachParent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent const* GetTransformParent() const noexcept { return this->Parent ? this->Parent : this->AttachParent; }
    //# The actors that are attached to this component. They are not owned by this component.
    NODISCARD FORCEINLINE constexpr TArray<AActor*> const& GetAttachedActors() const noexcept { return this->AttachedActors; }

    //# Relative to the transform parent. See #GetTransformParent. Equal to the world transform if there is none.
    NODISCARD FORCEINLINE constexpr LWorldTrans const& GetLocalTransform() const noexcept { return this->LocalTransform; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetLocalTranslation() const noexcept { return this->LocalTransform.t; }
    NODISCARD FORCEINLINE constexpr LWorldQuat const& GetLocalRotator() const noexcept { return this->LocalTransform.r; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetLocalScale() const noexcept { return this->LocalTransform.s; }
    void SetLocalTransformByTeleport(LWorldTrans const& Transform) noexcept
    {
        this->LocalTransform = Transform;
        this->OnTransformChanged(ETransformChangeBits::Translation|ETransformChangeBits::Rotator|ETransformChangeBits::Scale);
    }
    void SetLocalTranslationByTeleport(LWorldVec3 const& Location) noexcept
    {
        this->LocalTransform.t = Location;
        this->OnTransformChanged(ETransformChangeBits::Translation);
    }
    void AddLocalTranslationByTeleport(LWorldVec3 const& Location) noexcept
    {
        this->LocalTransform.t += Location;
        this->OnTransformChanged(ETransformChangeBits::Translation);
    }
    void SetLocalRotatorByTeleport(LWorldQuat const& Rotator) noexcept
    {
        this->LocalTransform.r = Rotator;
        this->OnTransformChanged(ETransformChangeBits::Rotator);
    }
    void AddLocalRotatorByTeleport(LWorldQuat const& Rotator) noexcept
    {
        this->LocalTransform.r = Rotator * this->LocalTransform.r;
        this->OnTransformChanged(ETransformChangeBits::Rotator);
    }
    void SetLocalScaleByTeleport(LWorldVec3 const& Scale) noexcept
    {
        this->LocalTransform.s = Scale;
        this->OnTransformChanged(ETransformChangeBits::Scale);
    }
    void AddLocalScaleByTeleport(LWorldVec3 const& Scale) noexcept
    {
        this->LocalTransform.s += Scale;
        this->OnTransformChanged(ETransformChangeBits::Scale);
    }

    //#
    //# Relative to the world origin.
    //#
    //# We follow the standard TRS order, which is
    //#     Mw = Mp * Ml,
    //#     where
    //#         M represents the transformation matrix,
    //#         w the world space,
    //#         p the parent space, and
    //#         l the local space.
    //#
    //# In detail:
    //#     Tw = Tp + Rp(Sp * Tl)
    //#     Rw = Rp * Rt
    //#     Sw = Sp * St
    //#     where
    //#         T is the translation,
    //#         R the rotation, and
    //#         S the scale.
    //#
    //# @note We do not support non-uniform scales under rotation. The shear effect is not supported with these
    //#       primitives.
    //#       TODO: In the future we might want that, but as an extra component that uses a LWorldMatrix instead of
    //#             simple LWorldTrans. E.g. an AShearComponent: AActorComponent.
    //#
    NODISCARD LWorldTrans GetWorldTransformSlow() const noexcept
    {
        LWorldTrans Result{this->LocalTransform};
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            LWorldTrans P{TransformParent->GetWorldTransformSlow()};
            Result.t = P.r * (P.s * Result.t) + P.t;
            Result.r = P.r * Result.r;
            Result.s *= P.s;
        }
        return Result;
    }
    NODISCARD LWorldVec3 GetWorldTranslationSlow() const noexcept
    {
        LWorldVec3 Result{this->LocalTransform.t};
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            Result = TransformParent->GetWorldRotationSlow() * (TransformParent->GetWorldScaleSlow() * Result) + TransformParent->GetWorldTranslationSlow();
        }
        return Result;
    }
    NODISCARD LWorldQuat GetWorldRotationSlow() const noexcept
    {
        LWorldQuat Result{this->LocalTransform.r};
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            Result = TransformParent->GetWorldRotationSlow() * Result;
        }
        return Result;
    }
    NODISCARD LWorldVec3 GetWorldScaleSlow() const noexcept
    {
        LWorldVec3 Result{this->LocalTransform.s};
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            Result *= TransformParent->GetWorldScaleSlow();
        }
        return Result;
    }

    //# Converts a transform in world space into the local space of this component. See #GetTransformParent.
    NODISCARD LWorldTrans WorldToLocalTransformSlow(LWorldTrans const& World) const noexcept
    {
        auto const* TransformParent{this->GetTransformParent()};
        if (!TransformParent)
        {
            return World;
        }
        LWorldTrans const P{TransformParent->GetWorldTransformSlow()};
        LWorldQuat const InverseRotation{maths::inverse(P.r)};
        return LWorldTrans{
            .t = (InverseRotation * (World.t - P.t)) / P.s,
            .r = InverseRotation * World.r,
            .s = World.s / P.s,
            };
    }
    void SetWorldTransformByTeleport(LWorldTrans const& World) noexcept
    {
        this->SetLocalTransformByTeleport(this->WorldToLocalTransformSlow(World));
    }
    void SetWorldTranslationByTeleport(LWorldVec3 const& Location) noexcept
    {
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            LWorldTrans const P{TransformParent->GetWorldTransformSlow()};
            this->SetLocalTranslationByTeleport((maths::inverse(P.r) * (Location - P.t)) / P.s);
        }
        else
        {
            this->SetLocalTranslationByTeleport(Location);
        }
    }
    void SetWorldRotatorByTeleport(LWorldQuat const& Rotator) noexcept
    {
        if (auto const* TransformParent{this->GetTransformParent()})
        {
            this->SetLocalRotatorByTeleport(maths::inverse(TransformParent->GetWorldRotationSlow()) * Rotator);
        }
        else
        {
            this->SetLocalRotatorByTeleport(Rotator);
        }
    }

    //# The aabb for this component and all it child components.
    NODISCARD constexpr LWorldAabb3 GetTransitiveComponentAabb() const noexcept
    {
        LWorldAabb3 Result{ maths::identity<LWorldAabb3> };
        this->GetTransitiveComponentAabbImpl(Result);
        return Result;
    }
    NODISCARD constexpr LWorldAabb3 const& GetAabbForThisComponentOnly() const noexcept { return this->Aabb; }

    //#
    //# Traces the ray against this component only (not its children) for non-physical traces, e.g., clicks in the
    //# editor. @see #LWorld::LineTraceNonPhysical
    //# The default tests the aabb of this component. That is right for components without a geometry of their own
    //# (e.g., lights). Components with a geometry override this to trace it precisely.
    //# @return The parameter along the ray (in multiples of its direction) of the closest hit in
    //#         [0, Ray.magnitude]. Empty if missed.
    //#
    NODISCARD virtual std::optional<LWorldReal> TraceNonPhysical(LWorldMagRay3 const& Ray) const;

    NODISCARD FORCEINLINE TArray<TJxxUnique<ASceneComponent>> const& GetChildren() const noexcept { return this->Children; }

    //# Emplace a new default scene component to this component.
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultSceneComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Children.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Parent = this;
        Result.Origin = EActorComponentOrigin::Ctor;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->_check_HasExecutedOnAttach())
        check(!this->IsOwningActorValid())
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultSceneComponent(TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Children.emplace_back(SpawnObject(CastTo<ASceneComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Parent = this;
        Result.Origin = EActorComponentOrigin::Ctor;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->_check_HasExecutedOnAttach())
        check(!this->IsOwningActorValid())
        return Result;
    }

    //# Emplace a new scene component to this component.
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Children.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Parent = this;
        Result.Origin = EActorComponentOrigin::Runtime;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        if (this->IsOwningActorValid())
        {
            checkCode(this->_check_EmplaceRuntimeComponentPreCondition())
            Result.OnAttach(this->GetOwningActor());
        }
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponent(TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Children.emplace_back(SpawnObject(CastTo<ASceneComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Parent = this;
        Result.Origin = EActorComponentOrigin::Runtime;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        if (this->IsOwningActorValid())
        {
            checkCode(this->_check_EmplaceRuntimeComponentPreCondition())
            Result.OnAttach(this->GetOwningActor());
        }
        return Result;
    }

    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T* GetComponent() const noexcept
    {
        for (auto const& Comp: this->Children)
        {
            if (Comp->IsA<T>())
            {
                return StaticCastChecked<T>(&*Comp);
            }
        }
        return {};
    }
    template<typename T> requires std::is_base_of_v<ASceneComponent, T>
    NODISCARD FORCEINLINE T& GetComponentChecked() const noexcept
    {
        auto* Result{this->GetComponent<T>()};
        check(Result)
        return *Result;
    }

    template<typename T = ASceneComponent> requires std::is_base_of_v<ASceneComponent, T>
    NODISCARD FORCEINLINE T* GetComponent(TSubclassOf<T> Class) const noexcept
    {
        for (auto const& Comp: this->Children)
        {
            if (Comp->IsA(Class))
            {
                return StaticCastChecked<T>(&*Comp);
            }
        }
        return {};
    }
    template<typename T = ASceneComponent> requires std::is_base_of_v<ASceneComponent, T>
    NODISCARD FORCEINLINE T& GetComponentChecked(TSubclassOf<T> Class) const noexcept
    {
        auto* Result{this->GetComponent<T>(Class)};
        check(Result)
        return *Result;
    }

    //# Roots only. Whether the owning actor may be attached to another actor. @see #AActor::AttachTo
    NODISCARD virtual bool CanFollowAttachParent() const noexcept { return true; }

    NODISCARD LPhysicsSystem& GetParentPhysicsSystem() noexcept { return this->GetWorld().GetWorldGlobalPhysicsSystem(); }
    NODISCARD LPhysicsSystem const& GetParentPhysicsSystem() const noexcept { return this->GetWorld().GetWorldGlobalPhysicsSystem(); }

    //# Public private functions of jafg. Do not use.
    void _detail_OnPhysicsPoll(ETransformChangeFlags Change) { this->OnTransformChanged(Change|ETransformChangeBits::PhysicsPoll); }
    void _detail_OnTransformChanged(ETransformChangeFlags Change) { this->OnTransformChanged(Change); }

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) {}

    virtual AWorldObject& CloneImpl(AWorldObject* Object) const noexcept override;

    //# The aabb for this component only.
    CLASS_FIELD(Identity)
    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };

private:

#if JAFG_DO_CHECKS
    void _check_EmplaceRuntimeComponentPreCondition();
#endif /* JAFG_DO_CHECKS */

    //#
    //# Detach all actors that are attached to this component.
    //# @param bKeepWorld Whether they stay where they are. Otherwise, they are only unlinked (e.g., during tear down).
    //# @param bTransitive Whether to also detach the actors attached to the children of this component.
    //#
    void DetachAttachedActors(bool bKeepWorld, bool bTransitive);

    FORCEINLINE constexpr void GetTransitiveComponentAabbImpl(LWorldAabb3& Result) const noexcept
    {
        if (!this->Aabb.empty())
        {
            // TODO: Pass the parent transform here. so we not have to calculate the expensive world transform.
            Result.inline_merge(this->Aabb.apply(this->GetWorldTransformSlow()));
        }
        for (auto& Child: this->Children)
        {
            Child->GetTransitiveComponentAabbImpl(Result);
        }
    }

    CLASS_FIELD(EditorEditable)
    LWorldTrans LocalTransform{ maths::identity<LWorldTrans> };
    ASceneComponent* Parent{};
    TArray<TJxxUnique<ASceneComponent>> Children;

    //# Roots only. Not owning. @see #AActor::AttachTo
    ASceneComponent* AttachParent{};
    //# Not owning. The roots of these actors have this component as their #AttachParent.
    TArray<AActor*> AttachedActors;
};

} /* ~Namespace Jafg */
