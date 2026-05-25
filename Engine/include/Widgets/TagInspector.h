// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "TagInspector.generated.h"

namespace Jafg
{

//# A widget that can inspect tags.
DECLARE_JAFG_WIDGET()
class ENGINE_API WTagInspector final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTagInspector)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Tag Inspector", "Icons/Jafg.File")

    virtual void Construct() override;
};
static_assert(CTabSelectorCandidate<WTagInspector>);
static_assert(CTabCandidate<WTagInspector>);

} /* ~Namespace Jafg */
