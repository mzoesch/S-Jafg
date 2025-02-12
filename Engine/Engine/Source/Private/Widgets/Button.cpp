// Copyright mzoesch. All rights reserved.

#include "Widgets/Button.h"

Jafg::WButton::WButton(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::DerivedHitTestInvisible);
    return;
}

void Jafg::WButton::Construct()
{
    Super::Construct();

    if (this->bLetUiReactToEvents)
    {
        this->SetBrush(this->NormalBrush);
    }

    return;
}

Jafg::LCursorReply Jafg::WButton::OnCursorEnter()
{
    if (LCursorReply Reply = Super::OnCursorEnter(); Reply.IsHandled())
    {
        return Reply;
    }

    if (this->bLetUiReactToEvents)
    {
        this->SetBrush(this->HoverBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WButton::OnCursorLeave()
{
    if (LCursorReply Reply = Super::OnCursorLeave(); Reply.IsHandled())
    {
        return Reply;
    }

    if (this->bLetUiReactToEvents)
    {
        this->SetBrush(this->NormalBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LReply Jafg::WButton::OnKeyDown(LKeyEvent& InKeyEvent)
{
    if (LReply Reply = Super::OnKeyDown(InKeyEvent); Reply.IsHandled())
    {
        return Reply;
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->PressBrush);
        }
        this->OnPrimaryPress();
        return LReply::Handled();
    }
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->PressBrush);
        }
        this->OnSecondaryPress();
        return LReply::Handled();
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WButton::OnKeyUp(LKeyEvent& InKeyEvent)
{
    if (LReply Reply = Super::OnKeyUp(InKeyEvent); Reply.IsHandled())
    {
        return Reply;
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->HoverBrush);
        }
        this->OnPrimaryUnpress();
        return LReply::Handled();
    }
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->HoverBrush);
        }
        this->OnSecondaryUnpress();
        return LReply::Handled();
    }

    return LReply::Unhandled();
}
