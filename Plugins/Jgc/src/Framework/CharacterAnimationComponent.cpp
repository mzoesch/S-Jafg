// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/CharacterAnimationComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/SkeletalMeshComponent.h"
#include "Framework/MovementTypes.h"
#include "Framework/Actor.h"

namespace
{

using EState = Jgc::ECharacterAnimationState;

constexpr LStringView IdleClip{ "Locomotion_Idle" };
constexpr LStringView WalkingClip{ "Locomotion_Walking" };
constexpr LStringView RunningClip{ "Locomotion_Running" };
constexpr LStringView SprintingClip{ "Locomotion_Sprinting" };
constexpr LStringView CrouchIdleClip{ "Locomotion_CrouchIdle" };
constexpr LStringView CrouchWalkingClip{ "Locomotion_CrouchWalking" };
constexpr LStringView JumpingClip{ "Air_Jumping" };
//# A jump from standing. Part of the locomotion set.
constexpr LStringView IdleJumpingClip{ "Locomotion_IdleJumping" };
constexpr LStringView FallingClip{ "Air_Falling" };
//# Falling without moving much. Part of the locomotion set.
constexpr LStringView IdleFallingClip{ "Locomotion_IdleFalling" };
constexpr LStringView FlyingClip{ "Air_Flying" };
constexpr LStringView FloatingClip{ "ZeroG_Floating" };
constexpr LStringView SeatedClip{ "Vehicle_Seated" };

//# The clips that show a state, the best one first. The first one that the body has is played.
NODISCARD std::span<LStringView const> GetClipCandidates(EState State) noexcept
{
    static constexpr std::array Idle{IdleClip};
    static constexpr std::array Walking{WalkingClip, RunningClip};
    static constexpr std::array Running{RunningClip, WalkingClip};
    static constexpr std::array Sprinting{SprintingClip, RunningClip, WalkingClip};
    static constexpr std::array CrouchIdle{CrouchIdleClip, IdleClip};
    static constexpr std::array CrouchWalking{CrouchWalkingClip, WalkingClip, RunningClip};
    static constexpr std::array Jumping{JumpingClip, IdleJumpingClip, FallingClip, IdleFallingClip, IdleClip};
    static constexpr std::array Falling{FallingClip, IdleFallingClip, IdleClip};
    static constexpr std::array Flying{FlyingClip, FallingClip, IdleFallingClip, IdleClip};
    static constexpr std::array Floating{FloatingClip, FallingClip, IdleFallingClip, IdleClip};
    static constexpr std::array Seated{SeatedClip, IdleClip};

    switch (State)
    {
    case EState::None:          return {};
    case EState::Idle:          return Idle;
    case EState::Walking:       return Walking;
    case EState::Running:       return Running;
    case EState::Sprinting:     return Sprinting;
    case EState::CrouchIdle:    return CrouchIdle;
    case EState::CrouchWalking: return CrouchWalking;
    case EState::Jumping:       return Jumping;
    case EState::Falling:       return Falling;
    case EState::Flying:        return Flying;
    case EState::Floating:      return Floating;
    case EState::Seated:        return Seated;
    }
    std::unreachable();
}

//# The first skeletal mesh in the hierarchy below #Root. Depth-first.
NODISCARD Jafg::ASkeletalMeshComponent* FindBody(Jafg::ASceneComponent& Root) noexcept
{
    if (auto* Body{Root.As<Jafg::ASkeletalMeshComponent>()})
    {
        return Body;
    }
    for (auto const& Child: Root.GetChildren())
    {
        if (auto* Body{::FindBody(*Child)})
        {
            return Body;
        }
    }
    return nullptr;
}

} /* ~Namespace <Anonymous> */

LStringView Jgc::LexToString(ECharacterAnimationState State) noexcept
{
    switch (State)
    {
    case ECharacterAnimationState::None:          return "None";
    case ECharacterAnimationState::Idle:          return "Idle";
    case ECharacterAnimationState::Walking:       return "Walking";
    case ECharacterAnimationState::Running:       return "Running";
    case ECharacterAnimationState::Sprinting:     return "Sprinting";
    case ECharacterAnimationState::CrouchIdle:    return "CrouchIdle";
    case ECharacterAnimationState::CrouchWalking: return "CrouchWalking";
    case ECharacterAnimationState::Jumping:       return "Jumping";
    case ECharacterAnimationState::Falling:       return "Falling";
    case ECharacterAnimationState::Flying:        return "Flying";
    case ECharacterAnimationState::Floating:      return "Floating";
    case ECharacterAnimationState::Seated:        return "Seated";
    }
    std::unreachable();
}

