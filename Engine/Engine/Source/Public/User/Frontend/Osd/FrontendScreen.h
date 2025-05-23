// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/HostSession.h"
#include "FrontendScreen.generated.h"

namespace Jafg
{

class WHostSessionScreen;
class WTabBar;
class WFrontendScreen;

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class ENGINE_API WFrontendScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WFrontendScreen)

public:

    virtual void Construct() override;

    FORCEINLINE bool IsTabBarValid() const { return this->TabBar != nullptr; }
    FORCEINLINE WTabBar* GetTabBar() { return this->TabBar; }
    FORCEINLINE WTabBar* GetTabBarChecked() { check( this->TabBar ) return this->TabBar; }
    FORCEINLINE WTabBar* GetTabBarAsserted() { jassert( this->TabBar ) return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBar() const { return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBarChecked() const { check( this->TabBar ) return this->TabBar; }
    FORCEINLINE const WTabBar* GetTabBarAsserted() const { jassert( this->TabBar ) return this->TabBar; }

    CLASS_FIELD(Config)
    TSubclassOf<WHostSessionScreen> HostSessionScreenClass { LazyInit };

private:

    WTabBar* TabBar { nullptr };
};

} /* ~Namespace Jafg */
