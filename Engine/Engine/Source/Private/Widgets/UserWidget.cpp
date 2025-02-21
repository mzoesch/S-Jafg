// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

Jafg::WUserWidget::WUserWidget(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    this->SetAnchor(EAnchor::Fill);
    this->SetShouldTick(false);

    return;
}

void Jafg::WUserWidget::OnGarbage()
{
    if (this->Root)
    {
        checkSlow( this->Root->Content )
        this->Root->Content->MarkAsGarbage();
        delete this->Root;
        this->Root = nullptr;
    }

    Super::OnGarbage();

    return;
}

void Jafg::WUserWidget::Construct()
{
    Super::Construct();

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        MakeDeferredWidgetNodeFinal(this->Root->Content);
    }

    return;
}

void Jafg::WUserWidget::Tick()
{
    Super::Tick();

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->ShouldNowTick())
        {
            this->Root->Content->Tick();
        }
    }

    return;
}

void Jafg::WUserWidget::Destruct()
{
    Super::Destruct();

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        this->Root->Content->MarkAsGarbage();
        delete this->Root;
        this->Root = nullptr;
    }

    return;
}

void Jafg::WUserWidget::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->ShouldNowDraw())
        {
            this->Root->Content->Draw(Context);
        }
    }

    return;
}

Jafg::LCursorReply Jafg::WUserWidget::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, InLocation);
    }

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->ShouldCheckForInputs())
        {
            const LCursorReply Reply = this->Root->Content->SweepMouse(Context, InLocation);
            if (Reply.IsHandled())
            {
                return Reply;
            }
        }
    }

    return Super::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WUserWidget::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepFocusTest(Context, InLocation);
    }

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        const LReply Reply = this->Root->Content->SweepFocusTest(Context, InLocation);
        if (Reply.IsHandled())
        {
            return Reply;
        }
    }

    return Super::SweepFocusTest(Context, InLocation);
}

bool Jafg::WUserWidget::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (Super::IsFocusWidgetTransitive(InViewport))
    {
        return true;
    }

    if (InViewport == nullptr)
    {
        return false;
    }

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->IsFocusWidgetTransitive(InViewport))
        {
            return true;
        }
    }

    return false;
}

bool Jafg::WUserWidget::FindNodeInVisiblePath(const WWidgetNode* InNode) const
{
    if (Super::FindNodeInVisiblePath(InNode))
    {
        return true;
    }

    if (this->ShouldNowDraw() == false)
    {
        return false;
    }

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->FindNodeInVisiblePath(InNode))
        {
            return true;
        }
    }

    return false;
}


void Jafg::WUserWidget::UpdateDesiredSize() const
{
    if (this->Root)
    {
        if (this->Root->Content->TransformsWidgetLayout())
        {
            this->Root->Content->UpdateDesiredSize();
        }
        else
        {
            this->Root->Content->SetDesiredSize(LVector2::Zero());
        }
        this->SetDesiredSize(this->Root->Content->GetDesiredSizeSmart() + this->Padding.GetDesiredSize());
    }
    else
    {
        this->SetDesiredSize(LVector2::Zero() + this->Padding.GetDesiredSize());
    }

    Super::UpdateDesiredSize();

    return;
}

Jafg::LVector2 Jafg::WUserWidget::GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const
{
    return this->Padding.GetTopLeftOffset();
}

void Jafg::WUserWidget::UpdateAnchoredSize(const LViewport& Context) const
{
    Super::UpdateAnchoredSize(Context);

    if (this->Root)
    {
        checkSlow( this->Root->Content )
        if (this->Root->Content->TransformsWidgetLayout())
        {
            this->Root->Content->UpdateAnchoredSize(Context);
        }
        else
        {
            this->Root->Content->SetAnchoredSize(LVector2::Zero());
        }
    }

    return;
}

Jafg::LVector2 Jafg::WUserWidget::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    check( this->Root->Content == InDirectChild )

    LVector2 Out = this->GetAnchoredTopLeftFromMostOuter(Context);

    Out += LVector2(this->Anchor.MinX, this->Anchor.MinY)
         * (this->GetAnchoredSize() - InDirectChild->GetAnchoredSize() - this->Padding.GetTopLeftOffset());
    Out += this->Padding.GetTopLeftOffset();

    return Out;
}

Jafg::LViewport* Jafg::WUserWidget::GetViewport() const
{
    if (this->AttachedViewport)
    {
        return this->AttachedViewport;
    }

    return Super::GetViewport();
}

void Jafg::WUserWidget::AddToViewport(LViewport* InViewport)
{
    check( this->AttachedViewport == nullptr ) // Handle this case?
    this->AttachedViewport = InViewport;
    this->AttachedViewport->AddWidget(this);

    return;
}

void Jafg::WUserWidget::RemoveFromParent(const bool bDestroy /* = true */)
{
    Super::RemoveFromParent();

    if (this->AttachedViewport)
    {
        this->AttachedViewport->RemoveWidget(this);
    }

    if (bDestroy)
    {
        this->KillYourSelfNow(true);
    }

    return;
}

void Jafg::WUserWidget::RemoveChild(WWidgetNode* InChild)
{
    check( this->SingleRootChild.GetSize() == 1 )

    if (this->SingleRootChild[0]->Content == InChild)
    {
        this->RemoveChild(this->SingleRootChild[0]);
        return;
    }

    panic( "The in child is not the root of this widget." )

    return;
}

void Jafg::WUserWidget::RemoveChild(LWidgetSlot* InSlot)
{
    check( this->SingleRootChild.GetSize() == 1 )

    if (this->SingleRootChild[0] == InSlot)
    {
        this->SingleRootChild.Empty();
        this->Root = nullptr;
        delete InSlot;
    }
    else
    {
        panic( "The in slot is not the root of this widget." )
    }

    return;
}

Jafg::WWidgetParent* Jafg::WUserWidget::ReplaceRootImpl(WWidgetParent& InRoot)
{
    check( this->Slot == nullptr )

    if (this->HasRoot())
    {
        check( this->Root )
        check( this->Root == this->SingleRootChild[0] )
        check( this->Root->Parent == this )
        check( this->Root->Content )
        this->Root->Content->RemoveFromParent();
        check( this->Root == nullptr )
        check( this->SingleRootChild.IsEmpty() )
    }

    check( InRoot.Slot == nullptr )
    check( this->Root == nullptr )
    check( this->SingleRootChild.IsEmpty() )

    this->SingleRootChild.Emplace(new LWidgetSlot(this, &InRoot));
    // this->Root = new LWidgetSlot(this, &InRoot);
    this->Root = this->SingleRootChild[0];

    this->Root->Content->Slot = this->Root;
    this->Root->Margin = this->GetPaddingPtr();

    check( this->Root == this->SingleRootChild[0] )

    return &InRoot;
}