void Jgc::ACharacterAnimationComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    /* The modifier refers to this component. */
    if (this->bModifierInstalled && this->Body && Jafg::IsValidFast(this->GetOuter(), this->Body))
    {
        this->Body->SetPoseModifier({});
    }
    this->bModifierInstalled = false;
    Super::OnGarbage(Reason);
}

void Jgc::ACharacterAnimationComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    if (!this->ResolveComponents())
    {
        return;
    }
    Jafg::ACharacterMovementComponent const& CharacterMovement{*this->Movement};
    Jafg::ASkeletalMeshComponent& CharacterBody{*this->Body};

    /* Relative to the ground, so that standing on a moving ship is standing. */
    LVec3F const Up{CharacterMovement.GetUp()};
    LVec3F const Relative{CharacterMovement.GetVelocity() - CharacterMovement.GetGroundVelocity()};
    f32 const UpSpeed{glm::dot(Relative, Up)};
    LVec3F const Planar{Relative - Up * UpSpeed};
    f32 const Speed{glm::length(Planar)};

    ECharacterAnimationState const NewState{this->ComputeState(CharacterMovement, Speed, UpSpeed)};
    this->bWasGrounded = Jafg::IsGroundedMovementMode(CharacterMovement.GetMovementMode()) && !CharacterMovement.IsRootAttached();

    LStringView Chosen;
    for (LStringView const Candidate: ::GetClipCandidates(NewState))
    {
        if (CharacterBody.HasClip(Candidate))
        {
            Chosen = Candidate;
            break;
        }
    }

    f32 Rate{1.0f};
    if (std::optional<f32> const ClipSpeed{this->GetClipSpeed(Chosen)}; ClipSpeed && *ClipSpeed > 0.0f)
    {
        Rate = maths::clamp(Speed / *ClipSpeed, this->MinClipRate, this->MaxClipRate);
        /* Backwards. A cheap stand-in until there are clips for it. */
        LVec3F const Forward{CharacterMovement.GetBodyRotation() * maths::forward_vector<LVec3F>};
        if (glm::dot(Planar, Forward) < -0.5f * Speed)
        {
            Rate = -Rate;
        }
    }

    if (NewState != this->State || Chosen != this->Clip)
    {
        LOG_TRACE(LogJgcAnimation, "[{}]: [{}] -> [{}] with clip [{}]."
            , this->GetNameAsString()
            , LexToString(this->State)
            , LexToString(NewState)
            , Chosen.empty() ? "<rest pose>"sv : Chosen
            )

        this->State = NewState;
        this->Clip = LString{Chosen};
        if (Chosen.empty())
        {
            CharacterBody.Stop(this->BlendTime);
        }
        else
        {
            bool const bJumping{NewState == ECharacterAnimationState::Jumping};
            CharacterBody.Play(Chosen, {
                .BlendTime = this->BlendTime,
                .Rate = Rate,
                .bLoop = !bJumping,
                .StartTime = bJumping ? std::optional<f32>{maths::max(this->JumpingClipStartTime, 0.0f)} : std::nullopt,
                .bRestart = bJumping,
                });
        }
    }
    else if (!Chosen.empty())
    {
        CharacterBody.GetPlayer().SetCurrentRate(Rate);
    }
}

bool Jgc::ACharacterAnimationComponent::ResolveComponents() noexcept
{
    auto& Outer{this->GetOuter()};
    if (!this->Movement || !Jafg::IsValidFast(Outer, this->Movement))
    {
        this->Movement = this->GetOwningActor().GetComponent<Jafg::ACharacterMovementComponent>();
    }
    if (!this->Body || !Jafg::IsValidFast(Outer, this->Body))
    {
        this->Body = this->GetOwningActor().HasRootComponent() ? ::FindBody(this->GetOwningActor().GetRootComponent()) : nullptr;
        this->bModifierInstalled = false;
        this->State = ECharacterAnimationState::None;
        this->Clip.clear();
    }
    if (!this->Movement || !this->Body)
    {
        return false;
    }

    if (!this->bModifierInstalled)
    {
        this->Body->SetPoseModifier([this](Jafg::ASkeletalMeshComponent& Comp, std::span<Jafg::LJointTransform> Pose)
        {
            this->ApplyHeadPitch(Comp, Pose);
        });
        this->bModifierInstalled = true;
    }
    return true;
}

