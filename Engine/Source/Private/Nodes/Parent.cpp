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

Jafg::LCursorReply Jafg::WParent::SweepMouse(LNodeSweepData const& Data, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable())
    {
        for (auto It{this->Children.rbegin()}; It != this->Children.rend(); ++It)
        {
            check(It->get())
            if ((*It)->ShouldCheckForInputs())
            {
                if (LCursorReply Reply{(*It)->SweepMouse(Data, Location)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
            continue;
        }
    }
    return Super::SweepMouse(Data, Location);
}

Jafg::LReply Jafg::WParent::SweepFocusTest(LNodeSweepData const& Data, const LVec2F& Location)
{
    if (this->CanChildrenBeHitTestable())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (LReply Reply{Child->SweepFocusTest(Data, Location)}; Reply.IsHandled())
            {
                return Reply;
            }
            continue;
        }
    }
    return Super::SweepFocusTest(Data, Location);
}

Jafg::LReply Jafg::WParent::OnParentKeyDown(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Info.CursorLocation.has_value())
    for (auto& Child : this->Children)
    {
        check(Child.get())
        check(&*Child != Info.Viewport.GetFocusedWidget())
        if (Child->ShouldCheckForInputs() && Child->IsInBounds({.Translation=Info.Translation}, *Info.CursorLocation))
        {
            if (LReply Reply{Child->OnParentKeyDown(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        continue;
    }
    return Super::OnParentKeyDown(Info, Event);
}

Jafg::LReply Jafg::WParent::OnParentKeyUp(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Info.CursorLocation.has_value())
    for (auto& Child : this->Children)
    {
        check(Child.get())
        check(&*Child != Info.Viewport.GetFocusedWidget())
        if (Child->ShouldCheckForInputs() && Child->IsInBounds({.Translation=Info.Translation}, *Info.CursorLocation))
        {
            if (LReply Reply{Child->OnParentKeyUp(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        continue;
    }
    return Super::OnParentKeyUp(Info, Event);
}

Jafg::LReply Jafg::WParent::OnParentKeyDownEntry(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Info.CursorLocation.has_value() && this->CanChildrenBeHitTestable())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (Child->ShouldCheckForInputs())
            {
                if (Child->IsInBounds({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (LReply Reply{Child->OnParentKeyDown(Info, Event)}; Reply.IsHandled())
                    {
                        return Reply;
                    }
                }
            }
            continue;
        }
    }
    return LReply::Unhandled();
}

Jafg::LReply Jafg::WParent::OnParentKeyUpEntry(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Info.CursorLocation.has_value() && this->CanChildrenBeHitTestable())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (Child->ShouldCheckForInputs())
            {
                if (Child->IsInBounds({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (LReply Reply{Child->OnParentKeyUp(Info, Event)}; Reply.IsHandled())
                    {
                        return Reply;
                    }
                }
            }
            continue;
        }
    }
    return LReply::Unhandled();
}

Jafg::LReply Jafg::WParent::OnKeyDownNoFocus(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() == false || &*Child == Info.Viewport.GetFocusedWidget())
        {
            continue;
        }
        if (Child->IsInBounds({maths::zero_vector<LVec2F>}, Info.Surface.GetMouseLocationValue()))
        {
            if (LReply Reply{Child->OnKeyDownNoFocus(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::OnKeyDownNoFocus(Info, Event);
}

Jafg::LReply Jafg::WParent::OnKeyUpNoFocus(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() == false || &*Child == Info.Viewport.GetFocusedWidget())
        {
            continue;
        }
        if (Child->IsInBounds({maths::zero_vector<LVec2F>}, Info.Surface.GetMouseLocationValue()))
        {
            if (LReply Reply{Child->OnKeyUpNoFocus(Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::OnKeyUpNoFocus(Info, Event);
}

void Jafg::WParent::OnSurfaceResize()
{
    Super::OnSurfaceResize();

    for (auto& Child : this->Children)
    {
        check(Child.get())
        Child->OnSurfaceResize();
        continue;
    }

    return;
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

Jafg::TJxxUnique<Jafg::WNode> Jafg::WParent::RemoveChildImpl(WNode& Child)
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
