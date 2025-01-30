// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PauseMenu.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"

void Jafg::WPauseMenu::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion).Anchor(EAnchor::Fill)
    [
        NewNode(WCommonMenuTabBar).SaveTo(this->PauseTabBar).AlignHorizontal().Anchor(EAnchor::Fill)
    ]
    FinishWidgetStyling()

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Resume";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Achievements";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Encyclopedia";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Preferences";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "SessionOptions";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "ExitToMenu";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "ExitToDesktop";
        Descriptor.PanelWidgetClass.Set<WDevelopmentTabBarPanelPlaceholder>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    return;
}
