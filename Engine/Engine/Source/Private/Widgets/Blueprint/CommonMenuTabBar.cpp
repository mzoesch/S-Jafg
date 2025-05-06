// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/VBox.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/BackgroundBlur.h"

void Jafg::WCommonMenuTabBarButton::Construct()
{
    this->SetNormalBrush({LColor::Transparent});
    this->SetHoverBrush({{0, 0, 0, 128}});
    this->SetPressBrush({{0, 0, 0, 192}});

    Super::Construct();

    this->SetPadding({5, 20});
    this->SetMinDesiredSize(LVector2(10));
    this->SetAnchor(EAnchor::HFill);

    if (this->IsButtonTextWidgetValid())
    {
        this->GetButtonTextWidget()->SetAnchor(EAnchor::CenterLeft);
    }

    return;
}

void Jafg::WCommonMenuTabBarPanel::Construct()
{
    Super::Construct();

    LColor Tint = LColor::Black;
    if (const WCommonMenuTabBar* CommonTabBar = DynamicCast<WCommonMenuTabBar>(this->GetOwningTabBar()))
    {
        Tint.A = WCommonMenuTabBar::GetAlphaTintBasedOfDepth(CommonTabBar->GetLeafDepth());
    }

    NewNode(WWidgetRegion).SaveTo(this->Panel)
        .Anchor(EAnchor::Fill).Tint(Tint);
    this->AddChild(this->Panel);
    MakeDeferredWidgetNodeFinal(this->Panel);

    return;
}

void Jafg::WCommonMenuTabBar::Construct()
{
    if (this->DefaultButtonClass == nullptr)
    {
        this->DefaultButtonClass.Set<WCommonMenuTabBarButton>();
    }

    if (this->bBlur)
    {
        WBackgroundBlur* Blur = ConstructDeferredWidgetNode<WBackgroundBlur>();
        Blur->SetAnchor(EAnchor::Fill);
        Blur->SetBlurStrength(0.4f);
        this->AddChild(Blur);
    }

    Super::Construct();

    this->SetAnchor(EAnchor::Fill);

    if (WVBox* VBox = DynamicCast<WVBox>(this->ButtonsContainer); VBox)
    {
        VBox->SetMinDesiredSize({200, 0});

        VBox->SetTint({0, 0, 0, WCommonMenuTabBar::GetAlphaTintBasedOfDepth(this->Depth)});
        VBox->SetAnchor(EAnchor::VFill);
    }

    this->Switcher->SetAnchor(EAnchor::Fill);

    return;
}
