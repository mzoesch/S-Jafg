// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Button.h"
#include "TabBarButton.generated.h"

namespace Jafg
{

class WTabBar;
class WTextBox;

//# @return True if event was handled.
typedef TFunction<bool(WTabBar& Self, const LString& InIdentifier)> LOnTabBarButtonRelease;

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_WIDGET(ECxxClassFlags::Abstract)
class WTabBarButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabBarButton)

public:

    virtual bool AddData(JNodeData& InData) override;

    virtual void OnPrimaryRelease() override;

    //# @param bInFocus Whether the tab bar currently focuses the panel associated with this button.
    virtual void OnTabBarFocus(const bool bInFocus);

    FORCEINLINE bool IsButtonTextWidgetValid() const { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonTextWidget() -> WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidget() const -> const WTextBox* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() -> WTextBox* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() const -> const WTextBox* { check( this->ButtonText ); return this->ButtonText; }

protected:

    WTabBar* TabBar{};
    LString Identifier;
    WTextBox* ButtonText{};
    LOnTabBarButtonRelease OnButtonRelease;
};

} /* ~Namespace Jafg */
