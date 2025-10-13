// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarPanel.h"
#include "Widgets/Compound/TabBar.h"

Jafg::WTabBarPanel::WTabBarPanel(LCxxObjectInitializer const& CxxObjectInitializer) : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

bool Jafg::WTabBarPanel::AddData(const LWidgetNodeData* InData)
{
    const bool bSuper = Super::AddData(InData);

    if (InData->DerivedClass != WTabBarPanel::StaticClass()->GetName())
    {
        return bSuper;
    }

    const LTabBarTabData* Data = static_cast<const LTabBarTabData*>(InData);
    this->OwningTabBar = Data->Context;

    return true;
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
