// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Rhi/Skeleton.h"
#include "JgcCharacterAnimationComponent.generated.h"

namespace Jafg
{

class ACharacterMovementComponent;
class ASkeletalMeshComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

//# What a character does, as far as its animations are concerned.
enum struct ECharacterAnimationState: u8
{
    None,
    Idle,
    Walking,
    Running,
    Sprinting,
    CrouchIdle,
    CrouchWalking,
    Jumping,
    Falling,
    Flying,
    Floating,
    Seated,
};

NODISCARD JGC_API LStringView LexToString(ECharacterAnimationState State) noexcept;

//#
//# Animates the body of a character (its #Jafg::ASkeletalMeshComponent) from its #Jafg::ACharacterMovementComponent.
//#
//# Every tick, the state of the movement (mode, speed relative to the ground, crouching, ...) is mapped to a state and
//# the state to a clip, which is crossfaded in. Clips are found by name; every state has a list of names, the first
//# clip that the body has wins. States without any clip show the rest pose. The names follow the convention of
//# Documentation/Animation.md, e.g., "Locomotion_Running".
//#
//# Locomotion clips are played at the rate that matches the speed of the character (@see #RunningClipSpeed), and
//# backwards when the character moves backwards. The head follows the pitch of the view.
//#
//# Purely cosmetic: Reads the movement, never changes it.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcCharacterAnimationComponent: public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcCharacterAnimationComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;
    virtual void ParentTick(f32 Dt) override;

    NODISCARD FORCEINLINE constexpr ECharacterAnimationState GetState() const noexcept { return this->State; }

    //# Below this speed relative to the ground the character stands. In m/s.
    CLASS_FIELD(EditorEditable)
    f32 IdleSpeed{ 0.3f };
    //# From this speed on walking turns into running. In m/s.
    CLASS_FIELD(EditorEditable)
    f32 RunSpeed{ 2.6f };
    //# From this speed on running turns into sprinting. In m/s.
    CLASS_FIELD(EditorEditable)
    f32 SprintSpeed{ 5.5f };
    //# Speeds have to fall this much below a threshold to go back to the slower state. In [0, 1].
    CLASS_FIELD(EditorEditable)
    f32 SpeedHysteresis{ 0.1f };
    //# Upwards speed that counts as a jump when the ground is left. In m/s.
    CLASS_FIELD(EditorEditable)
    f32 JumpSpeed{ 1.0f };

    //# The speeds at which the locomotion clips play at their authored rate. In m/s.
    CLASS_FIELD(EditorEditable)
    f32 WalkingClipSpeed{ 1.6f };
    CLASS_FIELD(EditorEditable)
    f32 RunningClipSpeed{ 4.0f };
    CLASS_FIELD(EditorEditable)
    f32 SprintingClipSpeed{ 7.0f };
    CLASS_FIELD(EditorEditable)
    f32 CrouchWalkingClipSpeed{ 1.5f };
    //# Bounds of the rate that locomotion clips are played at.
    CLASS_FIELD(EditorEditable)
    f32 MinClipRate{ 0.4f };
    CLASS_FIELD(EditorEditable)
    f32 MaxClipRate{ 2.5f };

    //# How long states crossfade. In seconds.
    CLASS_FIELD(EditorEditable)
    f32 BlendTime{ 0.2f };

    //# How much of the pitch of the view the head follows. In [0, 1].
    CLASS_FIELD(EditorEditable)
    f32 HeadPitchFollow{ 0.8f };
    //# The joints that share the pitch of the head, from the neck up. Rigify names by default.
    TArray<LString> HeadJoints{ "DEF-spine.004", "DEF-spine.005", "DEF-spine.006" };

private:

    //# Finds the movement and the body of the owning actor. @return Whether both exist.
    bool ResolveComponents() noexcept;
    NODISCARD ECharacterAnimationState ComputeState(Jafg::ACharacterMovementComponent const& Movement, f32 Speed, f32 UpSpeed) const noexcept;
    //# The speed at which a clip plays at its authored rate. Empty for clips that do not depend on the speed.
    NODISCARD std::optional<f32> GetClipSpeed(LStringView Clip) const noexcept;
    void ApplyHeadPitch(Jafg::ASkeletalMeshComponent& Body, std::span<Jafg::LJointTransform> Pose) const;

    Jafg::ACharacterMovementComponent* Movement{};
    Jafg::ASkeletalMeshComponent* Body{};
    bool bModifierInstalled{};

    ECharacterAnimationState State{ ECharacterAnimationState::None };
    //# The clip that was started for #State. Empty: the rest pose.
    LString Clip;
    //# The mode of the movement in the previous tick. To detect jumps.
    bool bWasGrounded{};
};

} /* ~Namespace Jgc */
