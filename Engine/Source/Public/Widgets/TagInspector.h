// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/TabOverlayForward.h"
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

    static LTabOverlayElement CreateTabDescriptor(LViewport& Viewport) noexcept
    {
        return {
            .Selector = LTabOverlayElement::CreateInfo{
                .DisplayName = "Tag Inspector",
                .Icon = LString{"Icons/Jafg.File"},
                },
            .Panel = NewStaticNodeVp(Viewport, WTagInspector),
            };
    }

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
