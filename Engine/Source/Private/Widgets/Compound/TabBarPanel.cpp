// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarPanel.h"
#include "Widgets/Compound/TabBar.h"

bool Jafg::WTabBarPanel::AddData(JNodeData& Data)
{
    const bool bSuper = Super::AddData(Data);
    JTabBarData* TbData{ Data.As<JTabBarData>() };
    if (TbData == nullptr)
    {
        return bSuper;
    }

    this->OwningTabBar = TbData->TabBar;

    return true;
}

void Jafg::WTabBarPanel::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVec2F DesiredSize{ maths::zero_vector<LVec2F> };
    for (auto& Child : this->GetChildren())
    {
        DesiredSize.x = maths::max(DesiredSize.x, Child->GetDesiredSize_v2().x);
        DesiredSize.y = maths::max(DesiredSize.y, Child->GetDesiredSize_v2().y);
    }
    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}
