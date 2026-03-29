// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "Nodes/ButtonBase.h"
#include "Button.generated.h"

namespace Jafg
{

class WButton;
typedef TFactoryButtonBase<WButton> LFactoryButton;

//# A generic overlay button.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryButton)
class ENGINE_API WButton : public WRegion, public TButtonBase<WButton, LRegionBrush, decltype(&WRegion::SetBrush)>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this, &WRegion::SetBrush}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

    template<typename TCxxClass>
    explicit WButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this, &WRegion::SetBrush}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
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
