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

DECLARE_JAFG_CLASS()
class ENGINE_API ASceneComponent : public AActorComponent
{
    GENERATED_CLASS_BODY()

    friend AActor;
    friend LWorld;

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASceneComponent)

public:

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

#if JAFG_DO_CHECKS
    virtual void OnAttach(AActor& InOwner) override;
#endif /* JAFG_DO_CHECKS */

    virtual void ParentTick(f32 Dt) override
    {
        Super::ParentTick(Dt);
        for (auto& Child: this->Children)
        {
            if (Child->bTick)
            {
                Child->ParentTick(Dt);
            }
        }
    }

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

    NODISCARD FORCEINLINE constexpr bool HasParent() const noexcept { return !!this->Parent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent* GetParent() noexcept { return this->Parent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent const* GetParent() const noexcept { return this->Parent; }

    //# Relative to the parent actor or component.
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
        if (this->Parent)
        {
            LWorldTrans P{this->Parent->GetWorldTransformSlow()};
            Result.t = P.r * (P.s * Result.t) + P.t;
            Result.r = P.r * Result.r;
            Result.s *= P.s;
        }
        return Result;
    }
    NODISCARD LWorldVec3 GetWorldTranslationSlow() const noexcept
    {
        LWorldVec3 Result{this->LocalTransform.t};
        if (this->Parent)
        {
            Result = this->Parent->GetWorldRotationSlow() * (this->Parent->GetWorldScaleSlow() * Result) + this->Parent->GetWorldTranslationSlow();
        }
        return Result;
    }
    NODISCARD LWorldQuat GetWorldRotationSlow() const noexcept
    {
        LWorldQuat Result{this->LocalTransform.r};
        if (this->Parent)
        {
            Result = this->Parent->GetWorldRotationSlow() * Result;
        }
        return Result;
    }
    NODISCARD LWorldVec3 GetWorldScaleSlow() const noexcept
    {
        LWorldVec3 Result{this->LocalTransform.s};
        if (this->Parent)
        {
            Result *= this->Parent->GetWorldScaleSlow();
        }
        return Result;
    }

    //# The aabb for this component and all it child components.
    NODISCARD constexpr LWorldAabb3 GetTransitiveComponentAabb() const noexcept
    {
        LWorldAabb3 Result{ maths::identity<LWorldAabb3> };
        this->GetTransitiveComponentAabbImpl(Result);
        return Result;
    }
    NODISCARD constexpr LWorldAabb3 GetAabbForThisComponentOnly() const noexcept { return this->Aabb; }

    NODISCARD FORCEINLINE TArray<TJxxUnique<ASceneComponent>> const& GetChildren() const noexcept { return this->Children; }

    NODISCARD LPhysicsSystem& GetParentPhysicsSystem() noexcept { return this->GetWorld().GetWorldGlobalPhysicsSystem(); }
    NODISCARD LPhysicsSystem const& GetParentPhysicsSystem() const noexcept { return this->GetWorld().GetWorldGlobalPhysicsSystem(); }

    //# Public private function of jafg. Do not use.
    void _detail_OnPhysicsPoll(ETransformChangeFlags Change) { this->OnTransformChanged(Change|ETransformChangeBits::PhysicsPoll); }

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) {}

    virtual AWorldObject& CloneImpl(AWorldObject* Object) const noexcept override;

    //# The aabb for this component only.
    CLASS_FIELD(Identity)
    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };

private:

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

    CLASS_FIELD(EditorVisible)
    LWorldTrans LocalTransform{ maths::identity<LWorldTrans> };
    ASceneComponent* Parent{};
    TArray<TJxxUnique<ASceneComponent>> Children;
};

} /* ~Namespace Jafg */
