// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarPanel.h"
#include "Widgets/Compound/TabBar.h"

Jafg::WTabBarPanel::WTabBarPanel(LCxxObjectInitializer const& CxxObjectInitializer) : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

bool Jafg::WTabBarPanel::AddData(JNodeData& Data)
{
    const bool bSuper = Super::AddData(Data);
    JTabBarData* TbData{ Data.As<JTabBarData>() };
    if (TbData == nullptr)
    {
        return bSuper;
    }

    this->OwningTabBar = TbData->Context;

    return true;
}

void Jafg::WTabBarPanel::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize; check( DesiredSize.IsZero() )
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize_v2().X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize_v2().Y);

        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt();

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}
