// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "NodeElementInspector.generated.h"

namespace Jafg
{

//# A widget that can inspect colors.
DECLARE_JAFG_WIDGET()
class ENGINE_API WNodeElementInspector final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WNodeElementInspector)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Node Inspector", "Icons/Jafg.Information")

    virtual void Construct() override;
};
static_assert(CTabSelectorCandidate<WNodeElementInspector>);
static_assert(CTabCandidate<WNodeElementInspector>);

} /* ~Namespace Jafg */
