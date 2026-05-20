// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Core/Application.h"

namespace Jafg
{

//# Base struct for input action modifiers. Inherit from this to add your own modifiers.
struct LInputActionMappedTriggerModifier;
typedef TArray<TUnique<LInputActionMappedTriggerModifier>> LInputActionMappedTriggerModifiers;

//# Negate all axes of the input value. */
struct LInputActionMappedKeyNegateModifier;

//# Swizzles two different axis's.
struct LInputActionMappedKeySwizzleXYModifier;
struct LInputActionMappedKeySwizzleXZModifier;
struct LInputActionMappedKeySwizzleYZModifier;

//#
//# Factory function to create a modifier of type TModifier.
//#
template <typename TModifier> requires(std::is_abstract_v<TModifier> == false && std::is_base_of_v<LInputActionMappedTriggerModifier, TModifier>)
FORCEINLINE TUnique<LInputActionMappedTriggerModifier> MakeInputModifier() noexcept;

struct LInputActionMappedTriggerModifier
{
    virtual ~LInputActionMappedTriggerModifier() = default;
    virtual LVec3F ApplyModifier(const LVec3F& InValue) const noexcept = 0;
};

struct LInputActionMappedKeyNegateModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeyNegateModifier() override = default;
    virtual LVec3F ApplyModifier(const LVec3F& InValue) const noexcept override { return -InValue; }
};

struct LInputActionMappedKeySwizzleXYModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleXYModifier() override = default;
    virtual LVec3F ApplyModifier(const LVec3F& InValue) const noexcept override { return maths::yxz(InValue); }
};

struct LInputActionMappedKeySwizzleXZModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleXZModifier() override = default;
    virtual LVec3F ApplyModifier(const LVec3F& InValue) const noexcept override { return maths::zyx(InValue); }
};

struct LInputActionMappedKeySwizzleYZModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleYZModifier() override = default;
    virtual LVec3F ApplyModifier(const LVec3F& InValue) const noexcept override { return maths::xzy(InValue); }
};

template <typename TModifier> requires(std::is_abstract_v<TModifier> == false && std::is_base_of_v<LInputActionMappedTriggerModifier, TModifier>)
FORCEINLINE TUnique<LInputActionMappedTriggerModifier> MakeInputModifier() noexcept
{
    return TUnique<LInputActionMappedTriggerModifier>{ new TModifier{} };
}

} /* ~Namespace Jafg */
