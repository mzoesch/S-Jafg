// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetParentBase.h"
#include "Widgets/Viewport.h"

Jafg::WWidgetParentBase::WWidgetParentBase(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    return;
}

void Jafg::WWidgetParentBase::Tick()
{
    Super::Tick();

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->ShouldNowTick())
        {
            ChildSlot->Content->Tick();
        }

        continue;
    }

    return;
}

Jafg::LCursorReply Jafg::WWidgetParentBase::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
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

Jafg::LReply Jafg::WWidgetParentBase::SweepFocusTest(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepFocusTest(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        const LReply Reply = ChildSlot->Content->SweepFocusTest(Context, InLocation);
        if (Reply.IsHandled())
        {
            return Reply;
        }

        continue;
    }

    return WWidgetNode::SweepFocusTest(Context, InLocation);
}

bool Jafg::WWidgetParentBase::IsFocusWidgetTransitive() const
{
    const LViewport* Viewport = this->GetViewport();
    if (Viewport == nullptr)
    {
        return false;
    }

    return this->IsFocusWidgetTransitive(Viewport);
}

bool Jafg::WWidgetParentBase::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (Super::IsFocusWidgetTransitive(InViewport))
    {
        return true;
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->IsFocusWidgetTransitive(InViewport))
        {
            return true;
        }

        continue;
    }

    return false;
}

void Jafg::WWidgetParentBase::UpdateDesiredSize() const
{
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateDesiredSize();
    }

    Super::UpdateDesiredSize();

    return;
}

void Jafg::WWidgetParentBase::UpdateAnchoredSize(const LViewport& Context) const
{
    Super::UpdateAnchoredSize(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateAnchoredSize(Context);
    }

    return;
}

void Jafg::WWidgetParentBase::UpdateAnchoredSizeOfChildren(const LViewport& Context) const
{
    Super::UpdateAnchoredSizeOfChildren(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateAnchoredSize(Context);
    }

    return;
}

bool Jafg::WWidgetParentBase::FindNodeInVisiblePath(const WWidgetNode* InNode) const
{
    if (Super::FindNodeInVisiblePath(InNode))
    {
        return true;
    }

    if (this->ShouldNowDraw() == false)
    {
        return false;
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->FindNodeInVisiblePath(InNode))
        {
            return true;
        }

        continue;
    }

    return false;
}
