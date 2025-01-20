// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Button.h"
#include "TabBarButton.generated.h"

namespace Jafg
{

class ENGINE_API LTabBarButtonBase
{
};

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API WTabBarButton : public WButton, public LTabBarButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBarButton)
};

} /* ~Namespace Jafg */
