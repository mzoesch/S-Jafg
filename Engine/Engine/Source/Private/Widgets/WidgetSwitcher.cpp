// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetSwitcher.h"

void Jafg::WWidgetSwitcher::SetActiveWidgetIndex(const int32 Index)
{
    return;
}

void Jafg::WWidgetSwitcher::SetActiveWidget(WWidgetNode* Widget)
{
    return;
}

void Jafg::WWidgetSwitcher::Tick()
{
    WWidgetParent::Tick();
}

Jafg::LCursorReply Jafg::WWidgetSwitcher::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    return WWidgetParent::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WWidgetSwitcher::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    return WWidgetParent::SweepFocusTest(Context, InLocation);
}

bool Jafg::WWidgetSwitcher::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    return WWidgetParent::IsFocusWidgetTransitive(InViewport);
}

void Jafg::WWidgetSwitcher::UpdateDesiredSize() const
{
    WWidgetParent::UpdateDesiredSize();
}

void Jafg::WWidgetSwitcher::UpdateAnchoredSize(const LViewport& Context) const
{
    WWidgetParent::UpdateAnchoredSize(Context);
}

void Jafg::WWidgetSwitcher::Draw(LViewport& Context) const
{
    if (this->GetChildren().IsValidIndex(this->ActiveIndex))
    {
        const WWidgetNode* Node = this->GetChildren()[this->ActiveIndex]->Content;
        check( Node )
        if (Node->ShouldNowDraw())
        {
            Node->Draw(Context);
        }
    }

    return;
}

void Jafg::WWidgetSwitcher::RemoveChild(WWidgetNode* InSlot)
{
    Super::RemoveChild(InSlot);
}

Jafg::LWidgetSlot* Jafg::WWidgetSwitcher::AddChild(WWidgetNode* InChild)
{
    return Super::AddChild(InChild);
}
