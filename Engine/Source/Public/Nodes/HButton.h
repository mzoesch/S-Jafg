// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HRegion.h"
#include "Nodes/ButtonBase.h"
#include "HButton.generated.h"

namespace Jafg
{

class WHButton;
typedef TFactoryButtonBase<WHButton> LFactoryHButton;

//# A generic horizontal button.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHButton)
class ENGINE_API WHButton : public WHRegion, public TButtonBase<WHButton, LRegionBrush, &WHRegion::Brush>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WHButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(TButtonBase::DefaultVisibility);
    }

    template<typename TCxxClass>
    explicit WHButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(TButtonBase::DefaultVisibility);
    }

public:

    virtual void Construct() override
    {
        Super::Construct();
        this->ButtonBase_Construct();
    }

    JAFG_NODE_BUTTON_BOILERPLATE()
};

} /* ~Namespace Jafg */
