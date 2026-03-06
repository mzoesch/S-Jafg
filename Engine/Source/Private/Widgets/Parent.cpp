// Copyright mzoesch. All rights reserved.

#include "Widgets/Parent.h"
#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"

void Jafg::WParent::Construct()
{
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

Jafg::LCursorReply Jafg::WParent::SweepMouse(LViewport& Viewport, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable())
    {
        for (auto It{this->Children.rbegin()}; It != this->Children.rend(); ++It)
        {
            check(It->get())
            if ((*It)->ShouldCheckForInputs())
            {
                if (LCursorReply Reply{(*It)->SweepMouse(Viewport, Location)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
            continue;
        }
    }

    return Super::SweepMouse(Viewport, Location);
}

Jafg::LReply Jafg::WParent::SweepFocusTest(LViewport const& Viewport, const LVec2F& Location)
{
    if (this->CanChildrenBeHitTestable())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (LReply Reply{Child->SweepFocusTest(Viewport, Location)}; Reply.IsHandled())
            {
                return Reply;
            }
            continue;
        }
    }

    return Super::SweepFocusTest(Viewport, Location);
}

Jafg::LReply Jafg::WParent::OnKeyDownNoFocus(const LViewport& Viewport, LKeyEvent const& KeyEvent)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() == false || &*Child == Viewport.GetFocusedWidget())
        {
            continue;
        }
        if (Child->IsInBounds(Viewport, *Viewport.GetCachedCursorLocationChecked()))
        {
            if (LReply Reply{Child->OnKeyDownNoFocus(Viewport, KeyEvent)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::OnKeyDownNoFocus(Viewport, KeyEvent);
}

Jafg::LReply Jafg::WParent::OnKeyUpNoFocus(const LViewport& Viewport, const LKeyEvent& KeyEvent)
{
    for (auto& Child : this->Children)
    {
        check(Child.get())
        if (Child->ShouldCheckForInputs() == false || &*Child == Viewport.GetFocusedWidget())
        {
            continue;
        }
        if (Child->IsInBounds(Viewport, *Viewport.GetCachedCursorLocationChecked()))
        {
            if (LReply Reply{Child->OnKeyUpNoFocus(Viewport, KeyEvent)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::OnKeyUpNoFocus(Viewport, KeyEvent);
}

bool Jafg::WParent::IsFocusWidgetTransitive(LViewport const* Viewport) const
{
    if (Viewport == nullptr)
    {
        return false;
    }

    if (Super::IsFocusWidgetTransitive(Viewport))
    {
        return true;
    }

    for (auto& Child : this->Children)
    {
        if (Child->IsFocusWidgetTransitive(Viewport))
        {
            return true;
        }
    }

    return false;
}

bool Jafg::WParent::FindNodeInVisiblePath(const WNode* Node) const
{
    if (Super::FindNodeInVisiblePath(Node))
    {
        return true;
    }

    if (this->ShouldNowDraw())
    {
        for (auto& Child : this->Children)
        {
            check(Child.get())
            if (Child->FindNodeInVisiblePath(Node))
            {
                return true;
            }
            continue;
        }
    }

    return false;
}

void Jafg::WParent::AddChildAt(u64 Index, TJxxUnique<WNode> Child)
{
    check(Tasks::IsOnMasterThread())
    check(Child.get())
    check(algo::contains(this->GetChildren(), &*Child, [](auto const& E){return &*E;}) == false)
    check(Child->_HasBegunLife() == false)

    Child->_SetParentDangerous(this);
    WNode& InsertedChild{**this->Children.insert(this->Children.begin() + Index, std::move(Child))};

    if (WUserWidget* UserWidget{DynamicCast<WUserWidget>(this->GetMostOuterParent())})
    {
        if (UserWidget->IsTopLevel())
        {
            check(UserWidget->_HasBegunLife())
            check(this->_HasBegunLife())
            check(InsertedChild._HasBegunLife() == false)
            MakeCxxObjectFinal(InsertedChild);
        }
        else
        {
            check(this->_HasBegunLife() == false)
            check(InsertedChild._HasBegunLife() == false)
        }
    }

    return;
}
