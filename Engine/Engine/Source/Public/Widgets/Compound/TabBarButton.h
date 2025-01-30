// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Button.h"
#include "TabBarButton.generated.h"

namespace Jafg
{

class WTabBar;
class WTextBlock;

//# @see Widgets/Compound/TabBar.h
DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WTabBarButton : public WButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBarButton)

public:

    virtual bool AddData(LWidgetNodeData* InData) override;

    virtual void OnPrimaryPress() override;

    FORCEINLINE bool IsButtonTextWidgetValid() const { return this->ButtonText != nullptr; }
    FORCEINLINE auto GetButtonTextWidget() -> WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidget() const -> const WTextBlock* { return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() -> WTextBlock* { check( this->ButtonText ); return this->ButtonText; }
    FORCEINLINE auto GetButtonTextWidgetChecked() const -> const WTextBlock* { check( this->ButtonText ); return this->ButtonText; }

private:

    WTabBar* Context = nullptr;
    LSimpleString Identifier;
    WTextBlock* ButtonText = nullptr;
};

} /* ~Namespace Jafg */
