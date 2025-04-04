// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PauseScreen.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/Viewport.h"
#include "Platform/Surface.h"
#include "User/Frontend/Osd/PreferencesScreen.h"

void Jafg::WPauseScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion).Anchor(EAnchor::Fill)
    [
        NewNode(WCommonMenuTabBar).SaveTo(this->PauseTabBar)
            .Anchor(EAnchor::Fill)
            .AlignHorizontal()
            .BlurBackground(true)
    ]
    FinishWidgetStyling()

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Resume";
        Descriptor.OnButtonPressed = [](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonPressed(InIdentifier);
            Self.GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->AddVirtualKeyDown(EKeys::Escape);
            return true;
        };
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Achievements";
        Descriptor.PanelWidgetClass.Set<WCommonMenuTabBarPanel>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Encyclopedia";
        Descriptor.PanelWidgetClass.Set<WCommonMenuTabBarPanel>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Preferences";
        Descriptor.PanelWidgetClass.Set<WPreferencesScreen>();
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "SessionOptions";
        Descriptor.PanelWidgetClass.Set<WCommonMenuTabBarPanel>();
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
        Descriptor.OnButtonPressed = [](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonPressed(InIdentifier);
            Self.GetEngine()->RequestEngineExit("Exited through pause menu.");
            return true;
        };
        this->PauseTabBar->RegisterTab(std::move(Descriptor));
    }

    return;
}

void Jafg::WPauseScreen::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    if (this->PauseTabBar)
    {
        this->PauseTabBar->OnOuterVisibilityChanged(InOldVisibility, InNewVisibility);
    }

    return;
}
