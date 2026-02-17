// Copyright mzoesch. All rights reserved.

#include "Widgets/Parent.h"
#include "Widgets/Viewport.h"

void Jafg::WParent::Construct()
{
    Super::Construct();

    for (LWidgetSlot const* ChildSlot : this->GetChildren())
    {
        check(ChildSlot->Content)
        check(ChildSlot->Content->_HasBegunLife() == false)
        MakeCxxObjectFinal(*ChildSlot->Content);
        check(ChildSlot->Content->_HasBegunLife())
        continue;
    }

    return;
}

void Jafg::WParent::Tick()
{
    Super::Tick();

    for (LWidgetSlot const* ChildSlot : this->GetChildren())
    {
        check(ChildSlot->Content)
        if (ChildSlot->Content->ShouldNowTick())
        {
            ChildSlot->Content->Tick();
        }
        continue;
    }

    return;
}

void Jafg::WParent::Destruct()
{
    check(Tasks::IsOnMasterThread())
    Super::Destruct();

    algo::for_each(this->Children,
    [
#if JAFG_DO_CHECKS
        this
#endif  /* JAFG_DO_CHECKS */
    ](LWidgetSlot* ChildSlot)
    {
        check(ChildSlot && ChildSlot->Content && ChildSlot->Parent == this)
        auto* ChildWidget{ChildSlot->Content};
        ChildSlot->Content->InvalidateSlotDangerous();
        check(ChildSlot->Parent == nullptr && ChildSlot->Content == nullptr && ChildSlot->Margin == nullptr)
        ChildWidget->MarkAsGarbage_v2();
        return;
    });

    algo::orphan(&this->Children);

    return;
}

void Jafg::WParent::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    for (LWidgetSlot const* ChildSlot : this->GetChildren())
    {
        check(ChildSlot->Content)
        if (ChildSlot->Content->ShouldNowDraw())
        {
            ChildSlot->Content->Draw(Context);
        }
        continue;
    }

    return;
}

Jafg::LCursorReply Jafg::WParent::SweepMouse(LViewport& Context, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, Location);
    }

    for (auto It{ this->Children.rbegin() }; It != this->Children.rend(); ++It)
    {
        if ((*It)->Content->ShouldCheckForInputs())
        {
            if (const LCursorReply Reply{ (*It)->Content->SweepMouse(Context, Location) }; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::SweepMouse(Context, Location);
}

Jafg::LReply Jafg::WParent::SweepFocusTest(const LViewport& Context, const LVec2F& Location)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepFocusTest(Context, Location);
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (const LReply Reply = ChildSlot->Content->SweepFocusTest(Context, Location); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::SweepFocusTest(Context, Location);
}

Jafg::LReply Jafg::WParent::OnKeyDownNoFocus(const LViewport& Viewport, LKeyEvent const& KeyEvent)
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        check( ChildSlot->Content )

        if (ChildSlot->Content == Viewport.GetFocusedWidget())
        {
            continue;
        }

        if (ChildSlot->Content->ShouldCheckForInputs() == false)
        {
            continue;
        }

        if (ChildSlot->Content->IsInBounds(Viewport, *Viewport.GetCachedCursorLocationChecked()) == false)
        {
            continue;
        }

        if (const LReply Reply = ChildSlot->Content->OnKeyDownNoFocus(Viewport, KeyEvent); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::OnKeyDownNoFocus(Viewport, KeyEvent);
}

Jafg::LReply Jafg::WParent::OnKeyUpNoFocus(const LViewport& Viewport, const LKeyEvent& KeyEvent)
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        check( ChildSlot->Content )

        if (ChildSlot->Content == Viewport.GetFocusedWidget())
        {
            continue;
        }

        if (ChildSlot->Content->ShouldCheckForInputs() == false)
        {
            continue;
        }

        if (ChildSlot->Content->IsInBounds(Viewport, *Viewport.GetCachedCursorLocationChecked()) == false)
        {
            continue;
        }

        if (const LReply Reply = ChildSlot->Content->OnKeyUpNoFocus(Viewport, KeyEvent); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::OnKeyUpNoFocus(Viewport, KeyEvent);
}

bool Jafg::WParent::IsFocusWidgetTransitive(const LViewport* Viewport) const
{
    if (Viewport == nullptr)
    {
        return false;
    }

    if (Super::IsFocusWidgetTransitive(Viewport))
    {
        return true;
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->IsFocusWidgetTransitive(Viewport))
        {
            return true;
        }

        continue;
    }

    return false;
}

bool Jafg::WParent::FindNodeInVisiblePath(const WNode* Node) const
{
    if (Super::FindNodeInVisiblePath(Node))
    {
        return true;
    }

    if (this->ShouldNowDraw() == false)
    {
        return false;
    }

    for (LWidgetSlot const* ChildSlot : this->Children)
    {
        check(ChildSlot->Content)
        if (ChildSlot->Content->FindNodeInVisiblePath(Node))
        {
            return true;
        }
        continue;
    }

    return false;
}

void Jafg::WParent::RemoveChild(WNode* Child)
{
    check(Tasks::IsOnMasterThread())
    check(Child)

    auto It{algo::find(this->Children, Child, [](auto const& E){return E->Content;})};

    if (It == this->Children.end())
    {
        panic("The in child is not a child of this widget.")
    }

    auto* ChildWidget{(*It)->Content};

    ChildWidget->InvalidateSlotDangerous();
    check((*It)->Parent == nullptr && (*It)->Content == nullptr && (*It)->Margin == nullptr)
    ChildWidget->MarkAsGarbage_v2();

    this->Children.erase(It);
    check(algo::find(this->GetChildren(), Child, [](auto const& E){return E->Content;}) == this->GetChildren().end())

    return;
}

void Jafg::WParent::RemoveChild(LWidgetSlot* Child)
{
    this->RemoveChild(Child->Content);
}

void Jafg::WParent::RemoveChildAt(const i32 InIndex)
{
    this->RemoveChild(this->GetChildren()[InIndex]);
}

void Jafg::WParent::RemoveChildren()
{
    check(Tasks::IsOnMasterThread())

    while (this->Children.empty() == false)
    {
        check(this->Children.back())
        this->RemoveChild(this->Children.back());
        continue;
    }

    return;
}

Jafg::LWidgetSlot* Jafg::WParent::AddChild(WNode* Child)
{
    return this->AddChildAt(this->Children.size(), Child);
}

Jafg::LWidgetSlot* Jafg::WParent::AddChildAt(const i32 Index, WNode* Child)
{
    check(Tasks::IsOnMasterThread())
    check(this->_HasBegunLife())
    check(Child)
    check(algo::find(this->GetChildren(), Child, [](auto const& E){return E->Content;}) == this->GetChildren().end())
    check(Child->_HasBegunLife() == false)
    check(Child->GetSlot().Parent == nullptr && Child->GetSlot().Content == nullptr && Child->GetSlot().Margin == nullptr)

    Child->GetMutableSlot().Parent  = this;
    Child->GetMutableSlot().Content = Child;
    Child->GetMutableSlot().Margin  = this->GetPaddingPtr();

    this->Children.insert(this->Children.begin() + Index, &Child->GetMutableSlotChecked());

    MakeCxxObjectFinal(*Child);
    check(Child->_HasBegunLife())

#if JAFG_DO_CHECKS
    auto* Result{this->Children[Index]};
    check(Result->Parent == this && Result->Content == Child && Result->Margin == this->GetPaddingPtr())
    return Result;
#else /* JAFG_DO_CHECKS */
    return this->Children[Index];
#endif /* !JAFG_DO_CHECKS */
}
