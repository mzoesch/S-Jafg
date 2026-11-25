// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Jgc/Framework/JgcHumanoidShapeComponent.h"

namespace Jafg
{

class LViewport;
class WFloatingWidget;

} /* ~Namespace Jafg */

namespace Jgc
{

struct LCustomizationWindowInfo final
{
    //# The values that the window starts with.
    LHumanoidShape Shape;
    //#
    //# The window spans its width, from its top left (e.g., the part of the world view next to the character). Empty:
    //# The top left of the viewport.
    //#
    std::optional<LRect2F> Area;
    //# After every change, with all values.
    TCopyableFunction<void(LHumanoidShape const& Shape)> OnChanged;
    //# The apply button.
    TCopyableFunction<void()> OnApply;
    //# The discard button and the close button of the window.
    TCopyableFunction<void()> OnDiscard;
};

//#
//# Opens a window that changes a #LHumanoidShape: A column per part of the body with a slider for every value (@see
//# #GetHumanoidShapeParameters), in percent, and buttons to apply or discard the changes. Values that do not fit
//# vertically scroll.
//#
//# The callbacks are expected to close the window (e.g., by leaving the customization station, whose owner closes
//# it). If they do not, the window closes itself after apply or discard.
//#
JGC_API Jafg::WFloatingWidget& OpenCustomizationWindow(Jafg::LViewport& Viewport, LCustomizationWindowInfo Info);

} /* ~Namespace Jgc */
