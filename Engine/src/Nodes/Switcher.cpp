// Copyright mzoesch. All rights reserved.

#include "Nodes/Switcher.h"

Jafg::WNode& Jafg::WSwitcher::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    check(algo::find(this->RecentVisibilities, &*Child, algo::pair_first) == this->RecentVisibilities.end())
    this->RecentVisibilities.emplace_back(&*Child, Child->GetVisibility());
    Child->SetVisibility(ENodeVisibility::Collapsed);

    return Super::OnAddChild(Index, std::move(Child), bConstructed);
}

void Jafg::WSwitcher::OnRemoveChildPrepare(WNode& Child)
{
    Super::OnRemoveChildPrepare(Child);

    if (this->GetActiveNode() == &Child)
    {
        this->ResetActiveNode();
    }

    if (auto* Pair{algo::find_pointer(this->RecentVisibilities, &Child, algo::pair_first)})
    {
        Child.SetVisibility(Pair->second);
    }
    algo::erase(&this->RecentVisibilities, &Child, algo::pair_first);

    return;
}

void Jafg::WSwitcher::SetActiveNode(WNode& Node)
{
    check(algo::contains(this->GetChildren(), &Node, algo::unique_raw))

    if (this->GetActiveNode() == &Node)
    {
        return;
    }

    this->ResetActiveNode();
    this->ActiveNode = &Node;

    if (auto* Pair{algo::find_pointer(this->RecentVisibilities, &Node, algo::pair_first)})
    {
        Node.SetVisibility(Pair->second);
        algo::erase_once_checked(&this->RecentVisibilities, &Node, algo::pair_first);
    }
    else
    {
        Node.SetVisibility(ENodeVisibility::Visible);
    }

    return;
}

void Jafg::WSwitcher::ResetActiveNode()
{
    if (this->ActiveNode)
    {
        if (auto* Pair{algo::find_pointer(this->RecentVisibilities, this->ActiveNode, algo::pair_first)})
        {
            Pair->second = this->ActiveNode->GetVisibility();
        }
        else
        {
            this->RecentVisibilities.emplace_back(this->ActiveNode, this->ActiveNode->GetVisibility());
        }
        this->ActiveNode->SetVisibility(ENodeVisibility::Collapsed);
    }

    this->ActiveNode = nullptr;

    return;
}
