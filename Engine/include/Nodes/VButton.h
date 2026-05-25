// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/VRegion.h"
#include "Nodes/ButtonBase.h"
#include "VButton.generated.h"

namespace Jafg
{

class WVButton;
typedef TFactoryButtonBase<WVButton> LFactoryVButton;

//# A generic vertical button.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryVButton)
class ENGINE_API WVButton : public WVRegion, public TButtonBase<WVButton, LRegionBrush, &WVRegion::Brush>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WVButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(TButtonBase::DefaultVisibility);
    }

    template<typename TCxxClass>
    explicit WVButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this}
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
