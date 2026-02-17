// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "BackgroundBlur.generated.h"

namespace Jafg
{

struct LFactoryBackgroundBlur;

//#
//# Adds blur for perspective projection only. Warning: Expensive.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryBackgroundBlur)
class WBackgroundBlur : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WBackgroundBlur)

public:

    FORCEINLINE constexpr void SetBlurStrength(const f32 InStrength) noexcept { this->Strength = InStrength; }
    FORCEINLINE constexpr f32  GetBlurStrength(void) const noexcept { return this->Strength; }

private:

    f32 Strength{};
};

struct LFactoryBackgroundBlur : NODE_FACTORY_PARENT(WBackgroundBlur)
{
    NODE_FACTORY_BODY(WBackgroundBlur)

    decltype(auto) Strength(this auto&& Self, f32 InStrength) noexcept
    {
        NODE_FACTORY_SELF().SetBlurStrength(InStrength);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
