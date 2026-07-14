// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "SceneComponent.generated.h"

namespace Jafg
{

enum struct ESceneSweep
{
    Teleport,
    //# TODO: Not implemented yet.
    Sweep,
    //# TODO: Not implemented yet.
    SweepComplex,
};

DECLARE_JAFG_CLASS()
class ENGINE_API ASceneComponent : public AActorComponent
{
    GENERATED_CLASS_BODY()

    friend AActor;

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASceneComponent)

public:

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    void SetTransform(const LWorldTrans& InTransform, const ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans = InTransform; }

    void AddTranslation(LWorldVec3 const& Location, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.t += Location; }
    void AddRotator(LWorldQuat const& Rotator, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.r = Rotator * this->Trans.r; }
    void AddScale(LWorldVec3 const& Scale, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.s += Scale; }

    void SetTranslation(LWorldVec3 const& Location, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.t = Location; }
    void SetRotator(LWorldQuat const& Rotator, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.r = Rotator; }
    void SetScale(LWorldVec3 const& Scale, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.s = Scale; }

    NODISCARD FORCEINLINE constexpr LWorldTrans const& GetTransform() const noexcept { return this->Trans; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetTranslation() const noexcept { return this->Trans.t; }
    NODISCARD FORCEINLINE constexpr LWorldQuat const& GetRotator() const noexcept { return this->Trans.r; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetScale() const noexcept { return this->Trans.s; }

    NODISCARD FORCEINLINE TArray<ASceneComponent*> const& GetChildren() const noexcept { return this->Children; }

    void SetAabb(LWorldAabb3 const& InAabb) noexcept { this->Aabb = InAabb; }
    NODISCARD FORCEINLINE constexpr LWorldAabb3 const& GetAabb() const noexcept { return this->Aabb; }

private:

    CLASS_FIELD(EditorVisible)
    LWorldTrans Trans{ maths::identity<LWorldTrans> };
    TArray<ASceneComponent*> Children;
    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };
};

} /* ~Namespace Jafg */
