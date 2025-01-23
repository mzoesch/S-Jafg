// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBarBase.h"
#include "TabBarPanel.generated.h"

namespace Jafg
{

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WTabBarPanel : public WTabBarBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBarPanel)
};

} /* ~Namespace Jafg */
