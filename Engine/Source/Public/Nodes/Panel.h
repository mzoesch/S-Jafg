// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Parent.h"
#include "Panel.generated.h"

namespace Jafg
{

//#
//# A panel is a parent node that has a complex but precise drawing capabilities.
//# It has many children that can be individually and precisely anchored with the LAnchor struct.
//#
//# If no complex drawing is needed, use #WOverlay instead for better performance.
//#
DECLARE_JAFG_WIDGET()
class WPanel : public WParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WPanel)

public:

    //# TODO Build the body of this widget...
    //#      This was used, but then after the widget refactor not anymore... Therefore it is empty for now.
    virtual void Construct() override NON_CALLABLE_MEMBER()
};

} /* ~Namespace Jafg */
