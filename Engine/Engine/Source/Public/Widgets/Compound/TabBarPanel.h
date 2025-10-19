// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "TabBarPanel.generated.h"

namespace Jafg
{

class WTabBar;

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_WIDGET(ECxxClassFlags::Abstract)
class WTabBarPanel : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabBarPanel(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WTabBarPanel)

public:

    virtual bool AddData(JNodeData& Data) override;
    virtual void UpdateDesiredSize() const override;

    FORCEINLINE bool IsOwningTabBarValid(void) const { return this->OwningTabBar != nullptr; }
    FORCEINLINE auto GetOwningTabBar(void) -> WTabBar* { return this->OwningTabBar; }
    FORCEINLINE auto GetOwningTabBar(void) const -> const WTabBar* { return this->OwningTabBar; }
    FORCEINLINE auto GetOwningTabBarChecked(void) -> WTabBar* { check( this->OwningTabBar ); return this->OwningTabBar; }
    FORCEINLINE auto GetOwningTabBarChecked(void) const -> const WTabBar* { check( this->OwningTabBar ); return this->OwningTabBar; }

private:

    WTabBar* OwningTabBar { nullptr };
};

} /* ~Namespace Jafg */
