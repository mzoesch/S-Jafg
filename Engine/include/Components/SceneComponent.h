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

    void AddTranslation(LWorldVec3 const& Location, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.T += Location; }
    void AddRotator(LWorldQuat const& Rotator, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.R = Rotator * this->Trans.R; }
    void AddScale(LWorldVec3 const& Scale, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.S += Scale; }

    void SetTranslation(LWorldVec3 const& Location, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.T = Location; }
    void SetRotator(LWorldQuat const& Rotator, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.R = Rotator; }
    void SetScale(LWorldVec3 const& Scale, ESceneSweep SweepType = ESceneSweep::Teleport) noexcept { check(SweepType == ESceneSweep::Teleport) this->Trans.S = Scale; }

    NODISCARD FORCEINLINE constexpr LWorldTrans const& GetTransform() const noexcept { return this->Trans; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetTranslation() const noexcept { return this->Trans.T; }
    NODISCARD FORCEINLINE constexpr LWorldQuat const& GetRotator() const noexcept { return this->Trans.R; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetScale() const noexcept { return this->Trans.S; }

    NODISCARD FORCEINLINE TArray<ASceneComponent*> const& GetChildren() const noexcept { return this->Children; }

private:

    LWorldTrans Trans{ maths::identity<LWorldTrans> };
    TArray<ASceneComponent*> Children;
};

} /* ~Namespace Jafg */
