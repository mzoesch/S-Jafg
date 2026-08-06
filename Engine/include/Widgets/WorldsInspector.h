// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "WorldsInspector.generated.h"

namespace Jafg
{

//# A widget that can inspect worlds.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldsInspector : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WWorldsInspector)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Worlds Inspector", "Icons/Jafg.Sphere")

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
