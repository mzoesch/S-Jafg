// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Core/Application.h"

namespace Jafg
{

//# Base struct for input action modifiers. Inherit from this to add your own modifiers.
struct LInputActionMappedKeyModifier;

//# Negate all axes of the input value. */
struct LInputActionMappedKeyNegateModifier;

//# Swizzle the X and Y axes of the input value. Aliases as it is the most common used.
struct LInputActionMappedKeySwizzleModifier;
struct LInputActionMappedKeySwizzleXYModifier;
struct LInputActionMappedKeySwizzleXZModifier;
struct LInputActionMappedKeySwizzleYZModifier;

//# Apply current application wide delta time to the input value.
struct LInputActionMappedKeyDeltaTimeModifier;

template <typename TModifier>
FORCEINLINE TModifier* MakeModifier()
{
    static_assert(std::is_abstract_v<TModifier> == false, "Cannot instantiate an abstract modifier.");
    static_assert(std::is_base_of_v<LInputActionMappedKeyModifier, TModifier>, "Modifier must derive from LInputActionMappedKeyModifier.");
    return new TModifier();
}

struct LInputActionMappedKeyModifier
{
    virtual ~LInputActionMappedKeyModifier() = default;
    virtual LVector ApplyModifier(const LVector& InValue) = 0;
};

struct LInputActionMappedKeyNegateModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeyNegateModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return -InValue; }
};

struct LInputActionMappedKeySwizzleModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeySwizzleModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return InValue.YXZ(); }
};

struct LInputActionMappedKeySwizzleXYModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeySwizzleXYModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return InValue.YXZ(); }
};

struct LInputActionMappedKeySwizzleXZModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeySwizzleXZModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return InValue.ZYX(); }
};

struct LInputActionMappedKeySwizzleYZModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeySwizzleYZModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return InValue.XZY(); }
};

struct LInputActionMappedKeyDeltaTimeModifier final : public LInputActionMappedKeyModifier
{
    ~LInputActionMappedKeyDeltaTimeModifier() override = default;
    virtual LVector ApplyModifier(const LVector& InValue) override { return InValue * Application::GetDeltaTimeAsFloat(); }
};

} /* ~Namespace Jafg */
