// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/Region.h"
#include "Widgets/InterfaceTypes.h"
#include "TestWidget.generated.h"

DECLARE_JAFG_WIDGET()
class WTestWidget : public Jafg::WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WIDGET_CONSTRUCTORS(WTestWidget)

public:

    virtual void Construct() override
    {
        Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
            .MinDesiredSize(200_pt2)
            .Type(Jafg::ERegionBrush::Box)
            .Tint(Colors::Aqua);
    }

    virtual void Draw(Jafg::LViewport& Context) const override
    {
        // Super::Draw(Context);
    }
};
