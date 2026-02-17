// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/Region.h"
#include "Widgets/VRegion.h"
#include "Widgets/Switcher.h"
#include "Widgets/BackgroundBlur.h"

void Jafg::WCommonMenuTabBarPanel::Construct()
{
    Super::Construct();

    LColor Tint{Colors::Black};
    if (WCommonMenuTabBar const* CommonTabBar{DynamicCast<WCommonMenuTabBar>(this->GetOwningTabBar())})
    {
        Tint.A = WCommonMenuTabBar::GetAlphaTintBasedOfDepth(CommonTabBar->GetLeafDepth());
    }

    BeginStyling(*this).Root<WRegion>().SaveTo(&this->Panel)
        .Anchor(EAnchor::Fill)
        .Type(ERegionBrush::Box)
        .Tint(Tint);

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
        BeginStyling(*this).Root<WBackgroundBlur>()
            .Anchor(EAnchor::Fill)
            .Strength(0.4f);
    }

    Super::Construct();

    this->SetAnchor(EAnchor::Fill);

    if (WVRegion* VRegion = DynamicCast<WVRegion>(this->ButtonsContainer); VRegion)
    {
        VRegion->SetMinDesiredSize({80_pt, 0});

        VRegion->SetAnchor(EAnchor::VFill);
        VRegion->SetType(ERegionBrush::Box);
        VRegion->SetTint({0, 0, 0, WCommonMenuTabBar::GetAlphaTintBasedOfDepth(this->Depth)});
    }

    this->Switcher->SetAnchor(EAnchor::Fill);

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
