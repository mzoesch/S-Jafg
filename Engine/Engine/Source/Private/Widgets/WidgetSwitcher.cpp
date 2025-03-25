// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetSwitcher.h"

void Jafg::WWidgetSwitcher::SetActiveWidgetIndex(const i32 Index)
{
    if (this->ActiveIndex == Index)
    {
        return;
    }

    if (WWidgetNode* CurrentNode = this->GetActiveNode(); CurrentNode)
    {
        if (LRecentVisibility* Recent = this->RecentVisibilities.FindByPredicate([CurrentNode](const LRecentVisibility& InRecent)
        {
            return InRecent.Target == CurrentNode;
        }))
        {
            Recent->Visibility = CurrentNode->GetVisibility();
        }
        else
        {
            this->RecentVisibilities.Emplace(CurrentNode, CurrentNode->GetVisibility());
        }
        CurrentNode->SetVisibility(EWidgetVisibility::Collapsed);
    }

    this->ActiveIndex = Index;

    if (Index == NoActiveWidgetIndex)
    {
        return;
    }

    if (this->GetChildren().IsValidIndex(Index) == false)
    {
        LOG_WARNING(LogWidgets, "The index [{}] is out of bounds.", Index)
        return;
    }

    if (WWidgetNode* NewNode = this->GetActiveNode(); NewNode)
    {
        if (LRecentVisibility* Recent = this->RecentVisibilities.FindByPredicate([NewNode](const LRecentVisibility& InRecent)
        {
            return InRecent.Target == NewNode;
        }))
        {
            NewNode->SetVisibility(Recent->Visibility);
            this->RecentVisibilities.RemoveOnceByPredicateChecked([NewNode](const LRecentVisibility& InRecent)
            {
                return InRecent.Target == NewNode;
            });
        }
        else
        {
            NewNode->SetVisibility(EWidgetVisibility::Visible);
        }
    }

    return;
}

void Jafg::WWidgetSwitcher::SetActiveWidget(WWidgetNode* Widget)
{
    checkSlow( Widget )

    if (const i32 Idx = this->GetChildren().FindIndexByPredicate([Widget](const LWidgetSlot* Slot)
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

Jafg::LWidgetSlot* Jafg::WWidgetSwitcher::AddChild(WWidgetNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChild(InChild);

    check( this->RecentVisibilities.FindByPredicate([InChild](const LRecentVisibility& InRecent){ return InRecent.Target == InChild; }) == nullptr )
    this->RecentVisibilities.Emplace(InChild, InChild->GetVisibility());
    InChild->SetVisibility(EWidgetVisibility::Collapsed);

    return Ret;
}

Jafg::LWidgetSlot* Jafg::WWidgetSwitcher::AddChildAt(const i32 InIndex, WWidgetNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChildAt(InIndex, InChild);

    check( this->RecentVisibilities.FindByPredicate([InChild](const LRecentVisibility& InRecent){ return InRecent.Target == InChild; }) == nullptr )
    this->RecentVisibilities.Emplace(InChild, InChild->GetVisibility());
    InChild->SetVisibility(EWidgetVisibility::Collapsed);

    return Ret;
}

void Jafg::WWidgetSwitcher::UpdateDesiredSize() const
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
