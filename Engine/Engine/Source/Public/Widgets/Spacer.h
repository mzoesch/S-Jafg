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

    FORCEINLINE TFactoryRetTy& Size(LWidgetSize2 const& Size) noexcept { this->This()->SetSize(Size); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Size(LWidgetSize2&& Size) noexcept { this->This()->SetSize(std::move(Size)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Height(const LWidgetSize1 Height) noexcept { this->This()->SetHeight(Height); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Width(const LWidgetSize1 Width) noexcept { this->This()->SetWidth(Width);   return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactorySpacer)
class ENGINE_API WSpacer final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WSpacer)

public:

    virtual void UpdateDesiredSize() const override { this->SetDesiredSize(this->Size); }

    FORCEINLINE constexpr WSpacer& SetSize(LWidgetSize2 Size) noexcept
    {
        this->Size = Size;
        return *this;
    }

    FORCEINLINE constexpr WSpacer& SetHeight(LWidgetSize1 Height) noexcept
    {
        this->Size.SetYAxis(Height);
        return *this;
    }

    FORCEINLINE constexpr WSpacer& SetWidth(LWidgetSize1 Width) noexcept
    {
        this->Size.SetXAxis(Width);
        return *this;
    }

    FORCEINLINE constexpr LWidgetSize2 const& GetSize() const noexcept
    {
        return this->Size;
    }

private:

    LWidgetSize2 Size;
};

} /* ~Namespace Jafg */
