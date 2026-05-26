// Copyright mzoesch. All rights reserved.

#include "Nodes/Parent.h"
#include "Nodes/UserWidget.h"
#include "Nodes/Viewport.h"
#include "Platform/Surface.h"

void Jafg::WParent::Construct()
{
    Super::Construct();
    check(this->_HasBegunLife())

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        check(!Child->_HasBegunLife())
        MakeCxxObjectFinal(*Child);
        check(Child->_HasBegunLife())
        continue;
    }

    return;
}

void Jafg::WParent::Tick()
{
    Super::Tick();

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (Child->ShouldNowTick())
        {
            Child->Tick();
        }
        continue;
    }

    return;
}

void Jafg::WParent::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (Child->ShouldNowDraw())
        {
            Child->Draw(Info);
        }
        continue;
    }

    return;
}

Jafg::LNodeReply Jafg::WParent::SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable())
    {
        LNodeSweepInfo ChildInfo{.Translation=Info.Translation + Info.ChildTranslationHint,};
        for (auto It{this->Children.rbegin()}; It != this->Children.rend(); ++It)
        {
            if (auto Reply{(*It)->SweepFocus(ChildInfo, Location)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
    }
    return Super::SweepFocus(Info, Location);
}

Jafg::LNodeReply Jafg::WParent::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (!(Location.has_value() || (this->_GetNodeState() & Detail::NodeStateSwept)))
    {
        return {};
    }

    LNodeSweepInfo ChildInfo{.Translation=Info.Translation + Info.ChildTranslationHint,};

    LNodeReply Result;
    auto It{this->Children.rbegin()};
    for (;It != this->Children.rend(); ++It)
    {
        check(It->get())
        if ((*It)->ShouldCheckForInputs())
        {
            if (auto Reply{(*It)->Sweep(ChildInfo, Location)}; Reply.IsHandled())
            {
                Result = Reply;
                ++It;
                break;
            }
        }
        else if ((*It)->_GetNodeState() & Detail::NodeStateSwept)
        {
            auto Reply{(*It)->Sweep(ChildInfo, {})};
            check(!((*It)->_GetNodeState() & Detail::NodeStateSwept))
            check(!Reply.IsHandled())
        }
        continue;
    }
    for (;It != this->Children.rend(); ++It)
    {
        if ((*It)->_GetNodeState() & Detail::NodeStateSwept)
        {
            check(It->get())
            auto Reply{(*It)->Sweep(ChildInfo, {})};
            check(!((*It)->_GetNodeState() & Detail::NodeStateSwept))
            check(!Reply.IsHandled())
        }
    }

    if (Result.IsHandled())
    {
        check(Location.has_value())
        this->_RemoveDispatchedState();
        return Result;
    }

    return Super::Sweep(Info, Location);
}

Jafg::LNodeReply Jafg::WParent::OnKeyEventUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() && Child->AabbTest({.Translation=Info.Translation}, Info.Surface.GetMouseLocationValue()))
        {
            if (auto Reply{Child->OnKeyEventUnfocused(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
    }
    return Super::OnKeyEventUnfocused(Info, Event);
}

bool Jafg::WParent::IsNodeInVisiblePath(WNode const& Node) const
{
    if (Super::IsNodeInVisiblePath(Node))
    {
        return true;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (Child->IsNodeInVisiblePath(Node))
            {
                return true;
            }
            continue;
        }
    }

    return false;
}

Jafg::WNode const* Jafg::WParent::FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) const noexcept
{
    if (auto* Result{Super::FindNodeInVisiblePathImpl(Class)})
    {
        return Result;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (auto* Result{Child->FindNodeInVisiblePathImpl(Class)})
            {
                return Result;
            }
            continue;
        }
    }

    return nullptr;
}

Jafg::WNode* Jafg::WParent::FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) noexcept
{
    if (auto* Result{Super::FindNodeInVisiblePathImpl(Class)})
    {
        return Result;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (auto* Result{Child->FindNodeInVisiblePathImpl(Class)})
            {
                return Result;
            }
            continue;
        }
    }

    return nullptr;
}

