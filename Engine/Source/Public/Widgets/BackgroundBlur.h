// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "BackgroundBlur.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryBackgroundBlur : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Strength(const float InStrength) { this->This()->SetBlurStrength(InStrength); return this->Self(); }
};

//#
//# Adds blur for perspective projection only. Warning: Expensive.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryBackgroundBlur)
class WBackgroundBlur : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WBackgroundBlur)

public:

    FORCEINLINE constexpr void SetBlurStrength(const f32 InStrength) noexcept { this->Strength = InStrength; }
    FORCEINLINE constexpr f32  GetBlurStrength(void) const noexcept { return this->Strength; }

private:

    f32 Strength{ 0.0f };
};

} /* ~Namespace Jafg */
