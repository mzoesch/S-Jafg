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
class ENGINE_API ASceneComponent : public AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASceneComponent)

public:

    LWorldVec3 TempRot{ maths::zero_vector<LWorldVec3> };

    LWorldVec3 TempPos{ maths::zero_vector<LWorldVec3> };
    LWorldVec3 TempPos_jafg{ maths::zero_vector<LWorldVec3> };

    // TODO: We do not have physics yet, so we cannot do sweeps.
    void ChangeTransform(const LWorldTrans& InTransform, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans = InTransform; }
    void AddTranslation(const LWorldVec3& InLocation, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.T += InLocation; }
    void AddRotator(const LWorldQuat& InRotator, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.R += InRotator; }
    void AddScale(const LWorldVec3& InScale, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.S += InScale; }
    void SetTranslation(const LWorldVec3& InLocation, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.T = InLocation; }
    void SetRotator(const LWorldQuat& InRotator, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.R = InRotator; }
    void SetScale(const LWorldVec3& InScale, const LSceneSweep SweepType = LSceneSweep::Teleport) noexcept { this->Trans.S = InScale; }

    NODISCARD FORCEINLINE constexpr LWorldTrans const& GetTransform() const noexcept { return this->Trans; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetTranslation() const noexcept { return this->Trans.T; }
    NODISCARD FORCEINLINE constexpr LWorldQuat const& GetRotator() const noexcept { return this->Trans.R; }
    NODISCARD FORCEINLINE constexpr LWorldVec3 const& GetScale() const noexcept { return this->Trans.S; }

    struct LRelativeVectors
    {
        LWorldVec3 Front;
        LWorldVec3 Right;
        LWorldVec3 Up;
    };
    NODISCARD FORCEINLINE LRelativeVectors GetRelativeVectors() const noexcept
    {
        LRelativeVectors Out;

        Out.Front.x =
            maths::cos(maths::yaw(this->GetRotator()) * maths::cos(maths::pitch(this->GetRotator())));
        Out.Front.y =
            maths::sin(maths::yaw(this->GetRotator())) * maths::cos(maths::pitch(this->GetRotator()));
        Out.Front.z =
            maths::sin(maths::pitch(this->GetRotator()));
        Out.Front = maths::normalize(Out.Front);

        Out.Right = maths::normalize(maths::cross(Out.Front, maths::up_vector<LWorldVec3>)) * static_cast<decltype(Out.Up)::value_type>(-1.0);
        Out.Up    = maths::normalize(maths::cross(Out.Right, Out.Front)) * static_cast<decltype(Out.Up)::value_type>(-1.0);

        return Out;
    }
    NODISCARD FORCEINLINE LRelativeVectors GetRelativeVectors2() const noexcept
    {
        LRelativeVectors Out;

        Out.Front.x =
            maths::cos(maths::radians(this->TempRot.y)) * maths::cos(maths::radians(this->TempRot.x));
        Out.Front.y =
            maths::sin(maths::radians(this->TempRot.y)) * maths::cos(maths::radians(this->TempRot.x));
        Out.Front.z =
            maths::sin(maths::radians(this->TempRot.x));
        Out.Front = maths::normalize(Out.Front);

        Out.Right = maths::normalize(maths::cross(Out.Front, maths::up_vector<LWorldVec3>)) * static_cast<decltype(Out.Up)::value_type>(-1.0);
        Out.Up    = maths::normalize(maths::cross(Out.Right, Out.Front)) * static_cast<decltype(Out.Up)::value_type>(-1.0);

        return Out;
    }


private:

    LWorldTrans Trans{ maths::zero_trans<LWorldTrans> };
};

} /* ~Namespace Jafg */