void Jafg::WParent::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateDesiredSize();
        }
        else
        {
            Child->SetDesiredSize({});
        }
    }

    return;
}

void Jafg::WParent::UpdateAnchoredSize() const
{
    Super::UpdateAnchoredSize();

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateAnchoredSize();
        }
        else
        {
            Child->SetAnchoredSize(maths::zero_vector<LVec2F>);
        }
    }

    return;
}

Jafg::WNode& Jafg::WParent::AddChildAt(std::size_t Index, TJxxUnique<WNode> Child)
{
    check(Tasks::IsOnMasterThread())
    check(!this->_IsGarbage())
    check(Child.get())
    check(algo::contains(this->GetChildren(), &*Child, [](auto const& E){return &*E;}) == false)
    check(!Child->_HasBegunLife())

    WNode& Result{this->OnAddChild(Index, std::move(Child), false)};

    if (WUserWidget* UserWidget{DynamicCast<WUserWidget>(&this->GetMostOuterParent())})
    {
        if (UserWidget->IsTopLevel())
        {
            check(UserWidget->_HasBegunLife())
            check(this->_HasBegunLife())
            check(!Result._HasBegunLife())
            MakeCxxObjectFinal(Result);
        }
        else
        {
            check(!this->_HasBegunLife())
            check(!Result._HasBegunLife())
        }
    }

    return Result;
}

Jafg::WNode& Jafg::WParent::AddConstructedChildAt(std::size_t Index, TJxxUnique<WNode> Child)
{
    check(Tasks::IsOnMasterThread())
    check(this->_HasBegunLife())
    check(!this->_IsGarbage())
    check(Child.get())
    check(algo::contains(this->GetChildren(), &*Child, [](auto const& E){return &*E;}) == false)
    check(Child->_HasBegunLife())

    return this->OnAddChild(Index, std::move(Child), false);
}

Jafg::WNode& Jafg::WParent::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    check(Tasks::IsOnMasterThread())
    check(Child.get())
    check(!Child->IsParentValid())
    check(algo::valid_index(this->Children, Index) || this->Children.size() == Index)
    Child->_SetParentDangerous(this);
    return **this->Children.insert(this->Children.begin() + Index, std::move(Child));
}

std::size_t Jafg::WParent::ReorderChild(WNode& Who, std::size_t Desired)
{
    check(Tasks::IsOnMasterThread())
    check(Who.GetParentChecked() == this)
    check(algo::valid_index(this->Children, Desired))

    auto Distance{algo::distance(this->Children, algo::find(this->Children, &Who, [](auto const& E){return &*E;}))};
    if (static_cast<std::size_t>(Distance) < Desired)
    {
        std::rotate(this->Children.begin() + Distance, this->Children.begin() + Distance + 1, this->Children.begin() + Desired + 1);
    }
    else if (static_cast<std::size_t>(Distance) > Desired)
    {
        std::rotate(this->Children.begin() + Desired, this->Children.begin() + Distance, this->Children.begin() + Distance + 1);
    }

    return {};
}

TJxxUnique<Jafg::WNode> Jafg::WParent::RemoveChildImpl(WNode& Child)
{
    check(Tasks::IsOnMasterThread())
    check(algo::find(this->Children, &Child, [](auto const& E){return &*E;}) != this->Children.end())
    this->OnRemoveChildPrepare(Child);

    Child._ResetFocusState();
    Child.Sweep({}, {}); // Reset hover state.

    auto It{algo::find(this->Children, &Child, [](auto const& E){return &*E;})};
    if (It == this->Children.end())
    {
        LOG_FATAL(LogWidgetFramework, "The child [{}] is not a child of [{}]."
            , Child.GetNameAsString(), this->GetNameAsString()
            )
    }
    auto Result{std::move(*It)};
    this->Children.erase(It);

    check(Child.IsParentValid() && Child.GetParent() == this)
    this->OnRemoveChildPost(Child);
    check(!Child.IsParentValid())

    return Result;
}
