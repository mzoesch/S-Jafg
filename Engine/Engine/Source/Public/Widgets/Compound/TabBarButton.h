// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Button.h"
#include "TabBarButton.generated.h"

namespace Jafg
{

class WTabBar;
class WTextBlock;

//# @return True if event was handled.
typedef TFunction<bool(WTabBar& Self, const LString& InIdentifier)> LOnTabBarButtonRelease;

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WTabBarButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBarButton)

public:

    virtual bool AddData(const LWidgetNodeData* InData) override;

    virtual void OnPrimaryRelease() override;

    //# @param bInFocus Whether the tab bar currently focuses the panel associated with this button.
    virtual void OnTabBarFocus(const bool bInFocus);

    FORCEINLINE bool IsButtonTextWidgetValid() const { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonTextWidget() -> WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidget() const -> const WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() -> WTextBlock* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() const -> const WTextBlock* { check( this->ButtonText ); return this->ButtonText; }

protected:

    WTabBar* Context { nullptr };
    LString Identifier;
    WTextBlock* ButtonText { nullptr };
    LOnTabBarButtonRelease OnButtonRelease;
};

} /* ~Namespace Jafg */
