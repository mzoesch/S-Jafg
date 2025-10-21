// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/Region.h"
#include "Widgets/VRegion.h"
#include "Widgets/Switcher.h"
#include "Widgets/BackgroundBlur.h"

Jafg::WCommonMenuTabBarButton::WCommonMenuTabBarButton(Jafg::LCxxObjectInitializer const& ObjectInitializer)
    : Super{ObjectInitializer}
{
    this->SetNormalBrush({ERegionBrush::None});
    this->SetHoverBrush({ERegionBrush::Box, {0, 0, 0, 128}});
    this->SetPressBrush({ERegionBrush::Box, {0, 0, 0, 192}});

    this->SetPadding({5, 10});
    this->SetMinDesiredSize(5_pt2);
    this->SetAnchor(EAnchor::HFill);

    return;
}

void Jafg::WCommonMenuTabBarButton::Construct()
{
    Super::Construct();

    if (this->IsButtonTextWidgetValid())
    {
        this->GetButtonTextWidget()->SetAnchor(EAnchor::CenterLeft);
        this->GetButtonTextWidget()->SetBrush(LTextBoxBrush::SubHeader());
    }

    return;
}

void Jafg::WCommonMenuTabBarPanel::Construct()
{
    Super::Construct();

    Lal::LColor Tint = Lal::LColor::Black;
    if (const WCommonMenuTabBar* CommonTabBar = DynamicCast<WCommonMenuTabBar>(this->GetOwningTabBar()))
    {
        Tint.A = WCommonMenuTabBar::GetAlphaTintBasedOfDepth(CommonTabBar->GetLeafDepth());
    }

    NewNode(WRegion).SaveTo(&this->Panel)
        .Anchor(EAnchor::Fill)
        .Type(ERegionBrush::Box)
        .Tint(Tint);
    this->AddChild(this->Panel);
    MakeDeferredWidgetNodeFinal(this->Panel);

    return;
}

void Jafg::WCommonMenuTabBar::Construct()
{
    if (this->DefaultButtonClass == nullptr)
    {
        this->DefaultButtonClass.SetClass<WCommonMenuTabBarButton>();
    }

    if (this->bBlur)
    {
        WBackgroundBlur* Blur = ConstructDeferredWidgetNode<WBackgroundBlur>(this->GetOuter());
        Blur->SetAnchor(EAnchor::Fill);
        Blur->SetBlurStrength(0.4f);
        this->AddChild(Blur);
    }

    Super::Construct();

    this->SetAnchor(EAnchor::Fill);

    if (WVRegion* VRegion = DynamicCast<WVRegion>(this->ButtonsContainer); VRegion)
    {
        VRegion->SetMinDesiredSize({100_pt, 0});

        VRegion->SetAnchor(EAnchor::VFill);
        VRegion->SetType(ERegionBrush::Box);
        VRegion->SetTint({0, 0, 0, WCommonMenuTabBar::GetAlphaTintBasedOfDepth(this->Depth)});
    }

    this->Switcher->SetAnchor(EAnchor::Fill);

    return;
}
