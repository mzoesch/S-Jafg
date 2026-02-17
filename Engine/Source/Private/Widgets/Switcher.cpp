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
        if (LRecentVisibility* Recent = algo::find_pointer(this->RecentVisibilities, CurrentNode, &LRecentVisibility::Target))
        {
            Recent->Visibility = CurrentNode->GetVisibility();
        }
        else
        {
            this->RecentVisibilities.emplace_back(CurrentNode, CurrentNode->GetVisibility());
        }
        CurrentNode->SetVisibility(ENodeVisibility::Collapsed);
    }

    this->ActiveIndex = Index;

    if (Index == NoActiveWidgetIndex)
    {
        return;
    }

    if (algo::is_valid_index(this->GetChildren(), Index) == false)
    {
        LOG_WARNING(LogWidgets, "The index [{}] is out of bounds.", Index)
        this->ActiveIndex = NoActiveWidgetIndex;
        return;
    }

    if (WNode* NewNode = this->GetActiveNode(); NewNode)
    {
        if (LRecentVisibility* Recent = algo::find_pointer(this->RecentVisibilities, NewNode, &LRecentVisibility::Target))
        {
            NewNode->SetVisibility(Recent->Visibility);
            algo::erase_once_checked(&this->RecentVisibilities, NewNode, &LRecentVisibility::Target);
        }
        else
        {
            NewNode->SetVisibility(ENodeVisibility::Visible);
        }
    }

    return;
}

void Jafg::WSwitcher::SetActiveWidget(WNode* Widget)
{
    checkSlow( Widget )

    if (auto It{ algo::find(this->GetChildren(), Widget, &LWidgetSlot::Content) }; It != this->GetChildren().end())
    {
        this->SetActiveWidgetIndex(algo::distance(this->GetChildren(), It));
    }
    else
    {
        LOG_WARNING
        (
            LogWidgets,
            "The widget [{}] is not a child of this [{}] switcher.",
            Widget->GetNameAsString(), this->GetNameAsString()
        )
    }

    return;
}

Jafg::LWidgetSlot* Jafg::WSwitcher::AddChild(WNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChild(InChild);

    check( algo::find_pointer(this->RecentVisibilities, InChild, &LRecentVisibility::Target) == nullptr )
    this->RecentVisibilities.emplace_back(InChild, InChild->GetVisibility());
    InChild->SetVisibility(ENodeVisibility::Collapsed);

    return Ret;
}

Jafg::LWidgetSlot* Jafg::WSwitcher::AddChildAt(const i32 InIndex, WNode* InChild)
{
    LWidgetSlot* Ret = Super::AddChildAt(InIndex, InChild);

    check( algo::find_pointer(this->RecentVisibilities, InChild, &LRecentVisibility::Target) == nullptr )
    this->RecentVisibilities.emplace_back(InChild, InChild->GetVisibility());
    InChild->SetVisibility(ENodeVisibility::Collapsed);

    return Ret;
}
