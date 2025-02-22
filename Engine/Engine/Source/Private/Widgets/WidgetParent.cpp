// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetParent.h"

Jafg::WWidgetParent::WWidgetParent(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    return;
}

void Jafg::WWidgetParent::OnGarbage()
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        checkSlow( ChildSlot->Content )
        ChildSlot->Content->Slot = nullptr;
        ChildSlot->Content->MarkAsGarbage();
        delete ChildSlot;
    }

    this->Children.Empty();

    Super::OnGarbage();

    return;
}

void Jafg::WWidgetParent::Construct()
{
    Super::Construct();

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        checkSlow( ChildSlot->Content )
        MakeDeferredWidgetNodeFinal(ChildSlot->Content);
    }

    return;
}

void Jafg::WWidgetParent::Tick()
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

void Jafg::WWidgetParent::Destruct()
{
    Super::Destruct();

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        ChildSlot->Content->Slot = nullptr;
        ChildSlot->Content->MarkAsGarbage();
        delete ChildSlot;
    }

    this->Children.Empty();

    return;
}

void Jafg::WWidgetParent::Draw(LViewport& Context) const
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

Jafg::LCursorReply Jafg::WWidgetParent::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->ShouldCheckForInputs())
        {
            const LCursorReply Reply = ChildSlot->Content->SweepMouse(Context, InLocation);
            if (Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WWidgetParent::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepFocusTest(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        const LReply Reply = ChildSlot->Content->SweepFocusTest(Context, InLocation);
        if (Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return Super::SweepFocusTest(Context, InLocation);
}

bool Jafg::WWidgetParent::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (Super::IsFocusWidgetTransitive(InViewport))
    {
        return true;
    }

    if (InViewport == nullptr)
    {
        return false;
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

bool Jafg::WWidgetParent::FindNodeInVisiblePath(const WWidgetNode* InNode) const
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

void Jafg::WWidgetParent::UpdateDesiredSize() const
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->TransformsWidgetLayout())
        {
            ChildSlot->Content->UpdateDesiredSize();
        }
        else
        {
            ChildSlot->Content->SetDesiredSize(LVector2::Zero());
        }

        continue;
    }

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        const LVector2 ChildDesiredSize = ChildSlot->Content->GetDesiredSizeSmart();
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildDesiredSize.X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildDesiredSize.Y);
        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

Jafg::LVector2 Jafg::WWidgetParent::GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const
{
    return this->Padding.GetTopLeftOffset();
}

void Jafg::WWidgetParent::UpdateAnchoredSize(const LViewport& Context) const
{
    Super::UpdateAnchoredSize(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->TransformsWidgetLayout())
        {
            ChildSlot->Content->UpdateAnchoredSize(Context);
        }
        else
        {
            ChildSlot->Content->SetAnchoredSize(LVector2::Zero());
        }

        continue;
    }

    return;
}

void Jafg::WWidgetParent::UpdateAnchoredSizeForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )
    checkSlow( InDirectChild->TransformsWidgetLayout() )

    LVector2 Out;

    const LVector2 ParentAnchorSize = this->GetAnchoredSize();
    Out.X = Maths::Max(InDirectChild->Anchor.MaxX * ParentAnchorSize.X - this->Padding.GetDesiredSize().X, InDirectChild->DesiredSize.X);
    Out.Y = Maths::Max(InDirectChild->Anchor.MaxY * ParentAnchorSize.Y - this->Padding.GetDesiredSize().Y, InDirectChild->DesiredSize.Y);

    InDirectChild->SetAnchoredSize(Out);

    return;
}

Jafg::LVector2 Jafg::WWidgetParent::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )

    LVector2 Out = this->GetAnchoredTopLeftFromMostOuter(Context);
    Out += LVector2(InDirectChild->Anchor.MinX, InDirectChild->Anchor.MinY)
         * ((this->GetAnchoredSize() - this->Padding.GetTopLeftOffset()) - InDirectChild->GetAnchoredSize());
    Out += this->Padding.GetTopLeftOffset() * (LVector2::OneVector - LVector2(InDirectChild->Anchor.MinX, InDirectChild->Anchor.MinY));

    return Out;
}

void Jafg::WWidgetParent::RemoveChild(WWidgetNode* Child)
{
    for (LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content == Child)
        {
            ChildSlot->Content->Slot = nullptr;
            ChildSlot->Content->MarkAsGarbage();
            this->Children.RemoveOnceChecked(ChildSlot);
            delete ChildSlot;

            check( this->Children.FindByPredicate([Child] (const LWidgetSlot* Slot)
            {
                return Slot->Content == Child;
            }) == nullptr )

            return;
        }

        continue;
    }

    panic( "The in child is not a child of this widget." )

    return;
}

void Jafg::WWidgetParent::RemoveChild(LWidgetSlot* Child)
{
    this->RemoveChild(Child->Content);
}

Jafg::LWidgetSlot* Jafg::WWidgetParent::AddChild(WWidgetNode* InChild)
{
    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.Add(NewChildSlot);
    NewChildSlot->Content->Slot = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}

Jafg::LWidgetSlot* Jafg::WWidgetParent::AddChildAt(const int32 InIndex, WWidgetNode* InChild)
{
    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.AddAt(InIndex, NewChildSlot);
    NewChildSlot->Content->Slot = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}
