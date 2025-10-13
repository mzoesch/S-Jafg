// Copyright mzoesch. All rights reserved.

#include "Widgets/Parent.h"
#include "Widgets/Viewport.h"

Jafg::WParent::WParent(LCxxObjectInitializer const& ObjectInitializer)
    : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    return;
}

void Jafg::WParent::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    // for (const LWidgetSlot* ChildSlot : this->Children)
    // {
    //     checkSlow( ChildSlot->Content )
    //     *ChildSlot->Content->GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() = nullptr;
    //     ChildSlot->Content->MarkAsGarbage();
    //     delete ChildSlot;
    // }
    //
    // this->Children.Empty();

    Super::OnGarbage(Reason);

    return;
}

void Jafg::WParent::Construct()
{
    Super::Construct();

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        checkSlow( ChildSlot->Content )
        MakeDeferredWidgetNodeFinal(ChildSlot->Content);
    }

    return;
}

void Jafg::WParent::Tick()
{
    Super::Tick();

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        checkSlow( ChildSlot->Content )
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
    Super::Destruct();

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        *ChildSlot->Content->GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() = nullptr;
        ChildSlot->Content->MarkAsGarbage_v2();
        delete ChildSlot;
    }

    algo::orphan(&this->Children);

    return;
}

void Jafg::WParent::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->ShouldNowDraw())
        {
            checkSlow( ChildSlot->Content )
            ChildSlot->Content->Draw(Context);
        }

        continue;
    }

    return;
}

Jafg::LCursorReply Jafg::WParent::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->ShouldCheckForInputs())
        {
            if (const LCursorReply Reply { ChildSlot->Content->SweepMouse(Context, InLocation) }; Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WParent::SweepFocusTest(const LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepFocusTest(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (const LReply Reply = ChildSlot->Content->SweepFocusTest(Context, InLocation); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::SweepFocusTest(Context, InLocation);
}

Jafg::LReply Jafg::WParent::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        check( ChildSlot->Content )

        if (ChildSlot->Content == InViewport.GetFocusedWidget())
        {
            continue;
        }

        if (ChildSlot->Content->ShouldCheckForInputs() == false)
        {
            continue;
        }

        if (ChildSlot->Content->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked()) == false)
        {
            continue;
        }

        if (const LReply Reply = ChildSlot->Content->OnKeyDownNoFocus(InViewport, InKeyEvent); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::OnKeyDownNoFocus(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WParent::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        check( ChildSlot->Content )

        if (ChildSlot->Content == InViewport.GetFocusedWidget())
        {
            continue;
        }

        if (ChildSlot->Content->ShouldCheckForInputs() == false)
        {
            continue;
        }

        if (ChildSlot->Content->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked()) == false)
        {
            continue;
        }

        if (const LReply Reply = ChildSlot->Content->OnKeyUpNoFocus(InViewport, InKeyEvent); Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::OnKeyUpNoFocus(InViewport, InKeyEvent);
}

bool Jafg::WParent::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (InViewport == nullptr)
    {
        return false;
    }

    if (Super::IsFocusWidgetTransitive(InViewport))
    {
        return true;
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->IsFocusWidgetTransitive(InViewport))
        {
            return true;
        }

        continue;
    }

    return false;
}

bool Jafg::WParent::FindNodeInVisiblePath(const WNode* InNode) const
{
    if (Super::FindNodeInVisiblePath(InNode))
    {
        return true;
    }

    if (this->ShouldNowDraw() == false)
    {
        return false;
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->FindNodeInVisiblePath(InNode))
        {
            return true;
        }

        continue;
    }

    return false;
}

void Jafg::WParent::RemoveChild(WNode* Child)
{
    for (LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content == Child)
        {
            *ChildSlot->Content->GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() = nullptr;
            ChildSlot->Content->MarkAsGarbage_v2();
            algo::erase_once_checked(&this->Children, ChildSlot);
            delete ChildSlot;
            check( algo::find(this->GetChildren(), Child, [](auto const& E){ return E->Content; }) == this->GetChildren().end() )

            return;
        }

        continue;
    }

    panic( "The in child is not a child of this widget." )

    return;
}

void Jafg::WParent::RemoveChild(LWidgetSlot* Child)
{
    this->RemoveChild(Child->Content);
}

void Jafg::WParent::RemoveChildAt(const i32 InIndex)
{
    this->RemoveChild(this->GetChildren()[InIndex]);
    return;
}

void Jafg::WParent::RemoveChildren()
{
    check( Tasks::IsOnMasterThread() )

    while (this->Children.empty() == false)
    {
        checkSlow( this->Children.back() )
        this->RemoveChild(this->Children.back());
        continue;
    }

    return;
}

Jafg::LWidgetSlot* Jafg::WParent::AddChild(WNode* InChild)
{
    check( algo::find(this->GetChildren(), InChild, [](auto const& E){ return E->Content; }) == this->GetChildren().end() )

    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.push_back(NewChildSlot);
    *NewChildSlot->Content->GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}

Jafg::LWidgetSlot* Jafg::WParent::AddChildAt(const i32 InIndex, WNode* InChild)
{
    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.insert(this->Children.begin() + InIndex, NewChildSlot);
    *NewChildSlot->Content->GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}

void Jafg::WParent::MakeChildrenFinal()
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        MakeDeferredWidgetNodeFinal(ChildSlot->Content);
    }

    return;
}
