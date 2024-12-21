// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "Spacer.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactorySpace : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    using Super::operator&;

    FORCEINLINE TFactoryRetTy& SetSize(const LVector2&  InSize) { this->This()->SetSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetSize(const LVector2&& InSize) { this->This()->SetSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& operator& (const LVector2&  InSize) { return this->SetSize(InSize); }
    FORCEINLINE TFactoryRetTy& operator& (const LVector2&& InSize) { return this->SetSize(InSize); }

    FORCEINLINE TFactoryRetTy& SetHeight(const float InHeight) { this->This()->SetHeight(InHeight); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetWidth(const float InWidth)   { this->This()->SetWidth(InWidth);   return this->Self(); }
};

DECLARE_JAFG_CLASS()
class ENGINE_API WSpacer final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WSpacer)

public:

    using TWidgetFactoryTy = TWidgetFactorySpace<Derived>;

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
