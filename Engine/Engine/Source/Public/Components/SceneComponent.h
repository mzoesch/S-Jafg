// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "SceneComponent.generated.h"

namespace Jafg
{

enum struct LSceneSweep
{
    Teleport,
    Sweep,
    SweepComplex,
};

DECLARE_JAFG_CLASS()
class ENGINE_API JSceneComponent : public JActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSceneComponent)

public:

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

    struct LRelativeVectors
    {
        LVector3F Front;
        LVector3F Right;
        LVector3F Up;
    };
    NODISCARD FORCEINLINE LRelativeVectors GetRelativeVectors() const noexcept
    {
        LRelativeVectors Out;

        Out.Front.X =
            Maths::Cos(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
        Out.Front.Y =
            Maths::Sin(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
        Out.Front.Z =
            Maths::Sin(Maths::ToRadians(this->GetRotator().Pitch));
        Out.Front.Normalize();

        Out.Right = Out.Front.Cross(LVector::UpVector).NormalizeRet().InvertRet();
        Out.Up    = Out.Right.Cross(Out.Front).NormalizeRet().InvertRet();

        return Out;
    }

private:

    LTransform Transform;
};

} /* ~Namespace Jafg */
