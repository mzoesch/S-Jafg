// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PauseScreen.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "Widgets/Region.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Platform/Surface.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/PreferencesScreen.h"
#include "Widgets/Spacer.h"

void Jafg::WPauseScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WCommonMenuTabBar).SaveTo(&this->TabBar)
        .Anchor(EAnchor::Fill)
        .AlignHorizontal()
        .BlurBackground(true)
    FinishWidgetStyling()

    {
        this->TabBar->AppendVSpace(35.0f);
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "Resume";
        Descriptor.OnButtonReleaseField = [](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonReleased(InIdentifier);
            Self.GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->AddVirtualKeyDown(EKeys::Escape);
            return true;
        };
        Descriptor.CallbackField = [](WTabBar* TabBar, WNode* Button, WNode* Panel) -> void
        {
            check( Button )
            if (WTabBarButton* Btn = DynamicCast<WTabBarButton>(Button))
            {
                WTextBox* Text = Btn->GetButtonTextWidget();
                if (Text)
                {
                    Text->SetBrush(LTextBoxBrush::Header());
                }
            }
        };
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        this->TabBar->AppendVSpace(40.0f);
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "Achievements";
        Descriptor.PanelWidgetClassField.SetClass<WCommonMenuTabBarPanel>();
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "Encyclopedia";
        Descriptor.PanelWidgetClassField.SetClass<WCommonMenuTabBarPanel>();
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "Preferences";
        Descriptor.PanelWidgetClassField.SetClass<WPreferencesScreen>();
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "SessionOptions";
        Descriptor.PanelWidgetClassField.SetClass<WCommonMenuTabBarPanel>();
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        this->TabBar->AppendStretch(LAnchor::VFill);
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "ExitToMenu";
        Descriptor.OnButtonReleaseField = [this](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonReleased(InIdentifier);
            if (this->GetOuter()->IsWorld())
            {
                this->GetEngine()->Browse(static_cast<LWorld*>(this->GetOuter()), Name_LevelFrontend.ToString());
            }
            else
            {
                LOG_ERROR(LogWidgets, "Cannot host, because outer is not a world.")
            }
            return true;
        };
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "ExitToDesktop";
        Descriptor.OnButtonReleaseField = [](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonReleased(InIdentifier);
            Self.GetEngine()->RequestEngineExit("Exited through pause menu.");
            return true;
        };
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        this->TabBar->AppendVSpace(125.0f);
    }

    return;
}

void Jafg::WPauseScreen::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    LViewport* Viewport { this->GetViewport() };
    if (ensure(Viewport != nullptr) == false)
    {
        return;
    }

    WCrosshair* Crosshair { Viewport->GetTopLevelWidgetByClass<WCrosshair>() };

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        if (this->TabBar)
        {
            this->TabBar->ResetToDefault();
        }

        if (Crosshair)
        {
            Crosshair->SetVisibility(EWidgetVisibility::Collapsed);
        }
    }
    else
    {
        if (Crosshair)
        {
            Crosshair->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
        }
    }

    return;
}
