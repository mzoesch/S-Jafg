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
        check(Child->_HasBegunLife() == false)
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
        for (auto& Child : this->Children)
        {
            if (auto Reply{Child->SweepFocus(ChildInfo, Location)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
    }
    return Super::SweepFocus(Info, Location);
}

Jafg::LNodeReply Jafg::WParent::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (!(Location.has_value() || (this->GetNodeState() & ENodeStateBits::Hovered)))
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
        else if ((*It)->GetNodeState() & ENodeStateBits::Hovered)
        {
            auto Reply{(*It)->Sweep(ChildInfo, {})};
            check(!((*It)->GetNodeState() & ENodeStateBits::Hovered))
            check(!Reply.IsHandled())
        }
        continue;
    }
    for (;It != this->Children.rend(); ++It)
    {
        if ((*It)->GetNodeState() & ENodeStateBits::Hovered)
        {
            check(It->get())
            auto Reply{(*It)->Sweep(ChildInfo, {})};
            check(!((*It)->GetNodeState() & ENodeStateBits::Hovered))
            check(!Reply.IsHandled())
        }
    }

    if (Result.IsHandled())
    {
        check(Location.has_value())
        this->_RemoveHoverDispatchedState();
        return Result;
    }

    return Super::Sweep(Info, Location);
}

void Jafg::WParent::_RemoveHoverState() noexcept
{
    for (auto& Child : this->Children)
    {
        Child->_RemoveHoverState();
        checkCode(Child->_check_StateInvariant())
    }

    Super::_RemoveHoverState();
    return;
}

Jafg::LNodeReply Jafg::WParent::OnKeyDownUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() && Child->AabbTest({.Translation=Info.Translation}, Info.Surface.GetMouseLocationValue()))
        {
            if (auto Reply{Child->OnKeyDownUnfocused(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
    }
    return Super::OnKeyDownUnfocused(Info, Event);
}

Jafg::LNodeReply Jafg::WParent::OnKeyUpUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() && Child->AabbTest({.Translation=Info.Translation}, Info.Surface.GetMouseLocationValue()))
        {
            if (auto Reply{Child->OnKeyUpUnfocused(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
    }
    return Super::OnKeyUpUnfocused(Info, Event);
}

bool Jafg::WParent::IsNodeInVisiblePath(WNode const* Node) const
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

Jafg::WNode const* Jafg::WParent::FindNodeInVisiblePath(TSubclassOf<WNode> Class) const noexcept
{
    if (auto* Result{Super::FindNodeInVisiblePath(Class)})
    {
        return Result;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (auto* Result{Child->FindNodeInVisiblePath(Class)})
            {
                return Result;
            }
            continue;
        }
    }

    return nullptr;
}

Jafg::WNode* Jafg::WParent::FindNodeInVisiblePath(TSubclassOf<WNode> Class) noexcept
{
    if (auto* Result{Super::FindNodeInVisiblePath(Class)})
    {
        return Result;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (auto* Result{Child->FindNodeInVisiblePath(Class)})
            {
                return Result;
            }
            continue;
        }
    }

    return nullptr;
}

Jafg::WNode& Jafg::WParent::AddChildAt(std::size_t Index, TJxxUnique<WNode> Child)
{
    check(Tasks::IsOnMasterThread())
    check(Child.get())
    check(algo::contains(this->GetChildren(), &*Child, [](auto const& E){return &*E;}) == false)
    check(Child->_HasBegunLife() == false)

    WNode& Result{this->OnAddChild(Index, std::move(Child), false)};

    if (WUserWidget* UserWidget{DynamicCast<WUserWidget>(&this->GetMostOuterParent())})
    {
        if (UserWidget->IsTopLevel())
        {
            check(UserWidget->_HasBegunLife())
            check(this->_HasBegunLife())
            check(Result._HasBegunLife() == false)
            MakeCxxObjectFinal(Result);
        }
        else
        {
            check(this->_HasBegunLife() == false)
            check(Result._HasBegunLife() == false)
        }
    }

    return Result;
}

Jafg::WNode& Jafg::WParent::AddConstructedChildAt(std::size_t Index, TJxxUnique<WNode> Child)
{
    check(Tasks::IsOnMasterThread())
    check(this->_HasBegunLife())
    check(Child.get())
    check(algo::contains(this->GetChildren(), &*Child, [](auto const& E){return &*E;}) == false)
    check(Child->_HasBegunLife())

    return this->OnAddChild(Index, std::move(Child), false);
}

Jafg::WNode& Jafg::WParent::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    Child->_SetParentDangerous(this);
    return **this->Children.insert(this->Children.begin() + Index, std::move(Child));
}

TJxxUnique<Jafg::WNode> Jafg::WParent::RemoveChildImpl(WNode& Child)
{
    check(algo::find(this->Children, &Child, [](auto const& E){return &*E;}) != this->Children.end())
    this->OnRemoveChildPrepare(Child);

    auto It{algo::find(this->Children, &Child, [](auto const& E){return &*E;})};
    if (It == this->Children.end())
    {
        LOG_FATAL(LogWidgetFramework, "The child [{}] is not a child of [{}]."
            , Child.GetNameAsString(), this->GetNameAsString()
            )
    }
    auto Result{std::move(*It)};
    this->Children.erase(It);

    this->OnRemoveChildPost(Child);

    return Result;
}
