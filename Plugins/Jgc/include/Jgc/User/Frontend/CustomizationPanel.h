// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Jgc/Framework/HumanoidShapeComponent.h"

namespace Jafg
{

class LViewport;
class WUserWidget;

} /* ~Namespace Jafg */

namespace Jgc
{

struct LCustomizationPanelInfo final
{
    //# The values that the panel starts with.
    LHumanoidShape Shape;
    //# The part of the width of the viewport that the panel takes, from its left. It always spans its whole height.
    f32 Width{ 2.0f / 3.0f };
    //# After every change, with all values.
    TCopyableFunction<void(LHumanoidShape const& Shape)> OnChanged;
    //# The apply button.
    TCopyableFunction<void()> OnApply;
    //# The discard button.
    TCopyableFunction<void()> OnDiscard;
};

//#
//# Opens a panel that changes a #LHumanoidShape. It is a top level widget at the left of the viewport, from its top to
//# its bottom: A column per part of the body with a slider for every value (@see #GetHumanoidShapeParameters), in
//# percent, and buttons to apply or discard the changes at the bottom. Values that do not fit scroll.
//#
//# The callbacks are expected to close the panel (e.g., by leaving the customization station, whose owner closes
//# it). If they do not, the panel closes itself after apply or discard.
//#
JGC_API Jafg::WUserWidget& OpenCustomizationPanel(Jafg::LViewport& Viewport, LCustomizationPanelInfo Info);

} /* ~Namespace Jgc */
