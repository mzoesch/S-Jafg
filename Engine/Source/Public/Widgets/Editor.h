// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Editor.generated.h"

namespace Jafg
{

class WTabOverlay;

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditor)

public:

    virtual void Construct() override;

    WUserWidget& AddWindow(LTabCreateInfo Info);
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget> && CTabCandidate<TWidget>
    inline TWidget& AddWindow()
    {
        return *StaticCast<TWidget>(&this->AddWindow(TWidget::TabCreateInfo()));
    }

private:

    WTabOverlay* Selected{};
    TArray<TReference<WTabOverlay>> Overlays;
};

} /* ~Namespace Jafg */
