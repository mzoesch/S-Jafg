// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Skeleton.h"

namespace Jafg
{

enum struct EAnimationPath: u8
{
    Translation,
    Rotation,
    Scale,
};

//# As glTF. @see https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#interpolation
enum struct EAnimationInterpolation: u8
{
    Step,
    Linear,
    //# Every key has an in-tangent, a value and an out-tangent (in this order).
    CubicSpline,
};

//# The keys of one property (translation, rotation or scale) of one joint.
struct LAnimationChannel final
{
    //# The name of the animated joint. Clips are bound to skeletons by joint names. @see #LAnimationClipBinding
    LString Target;
    EAnimationPath Path{ EAnimationPath::Rotation };
    EAnimationInterpolation Interpolation{ EAnimationInterpolation::Linear };
    //# In seconds, ascending, relative to the start of the clip.
    TArray<f32> Times;
    //#
    //# One value per key (three for #EAnimationInterpolation::CubicSpline). Translations and scales use xyz,
    //# rotations are quaternions as (x, y, z, w).
    //#
    TArray<LVec4F> Values;

    //# The value at #Time. Clamped to the first and last key.
    NODISCARD ENGINE_API LVec4F Sample(f32 Time) const noexcept;
};

//#
//# A named animation (a glTF animation). Independent of any skeleton: Its channels target joints by name, so that
//# one clip can drive every skeleton that has joints with these names.
//#
struct LAnimationClip final
{
    LString Name;
    //# In seconds. The time of the last key.
    f32 Duration{};
    TArray<LAnimationChannel> Channels;
};
typedef std::shared_ptr<LAnimationClip const> LAnimationClipRef;

//# The channels of a clip resolved against one skeleton.
struct LAnimationClipBinding final
{
    struct LEntry final
    {
        u32 Channel;
        u32 Joint;
    };
    TArray<LEntry> Entries;
    //# Channels whose target is not part of the skeleton. They are ignored.
    u32 UnboundChannels{};

    NODISCARD ENGINE_API static LAnimationClipBinding Bind(LAnimationClip const& Clip, LSkeleton const& Skeleton);
};

//#
//# Writes the transforms of all bound joints at #Time into #Pose. Joints without channels (or properties without a
//# channel) keep their values. #Time is clamped to the clip.
//#
ENGINE_API void SampleClip(LAnimationClip const& Clip, LAnimationClipBinding const& Binding, f32 Time, std::span<LJointTransform> Pose) noexcept;

//# All clips of one file.
struct LAnimationLibrary final
{
    //# For diagnostics.
    LString Path;
    TArray<LAnimationClipRef> Clips;

    //# Null if there is no clip with this name.
    NODISCARD ENGINE_API LAnimationClipRef FindClip(LStringView Name) const noexcept;
};
typedef std::shared_ptr<LAnimationLibrary const> LAnimationLibraryRef;

struct LAnimationPlayParams final
{
    //# How long the previous clips fade out. In seconds. Zero cuts.
    f32 BlendTime{ 0.2f };
    //# Playback speed. Negative plays backwards.
    f32 Rate{ 1.0f };
    bool bLoop{ true };
    //# Where to start in seconds. Empty: At the beginning (or the end, if the rate is negative).
    std::optional<f32> StartTime;
    //# Whether to restart the clip if it is already the current one. Otherwise only the rate and looping change.
    bool bRestart{};
};

//#
//# Plays clips on a skeleton and crossfades between them. Each #Play fades the new clip in over the previous ones.
//# Owns no pose: #Evaluate writes into the caller's pose. Not bound to a skeleton itself; the bindings are resolved
//# through a callback, so that they can be cached by the caller.
//#
class LAnimationPlayer final
{
public:

    typedef TCopyableFunction<LAnimationClipBinding const&(LAnimationClip const& Clip)> LResolveBinding;

    //# Fades #Clip in. A null clip fades to the rest pose.
    ENGINE_API void Play(LAnimationClipRef Clip, LAnimationPlayParams const& Params = {});
    //# Fades to the rest pose.
    void Stop(f32 BlendTime = 0.2f) { this->Play(nullptr, {.BlendTime = BlendTime}); }
    //# Removes all clips instantly. The pose is the rest pose afterward.
    void Reset() noexcept { this->Layers.clear(); }

    //# Advances the time of all clips and the fades.
    ENGINE_API void Advance(f32 Dt) noexcept;
    //#
    //# The blended local pose. Starts from the rest pose of the skeleton.
    //# @param Out Resized to the joint count of #Skeleton.
    //#
    ENGINE_API void Evaluate(LSkeleton const& Skeleton, LResolveBinding const& ResolveBinding, TArray<LJointTransform>& Out) const;

    //# The clip that was played last. Null if nothing plays or the player fades to the rest pose.
    NODISCARD ENGINE_API LAnimationClip const* GetCurrentClip() const noexcept;
    //# The time of the current clip in seconds. Zero if there is none.
    NODISCARD ENGINE_API f32 GetCurrentTime() const noexcept;
    //# Change the rate of the current clip without restarting it.
    ENGINE_API void SetCurrentRate(f32 Rate) noexcept;
    //# Whether the current clip does not loop and reached its end.
    NODISCARD ENGINE_API bool HasCurrentClipFinished() const noexcept;
    NODISCARD FORCEINLINE bool IsPlaying() const noexcept { return !this->Layers.empty(); }

private:

    struct LLayer final
    {
        //# Null: The rest pose.
        LAnimationClipRef Clip;
        f32 Time{};
        f32 Rate{ 1.0f };
        bool bLoop{ true };
        //# How much this layer covers the layers below it. In [0, 1].
        f32 Weight{ 1.0f };
        //# Weight per second while fading in.
        f32 FadeSpeed{};
    };

    //# Oldest first. The last one is the current one.
    TArray<LLayer> Layers;
    //# Scratch pose for #Evaluate.
    mutable TArray<LJointTransform> Scratch;
};

} /* ~Namespace Jafg */
