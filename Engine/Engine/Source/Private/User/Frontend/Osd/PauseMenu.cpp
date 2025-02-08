// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PauseMenu.h"

#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/Viewport.h"
#include "Platform/Surface.h"

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
        Descriptor.OnButtonPressed = [](WTabBar& Self, const LSimpleString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonPressed(InIdentifier);
            Self.GetLocalEgo()->GetPrimarySurface()->AddVirtualKeyDown(EKeys::Escape);
            return true;
        };
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
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "ExitToDesktop";
        Descriptor.OnButtonPressed = [](WTabBar& Self, const LSimpleString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonPressed(InIdentifier);
            Self.GetEngine()->RequestEngineExit("Exited through pause menu.");
            return true;
        };
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    return;
}

void Jafg::WPauseMenu::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    if (this->PauseTabBar)
    {
        this->PauseTabBar->OnOuterVisibilityChanged(InOldVisibility, InNewVisibility);
    }

    return;
}
