// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
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
class ENGINE_API WBackgroundBlur : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WBackgroundBlur)

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual LReply OnKeyDown(LKeyEvent& InKeyEvent) override;
    virtual void OnFocusReceived() override;

    FORCEINLINE void SetBlurStrength(const float InStrength) { this->Strength = InStrength; }
    FORCEINLINE auto GetBlurStrength(void) const -> float { return this->Strength; }

private:

    float Strength = 0.0f;
};

} /* ~Namespace Jafg */
