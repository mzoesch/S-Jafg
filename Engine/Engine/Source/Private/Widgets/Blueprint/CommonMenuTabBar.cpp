// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/VBox.h"
#include <random>

void Jafg::WCommonMenuTabBarButton::Construct()
{
    Super::Construct();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    this->SetTint(LColor(dist(gen), dist(gen), dist(gen), 128));

    this->SetPadding({5, 20});
    this->SetAnchor(EAnchor::HFill);
    this->SetMinDesiredSize(LVector2(10));

    if (this->IsButtonTextWidgetValid())
    {
        this->GetButtonTextWidget()->SetAnchor(EAnchor::CenterLeft);
    }

    return;
}

void Jafg::WCommonMenuTabBar::Construct()
{
    if (this->DefaultButtonClass == nullptr)
    {
        this->DefaultButtonClass.Set<WCommonMenuTabBarButton>();
    }

    Super::Construct();

    this->SetAnchor(EAnchor::Fill);

    if (WVBox* VBox = DynamicCast<WVBox>(this->ButtonsContainer); VBox)
    {
        VBox->SetMinDesiredSize({200, 0});
        VBox->SetTint({0, 0, 0, 128});
        VBox->SetAnchor(EAnchor::VFill);
    }

    return;
}
