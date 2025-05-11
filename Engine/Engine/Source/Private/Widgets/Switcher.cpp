// Copyright mzoesch. All rights reserved.

#include "Widgets/Switcher.h"

void Jafg::WSwitcher::SetActiveWidgetIndex(const i32 Index)
{
    if (this->ActiveIndex == Index)
    {
        return;
    }

    if (WNode* CurrentNode = this->GetActiveNode(); CurrentNode)
    {
        if (LRecentVisibility* Recent = this->RecentVisibilities.FindRefByPredicate([CurrentNode](const LRecentVisibility& InRecent)
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
        this->ActiveIndex = NoActiveWidgetIndex;
        return;
    }

    if (WNode* NewNode = this->GetActiveNode(); NewNode)
    {
        if (LRecentVisibility* Recent = this->RecentVisibilities.FindRefByPredicate([NewNode](const LRecentVisibility& InRecent)
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

void Jafg::WSwitcher::SetActiveWidget(WNode* Widget)
{
    checkSlow( Widget )

    if (const i32 Idx = this->GetChildren().FindByPredicate([Widget](const LWidgetSlot* Slot)
    {
        return Slot->Content == Widget;
    }); Idx != INDEX_NONE)
    {
        this->SetActiveWidgetIndex(Idx);
    }
    else
    {
        LOG_WARNING
        (
            LogWidgets,
            "The widget [{}] is not a child of this [{}] switcher.",
            Widget->GetFullName(), this->GetFullName()
        )
    }

    return;
}

Jafg::LWidgetSlot* Jafg::WSwitcher::AddChild(WNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChild(InChild);

    check( this->RecentVisibilities.FindRefByPredicate([InChild](const LRecentVisibility& InRecent){ return InRecent.Target == InChild; }) == nullptr )
    this->RecentVisibilities.Emplace(InChild, InChild->GetVisibility());
    InChild->SetVisibility(EWidgetVisibility::Collapsed);

    return Ret;
}

Jafg::LWidgetSlot* Jafg::WSwitcher::AddChildAt(const i32 InIndex, WNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChildAt(InIndex, InChild);

    check( this->RecentVisibilities.FindRefByPredicate([InChild](const LRecentVisibility& InRecent){ return InRecent.Target == InChild; }) == nullptr )
    this->RecentVisibilities.Emplace(InChild, InChild->GetVisibility());
    InChild->SetVisibility(EWidgetVisibility::Collapsed);

    return Ret;
}
