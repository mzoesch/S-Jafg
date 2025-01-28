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
        NewNode(WCommonMenuTabBar).SaveTo(this->PauseTabBar).Anchor(EAnchor::Fill)
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
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Encyclopedia";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Preferences";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "SessionOptions";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "ExitToMenu";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "ExitToDesktop";
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    return;
}
