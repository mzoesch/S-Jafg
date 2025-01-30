// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetSwitcher.h"

void Jafg::WWidgetSwitcher::SetActiveWidgetIndex(const int32 Index)
{
    if (this->GetChildren().IsValidIndex(Index) == false)
    {
        LOG_WARNING(LogWidgets, "The index [{}] is out of bounds.", Index)
        return;
    }

    this->ActiveIndex = Index;
    LOG_WARNING(LogTemporal, "Set active widget index to [{}].", Index)

    return;
}

void Jafg::WWidgetSwitcher::SetActiveWidget(WWidgetNode* Widget)
{
    checkSlow( Widget )

    if (const int32 Idx = this->GetChildren().FindIndexByPredicate([Widget](const LWidgetSlot* Slot)
    {
        return Slot->Content == Widget;
    }); Idx != INDEX_NONE)
    {
        this->SetActiveWidgetIndex(Idx);
    }
    else
    {
        LOG_WARNING(
            LogWidgets,
            "The widget [{}] is not a child of this [{}] switcher.",
            Widget->GetFullName(), this->GetFullName()
        )
    }

    return;
}

void Jafg::WWidgetSwitcher::Tick()
{
    WWidgetParent::Tick();
}

Jafg::LCursorReply Jafg::WWidgetSwitcher::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->IsIndexValid() == false)
    {
        return LCursorReply::Unhandled();
    }

    WWidgetNode* Node = this->GetActiveNode();
    if (Node->ShouldCheckForInputs())
    {
        LCursorReply Reply = Node->SweepMouse(Context, InLocation);
        if (Reply.IsHandled())
        {
            return Reply;
        }
    }

    // DO NOT CALL THE SUPER METHOD OF THE PARENT AND PARENT BASE CLASS!!!
    return WWidgetNode::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WWidgetSwitcher::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    if (this->IsIndexValid() == false)
    {
        return LReply::Unhandled();
    }

    WWidgetNode* Node = this->GetActiveNode();
    LReply Reply = Node->SweepFocusTest(Context, InLocation);
    if (Reply.IsHandled())
    {
        return Reply;
    }

    // DO NOT CALL THE SUPER METHOD OF THE PARENT AND PARENT BASE CLASS!!!
    return WWidgetNode::SweepFocusTest(Context, InLocation);
}

void Jafg::WWidgetSwitcher::UpdateDesiredSize() const
{
    if (this->IsIndexValid() == false)
    {
        this->SetDesiredSize({});
        return;
    }

    const WWidgetNode* Node = this->GetActiveNode();
    Node->UpdateDesiredSize();

    LVector2 DesiredSize = Node->GetDesiredSize();
    DesiredSize += this->GetPadding().GetDesiredSize();
    this->SetDesiredSize(DesiredSize);

    return;
}

void Jafg::WWidgetSwitcher::UpdateAnchoredSize(const LViewport& Context) const
{
    // DO NOT CALL THE SUPER METHOD OF THE PARENT AND PARENT BASE CLASS!!!
    WWidgetNode::UpdateAnchoredSize(Context);

    if (this->IsIndexValid())
    {
        this->GetActiveNode()->UpdateAnchoredSize(Context);
    }

    return;
}

void Jafg::WWidgetSwitcher::Draw(LViewport& Context) const
{
    if (this->IsIndexValid())
    {
        const WWidgetNode* Node = this->GetActiveNode();
        if (Node->ShouldNowDraw())
        {
            Node->Draw(Context);
        }
    }

    return;
}
