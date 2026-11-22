// Copyright mzoesch. All rights reserved.

#include "Rhi/Animation.h"

namespace
{

NODISCARD FORCEINLINE LQuatF ToQuat(LVec4F const& V) noexcept
{
    return LQuatF::wxyz(V.w, V.x, V.y, V.z);
}

NODISCARD FORCEINLINE LVec4F FromQuat(LQuatF const& Q) noexcept
{
    return LVec4F{Q.x, Q.y, Q.z, Q.w};
}

//# More than this many clips fading at once are collapsed. Only reached when the clip changes faster than it fades.
constexpr std::size_t MaxLayers{ 8 };

} /* ~Namespace <Anonymous> */

LVec4F Jafg::LAnimationChannel::Sample(f32 Time) const noexcept
{
    check(!this->Times.empty())

    bool const bCubic{this->Interpolation == EAnimationInterpolation::CubicSpline};
    std::size_t const Stride{bCubic ? 3uz : 1uz};
    auto const Value{[this, Stride, bCubic](std::size_t Key) -> LVec4F const&
    {
        return this->Values[Key * Stride + (bCubic ? 1uz : 0uz)];
    }};

    std::size_t const Count{this->Times.size()};
    if (Count == 1 || Time <= this->Times.front())
    {
        return Value(0);
    }
    if (Time >= this->Times.back())
    {
        return Value(Count - 1);
    }

    /* The key at or before #Time. Not the last one, as #Time is before the last key. */
    std::size_t const Key{static_cast<std::size_t>(std::upper_bound(this->Times.begin(), this->Times.end(), Time) - this->Times.begin()) - 1uz};
    check(Key + 1 < Count)

    f32 const T0{this->Times[Key]};
    f32 const Span{this->Times[Key + 1] - T0};
    f32 const U{Span > 0.0f ? (Time - T0) / Span : 0.0f};
    bool const bRotation{this->Path == EAnimationPath::Rotation};

    switch (this->Interpolation)
    {
    case EAnimationInterpolation::Step:
    {
        return Value(Key);
    }
    case EAnimationInterpolation::Linear:
    {
        if (bRotation)
        {
            /* Takes the shortest path. */
            return ::FromQuat(glm::slerp(::ToQuat(Value(Key)), ::ToQuat(Value(Key + 1)), U));
        }
        return glm::mix(Value(Key), Value(Key + 1), U);
    }
    case EAnimationInterpolation::CubicSpline:
    {
        /* Hermite spline. The tangents are scaled by the duration of the segment (glTF 2.0, Appendix C). */
        LVec4F const P0{Value(Key)};
        LVec4F const M0{this->Values[Key * 3 + 2] * Span};
        LVec4F const P1{Value(Key + 1)};
        LVec4F const M1{this->Values[(Key + 1) * 3 + 0] * Span};

        f32 const U2{U * U};
        f32 const U3{U2 * U};
        LVec4F Result{
              P0 * (2.0f * U3 - 3.0f * U2 + 1.0f)
            + M0 * (U3 - 2.0f * U2 + U)
            + P1 * (-2.0f * U3 + 3.0f * U2)
            + M1 * (U3 - U2)
            };
        if (bRotation)
        {
            f32 const Length{glm::length(Result)};
            Result = Length > 1.0e-8f ? Result / Length : P0;
        }
        return Result;
    }
    }

    std::unreachable();
}

Jafg::LAnimationClipBinding Jafg::LAnimationClipBinding::Bind(LAnimationClip const& Clip, LSkeleton const& Skeleton)
{
    LAnimationClipBinding Result;
    Result.Entries.reserve(Clip.Channels.size());
    for (auto Idx{0uz}; Idx < Clip.Channels.size(); ++Idx)
    {
        if (auto const Joint{Skeleton.FindJoint(Clip.Channels[Idx].Target)})
        {
            Result.Entries.push_back({.Channel = static_cast<u32>(Idx), .Joint = *Joint});
        }
        else
        {
            ++Result.UnboundChannels;
        }
    }
    return Result;
}

void Jafg::SampleClip(LAnimationClip const& Clip, LAnimationClipBinding const& Binding, f32 Time, std::span<LJointTransform> Pose) noexcept
{
    f32 const Clamped{maths::clamp(Time, 0.0f, Clip.Duration)};
    for (auto const& Entry: Binding.Entries)
    {
        check(Entry.Channel < Clip.Channels.size())
        check(Entry.Joint < Pose.size())

        LAnimationChannel const& Channel{Clip.Channels[Entry.Channel]};
        LVec4F const Value{Channel.Sample(Clamped)};
        LJointTransform& Joint{Pose[Entry.Joint]};
        switch (Channel.Path)
        {
        case EAnimationPath::Translation:
        {
            Joint.T = LVec3F{Value};
            break;
        }
        case EAnimationPath::Rotation:
        {
            Joint.R = glm::normalize(::ToQuat(Value));
            break;
        }
        case EAnimationPath::Scale:
        {
            Joint.S = LVec3F{Value};
            break;
        }
        }
    }
}

