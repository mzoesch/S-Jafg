// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "ClassInspector.generated.h"

namespace Jafg
{

//# A widget that can inspect classes.
DECLARE_JAFG_WIDGET()
class ENGINE_API WClassInspector final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WClassInspector)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Class Inspector", "Icons/Jafg.File")

    virtual void Construct() override;
};
static_assert(CTabSelectorCandidate<WClassInspector>);
static_assert(CTabCandidate<WClassInspector>);

} /* ~Namespace Jafg */
