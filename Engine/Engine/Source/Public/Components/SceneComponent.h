// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"

namespace Jafg
{

enum struct LSceneSweep
{
    Teleport,
    Sweep,
    SweepComplex,
};

class ENGINE_API LSceneComponent : public LActorComponent
{
public:

    constexpr LSceneComponent() noexcept = default;

    // TODO: We do not have physics yet, so we cannot do sweeps.
    void ChangeTransform(const LTransform& InTransform, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform = InTransform; }
    void AddTranslation(const LVector& InLocation, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Translation += InLocation; }
    void AddRotator(const LRotator& InRotator, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Rotator += InRotator; }
    void AddScale(const LVector& InScale, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Scale += InScale; }
    void SetTranslation(const LVector& InLocation, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Translation = InLocation; }
    void SetRotator(const LRotator& InRotator, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Rotator = InRotator; }
    void SetScale(const LVector& InScale, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Transform.Scale = InScale; }

    NODISCARD FORCEINLINE LTransform const& GetTransform() const noexcept { return this->Transform; }
    NODISCARD FORCEINLINE LVector const& GetTranslation() const noexcept { return this->Transform.Translation; }
    NODISCARD FORCEINLINE LRotator const& GetRotator() const noexcept { return this->Transform.Rotator; }
    NODISCARD FORCEINLINE LVector const& GetScale() const noexcept { return this->Transform.Scale; }

private:

    LTransform Transform;
};

} /* ~Namespace Jafg */
