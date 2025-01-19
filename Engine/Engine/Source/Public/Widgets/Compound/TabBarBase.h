// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParent.h"
#include "TabBarBase.generated.h"

namespace Jafg
{

/** @see Widgets/Compound/TabBar.h */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API WTabBarBase : public WWidgetParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBarBase)
};


} /* ~Namespace Jafg */
