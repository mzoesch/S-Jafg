// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "BackgroundBlur.generated.h"

namespace Jafg
{

struct LFactoryBackgroundBlur;

//# Adds blur for perspective projection only. Warning: Expensive.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryBackgroundBlur)
class WBackgroundBlur : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WBackgroundBlur)

public:

    f32 BlurStrength{};
};

struct LFactoryBackgroundBlur : NODE_FACTORY_PARENT(WBackgroundBlur)
{
    NODE_FACTORY_BODY(WBackgroundBlur)

    decltype(auto) Strength(this auto&& Self, f32 Strength) noexcept
    {
        NODE_FACTORY_SELF().BlurStrength = Strength;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
