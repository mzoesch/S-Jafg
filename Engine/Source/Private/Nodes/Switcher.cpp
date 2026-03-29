// Copyright mzoesch. All rights reserved.

#include "Nodes/Switcher.h"

void Jafg::WSwitcher::AddChildAt(u64 InIndex, TJxxUnique<WNode> InChild)
{
    Super::AddChildAt(InIndex, std::move(InChild));

    check(algo::find_pointer(this->RecentVisibilities, &*InChild, algo::pair_first{}) == nullptr)
    this->RecentVisibilities.emplace_back(&*InChild, InChild->GetVisibility());
    InChild->SetVisibility(ENodeVisibility::Collapsed);

    return;
}

void Jafg::WSwitcher::SetActiveNode(WNode* Node)
{
    check(Node)
    if (auto It{algo::find(this->GetChildren(), Node, algo::unique_raw{})}; It != this->GetChildren().end())
    {
        this->SetActiveNodeByIndex(algo::distance(this->GetChildren(), It));
    }
    else
    {
        LOG_WARNING(LogWidgets
            , "The node [{}] is not a child of switcher [{}]."
            , Node->GetNameAsString(), this->GetNameAsString()
        )
    }

    return;
}

void Jafg::WSwitcher::SetActiveNodeByIndex(i64 Index)
{
    if (this->ActiveNodeIndex == Index)
    {
        return;
    }

    if (WNode* CurrentNode{this->GetActiveNode()}; CurrentNode)
    {
        if (auto* Pair{algo::find_pointer(this->RecentVisibilities, CurrentNode, algo::pair_first{})})
        {
            Pair->second = CurrentNode->GetVisibility();
        }
        else
        {
            this->RecentVisibilities.emplace_back(CurrentNode, CurrentNode->GetVisibility());
        }
        CurrentNode->SetVisibility(ENodeVisibility::Collapsed);
    }

    this->ActiveNodeIndex = Index;

    if (Index == NoActiveNodeIndex)
    {
        return;
    }

    if (algo::is_valid_index(this->GetChildren(), Index) == false)
    {
        LOG_WARNING(LogWidgets, "The index [{}] is out of bounds.", Index)
        this->ActiveNodeIndex = NoActiveNodeIndex;
        return;
    }

    WNode* Node{this->GetActiveNode()};
    check(Node)
    if (auto* Pair{algo::find_pointer(this->RecentVisibilities, Node, algo::pair_first{})})
    {
        Node->SetVisibility(Pair->second);
        algo::erase_once_checked(&this->RecentVisibilities, Node, algo::pair_first{});
    }
    else
    {
        Node->SetVisibility(ENodeVisibility::Visible);
    }

    return;
}
