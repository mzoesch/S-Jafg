// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "ColorInspector.generated.h"

namespace Jafg
{

//# A widget that can inspect colors.
DECLARE_JAFG_WIDGET()
class ENGINE_API WColorInspector final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WColorInspector)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Color Inspector", "Icons/Jafg.Colors")

    virtual void Construct() override;
};
static_assert(CTabSelectorCandidate<WColorInspector>);
static_assert(CTabCandidate<WColorInspector>);

} /* ~Namespace Jafg */
