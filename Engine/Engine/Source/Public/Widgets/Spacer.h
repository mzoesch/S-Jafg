// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Spacer.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactorySpacer : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Size(const LVector2&  InSize) { this->This()->SetSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Size(const LVector2&& InSize) { this->This()->SetSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Height(const f32 InHeight)  { this->This()->SetHeight(InHeight); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Width(const f32 InWidth)    { this->This()->SetWidth(InWidth);   return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactorySpacer)
class WSpacer final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WSpacer)

public:

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE WSpacer& SetSize(const LVector2& InSize)
    {
        this->Size = InSize;
        return *this;
    }

    FORCEINLINE WSpacer& SetHeight(const float InHeight)
    {
        this->Size.Y = InHeight;
        return *this;
    }

    FORCEINLINE WSpacer& SetWidth(const float InWidth)
    {
        this->Size.X = InWidth;
        return *this;
    }

private:

    LVector2 Size = LVector2::Zero();
};

} /* ~Namespace Jafg */
