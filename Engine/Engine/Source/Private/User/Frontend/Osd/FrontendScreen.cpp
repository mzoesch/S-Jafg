// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/FrontendScreen.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "User/Frontend/Osd/HostSession.h"
#include "User/Frontend/Osd/PreferencesScreen.h"
#include "Engine/Engine.h"

void Jafg::WFrontendScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WCommonMenuTabBar).SaveTo(&this->TabBar)
        .AlignHorizontal()
    FinishWidgetStyling()

    {
        this->TabBar->AppendVSpace_v2(15_pt);
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "HostSession";
        Descriptor.PanelWidgetClassField = this->HostSessionScreenClass;
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
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "JoinSession";
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
        this->TabBar->AppendVSpace_v2(20_pt);
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
        Descriptor.IdentifierField = "Credits";
        Descriptor.PanelWidgetClassField.SetClass<WCommonMenuTabBarPanel>();
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        this->TabBar->AppendStretch(LAnchor::VFill);
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = "ExitToDesktop";
        Descriptor.OnButtonReleaseField = [](WTabBar& Self, const LString& InIdentifier) -> bool
        {
            Self.OnTabBarButtonReleased(InIdentifier);
            Self.GetEngine().RequestEngineExit("Exited through front-end menu.");
            return true;
        };
        this->TabBar->RegisterTab(std::move(Descriptor));
    }

    {
        this->TabBar->AppendVSpace_v2(55_pt);
    }

    return;
}
