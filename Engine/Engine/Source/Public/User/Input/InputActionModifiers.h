// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Core/Application.h"

namespace Jafg
{

//# Base struct for input action modifiers. Inherit from this to add your own modifiers.
struct LInputActionMappedTriggerModifier;

//# Negate all axes of the input value. */
struct LInputActionMappedKeyNegateModifier;

//# Swizzle the X and Y axes of the input value. Aliased as it is the most common used.
struct LInputActionMappedKeySwizzleModifier;

//# Swizzles two different axis's.
struct LInputActionMappedKeySwizzleXYModifier;
struct LInputActionMappedKeySwizzleXZModifier;
struct LInputActionMappedKeySwizzleYZModifier;

//# Apply current application wide delta time to the input value.
struct LInputActionMappedKeyDeltaTimeModifier;

template <typename TModifier>
FORCEINLINE Smart::TUnique<LInputActionMappedTriggerModifier> MakeModifier()
{
    static_assert(std::is_abstract_v<TModifier> == false, "Cannot instantiate an abstract modifier.");
    static_assert(std::is_base_of_v<LInputActionMappedTriggerModifier, TModifier>, "Modifier must derive from LInputActionMappedKeyModifier.");
    return Smart::EmplaceUniqueOfType<LInputActionMappedTriggerModifier, TModifier>();
}

struct LInputActionMappedTriggerModifier
{
    virtual ~LInputActionMappedTriggerModifier() = default;
    virtual LVector ApplyModifier(const LVector& InValue) const = 0;
};

struct LInputActionMappedKeyNegateModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeyNegateModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return -InValue; }
};

struct LInputActionMappedKeySwizzleModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return InValue.YXZ(); }
};

struct LInputActionMappedKeySwizzleXYModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleXYModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return InValue.YXZ(); }
};

struct LInputActionMappedKeySwizzleXZModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleXZModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return InValue.ZYX(); }
};

struct LInputActionMappedKeySwizzleYZModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeySwizzleYZModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return InValue.XZY(); }
};

struct LInputActionMappedKeyDeltaTimeModifier final : public LInputActionMappedTriggerModifier
{
    ~LInputActionMappedKeyDeltaTimeModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) const override { return InValue * Application::GetDeltaTimeAsFloat(); }
};

} /* ~Namespace Jafg */
