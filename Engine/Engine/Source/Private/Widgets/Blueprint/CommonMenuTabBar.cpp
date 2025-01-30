// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/VBox.h"
#include "Widgets/WidgetSwitcher.h"
#include <random>

void Jafg::WCommonMenuTabBarButton::Construct()
{
    this->SetNormalBrush({LColor::Transparent});
    this->SetHoverBrush({{0, 0, 0, 64}});
    this->SetPressBrush({{0, 0, 0, 128}});

    Super::Construct();

    this->SetPadding({5, 20});
    this->SetMinDesiredSize(LVector2(10));
    this->SetAnchor(EAnchor::HFill);

    if (this->IsButtonTextWidgetValid())
    {
        this->GetButtonTextWidget()->SetAnchor(EAnchor::CenterLeft);
        this->GetButtonTextWidget()->SetPadding({});
    }

    return;
}

void Jafg::WDevelopmentTabBarPanelPlaceholder::Construct()
{
    Super::Construct();

    // Make a random color for the panel.
    std::random_device RandomDevice;
    std::mt19937 RandomEngine(RandomDevice());
    std::uniform_int_distribution<int> RandomColor(0, 255);
    LColor Tint = LColor(RandomColor(RandomEngine), RandomColor(RandomEngine), RandomColor(RandomEngine), 32);

    ConstructDeferredWidgetNode<WWidgetRegion>()->GetFactory<WWidgetRegion>().SaveTo(this->Panel)
        .Tint(Tint).Anchor(EAnchor::Fill).MinDesiredSize({50,70});
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

    Super::Construct();

    this->SetAnchor(EAnchor::Fill);

    if (WVBox* VBox = DynamicCast<WVBox>(this->ButtonsContainer); VBox)
    {
        VBox->SetMinDesiredSize({200, 0});
        VBox->SetTint({0, 0, 0, 128});
        VBox->SetAnchor(EAnchor::VFill);
    }

    this->Switcher->SetAnchor(EAnchor::Fill);

    return;
}
