// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/TabOverlay.h"
#include "Editor.generated.h"

namespace Jafg
{

class WTabOverlay;

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget, public LTabOverlayPossibilities
{
    GENERATED_CLASS_BODY()

protected:

    inline  explicit WEditor(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
    }
    template<typename TCxxClass>
    inline explicit WEditor(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
    }

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
