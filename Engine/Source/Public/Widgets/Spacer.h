// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Spacer.generated.h"

namespace Jafg
{

struct LFactorySpacer;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactorySpacer)
class ENGINE_API WSpacer final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WSpacer)

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

struct LFactorySpacer : NODE_FACTORY_PARENT(WSpacer)
{
    NODE_FACTORY_BODY(WSpacer)

    decltype(auto) Size(this auto&& Self, LWidgetSize2 const& Size) noexcept
    {
        NODE_FACTORY_SELF().SetSize(Size);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Height(this auto&& Self, const LWidgetSize1 Height) noexcept
    {
        NODE_FACTORY_SELF().SetHeight(Height);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Width(this auto&& Self, const LWidgetSize1 Width) noexcept
    {
        NODE_FACTORY_SELF().SetWidth(Width);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
