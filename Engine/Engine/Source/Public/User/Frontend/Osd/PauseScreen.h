// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "PauseScreen.generated.h"

namespace Jafg
{

class WTabBar;
class WPauseScreen;

DECLARE_JAFG_WIDGET()
class WPauseScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPauseScreen)

public:

    virtual void Construct() override;
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    FORCEINLINE bool IsTabBarValid() const { return this->TabBar != nullptr; }
    FORCEINLINE WTabBar* GetTabBar() { return this->TabBar; }
    FORCEINLINE WTabBar* GetTabBarChecked() { check( this->TabBar ) return this->TabBar; }
    FORCEINLINE WTabBar* GetTabBarAsserted() { jassert( this->TabBar ) return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBar() const { return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBarChecked() const { check( this->TabBar ) return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBarAsserted() const { jassert( this->TabBar ) return this->TabBar; }

private:

    WTabBar* TabBar { nullptr };
};

} /* ~Namespace Jafg */
