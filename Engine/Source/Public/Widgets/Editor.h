// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Editor.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditor)

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