Jgc::ECharacterAnimationState Jgc::ACharacterAnimationComponent::ComputeState(
      Jafg::ACharacterMovementComponent const& CharacterMovement
    , f32 Speed
    , f32 UpSpeed
    ) const noexcept
{
    Jafg::EMovementMode const Mode{CharacterMovement.GetMovementMode()};
    if (CharacterMovement.IsRootAttached() || Mode == Jafg::EMovementMode::None)
    {
        return ECharacterAnimationState::Seated;
    }

    switch (Mode)
    {
    case Jafg::EMovementMode::ZeroG:
    {
        return ECharacterAnimationState::Floating;
    }
    case Jafg::EMovementMode::Flying:
    {
        return ECharacterAnimationState::Flying;
    }
    case Jafg::EMovementMode::Falling:
    {
        if (this->State == ECharacterAnimationState::Jumping)
        {
            /* Until the jump is played or the character starts to come down. */
            bool const bFinished{this->Body && this->Body->GetPlayer().HasCurrentClipFinished()};
            return UpSpeed > 0.0f && !bFinished ? ECharacterAnimationState::Jumping : ECharacterAnimationState::Falling;
        }
        return this->bWasGrounded && UpSpeed > this->JumpSpeed ? ECharacterAnimationState::Jumping : ECharacterAnimationState::Falling;
    }
    default:
    {
        /* Grounded (walking or magnetic boots). */
        break;
    }
    }

    /* How fast, as a rank. The thresholds of ranks that are reached already are lowered a bit, so that the state does not flicker. */
    int const CurrentRank{[](ECharacterAnimationState Current) -> int
    {
        switch (Current)
        {
        case ECharacterAnimationState::Walking:
        case ECharacterAnimationState::CrouchWalking:
            return 1;
        case ECharacterAnimationState::Running:
            return 2;
        case ECharacterAnimationState::Sprinting:
            return 3;
        default:
            return 0;
        }
    }(this->State)};
    std::array const Thresholds{this->IdleSpeed, this->RunSpeed, this->SprintSpeed};
    int Rank{0};
    for (int Idx{0}; Idx < static_cast<int>(Thresholds.size()); ++Idx)
    {
        f32 const Threshold{Thresholds[static_cast<std::size_t>(Idx)] * (CurrentRank > Idx ? 1.0f - this->SpeedHysteresis : 1.0f)};
        if (Speed < Threshold)
        {
            break;
        }
        Rank = Idx + 1;
    }

    if (CharacterMovement.IsCrouched())
    {
        return Rank == 0 ? ECharacterAnimationState::CrouchIdle : ECharacterAnimationState::CrouchWalking;
    }
    static constexpr std::array ByRank{
        ECharacterAnimationState::Idle,
        ECharacterAnimationState::Walking,
        ECharacterAnimationState::Running,
        ECharacterAnimationState::Sprinting,
        };
    return ByRank[static_cast<std::size_t>(Rank)];
}

std::optional<f32> Jgc::ACharacterAnimationComponent::GetClipSpeed(LStringView ClipName) const noexcept
{
    if (ClipName == WalkingClip)
    {
        return this->WalkingClipSpeed;
    }
    if (ClipName == RunningClip)
    {
        return this->RunningClipSpeed;
    }
    if (ClipName == SprintingClip)
    {
        return this->SprintingClipSpeed;
    }
    if (ClipName == CrouchWalkingClip)
    {
        return this->CrouchWalkingClipSpeed;
    }
    return {};
}

void Jgc::ACharacterAnimationComponent::ApplyHeadPitch(Jafg::ASkeletalMeshComponent& Comp, std::span<Jafg::LJointTransform> Pose) const
{
    if (this->HeadPitchFollow == 0.0f || !this->Movement || !Jafg::IsValidFast(this->GetOuter(), this->Movement))
    {
        return;
    }
    Jafg::LSkeleton const* Skeleton{Comp.GetSkeleton()};
    if (!Skeleton)
    {
        return;
    }

    std::array<u32, 8> Joints{};
    std::size_t JointCount{};
    for (LString const& Name: this->HeadJoints)
    {
        if (auto const Joint{Skeleton->FindJoint(Name)}; Joint && JointCount < Joints.size())
        {
            Joints[JointCount++] = *Joint;
        }
    }
    if (JointCount == 0)
    {
        return;
    }

    /* The pitch is shared by all joints of the neck. The space of the body is the space of the mesh. */
    f32 const Pitch{this->Movement->GetViewPitch() * this->HeadPitchFollow / static_cast<f32>(JointCount)};
    LQuatF const Step{Jafg::Movement::PitchToRotation(Pitch)};
    for (std::size_t Idx{0}; Idx < JointCount; ++Idx)
    {
        Skeleton->RotateInModelSpace(Pose, Joints[Idx], Step);
    }
}
