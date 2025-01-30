// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarPanel.h"

Jafg::WTabBarPanel::WTabBarPanel(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WTabBarPanel::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize().X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize().Y);

        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}