Jafg::LAnimationClipRef Jafg::LAnimationLibrary::FindClip(LStringView Name) const noexcept
{
    for (auto const& Clip: this->Clips)
    {
        if (Clip->Name == Name)
        {
            return Clip;
        }
    }
    return nullptr;
}

void Jafg::LAnimationPlayer::Play(LAnimationClipRef Clip, LAnimationPlayParams const& Params /* = {} */)
{
    if (!this->Layers.empty())
    {
        LLayer& Current{this->Layers.back()};
        if (Current.Clip == Clip && !Params.bRestart)
        {
            Current.Rate = Params.Rate;
            Current.bLoop = Params.bLoop;
            return;
        }
    }
    else if (!Clip)
    {
        /* Already in the rest pose. */
        return;
    }

    f32 const Duration{Clip ? Clip->Duration : 0.0f};
    bool const bFade{Params.BlendTime > 0.0f && !this->Layers.empty()};
    this->Layers.push_back(LLayer{
        .Clip = std::move(Clip),
        .Time = maths::clamp(Params.StartTime.value_or(Params.Rate < 0.0f ? Duration : 0.0f), 0.0f, Duration),
        .Rate = Params.Rate,
        .bLoop = Params.bLoop,
        .Weight = bFade ? 0.0f : 1.0f,
        .FadeSpeed = bFade ? 1.0f / Params.BlendTime : 0.0f,
        });

    if (!bFade)
    {
        /* Covers everything below it at once. */
        this->Layers.erase(this->Layers.begin(), this->Layers.end() - 1);
    }
    else if (this->Layers.size() > ::MaxLayers)
    {
        this->Layers.erase(this->Layers.begin());
        this->Layers.front().Weight = 1.0f;
    }
}

void Jafg::LAnimationPlayer::Advance(f32 Dt) noexcept
{
    for (LLayer& Layer: this->Layers)
    {
        if (Layer.Clip)
        {
            f32 const Duration{Layer.Clip->Duration};
            Layer.Time += Dt * Layer.Rate;
            if (Layer.bLoop && Duration > 0.0f)
            {
                Layer.Time = std::fmod(Layer.Time, Duration);
                if (Layer.Time < 0.0f)
                {
                    Layer.Time += Duration;
                }
            }
            else
            {
                Layer.Time = maths::clamp(Layer.Time, 0.0f, Duration);
            }
        }

        if (Layer.Weight < 1.0f)
        {
            Layer.Weight = Layer.FadeSpeed > 0.0f ? maths::min(1.0f, Layer.Weight + Layer.FadeSpeed * Dt) : 1.0f;
        }
    }

    /* Layers below a layer that covers them completely are no longer visible. */
    for (auto Idx{this->Layers.size()}; Idx-- > 1uz;)
    {
        if (this->Layers[Idx].Weight >= 1.0f)
        {
            this->Layers.erase(this->Layers.begin(), this->Layers.begin() + static_cast<std::ptrdiff_t>(Idx));
            break;
        }
    }
}

void Jafg::LAnimationPlayer::Evaluate(LSkeleton const& Skeleton, LResolveBinding const& ResolveBinding, TArray<LJointTransform>& Out) const
{
    Skeleton.GetRestPose(Out);

    for (auto Idx{0uz}; Idx < this->Layers.size(); ++Idx)
    {
        LLayer const& Layer{this->Layers[Idx]};

        if (Idx == 0 && Layer.Weight >= 1.0f)
        {
            /* Nothing below. Sample straight into the result. */
            if (Layer.Clip)
            {
                SampleClip(*Layer.Clip, ResolveBinding(*Layer.Clip), Layer.Time, Out);
            }
            continue;
        }

        Skeleton.GetRestPose(this->Scratch);
        if (Layer.Clip)
        {
            SampleClip(*Layer.Clip, ResolveBinding(*Layer.Clip), Layer.Time, this->Scratch);
        }
        for (auto Joint{0uz}; Joint < Out.size(); ++Joint)
        {
            Out[Joint] = LJointTransform::Blend(Out[Joint], this->Scratch[Joint], Layer.Weight);
        }
    }
}

Jafg::LAnimationClip const* Jafg::LAnimationPlayer::GetCurrentClip() const noexcept
{
    return this->Layers.empty() ? nullptr : this->Layers.back().Clip.get();
}

f32 Jafg::LAnimationPlayer::GetCurrentTime() const noexcept
{
    return this->Layers.empty() ? 0.0f : this->Layers.back().Time;
}

void Jafg::LAnimationPlayer::SetCurrentRate(f32 Rate) noexcept
{
    if (!this->Layers.empty())
    {
        this->Layers.back().Rate = Rate;
    }
}

bool Jafg::LAnimationPlayer::HasCurrentClipFinished() const noexcept
{
    if (this->Layers.empty() || !this->Layers.back().Clip)
    {
        return false;
    }
    LLayer const& Current{this->Layers.back()};
    if (Current.bLoop)
    {
        return false;
    }
    return Current.Rate >= 0.0f ? Current.Time >= Current.Clip->Duration : Current.Time <= 0.0f;
}
