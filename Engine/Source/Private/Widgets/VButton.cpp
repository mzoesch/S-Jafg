// Copyright mzoesch. All rights reserved.

#include "Widgets/VButton.h"

Jafg::WVButton::WVButton(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    return;
}

void Jafg::WVButton::Construct()
{
    Super::Construct();

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    return;
}

Jafg::LCursorReply Jafg::WVButton::OnCursorEnter()
{
    if (LCursorReply Reply { Super::OnCursorEnter() }; Reply.IsHandled())
    {
        return Reply;
    }

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.HoverBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WVButton::OnCursorLeave()
{
    if (LCursorReply Reply { Super::OnCursorLeave() }; Reply.IsHandled())
    {
        return Reply;
    }

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LReply Jafg::WVButton::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->IsEnabled() == false)
    {
        return Super::OnKeyDown(InViewport, InKeyEvent);
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->Style.PressBrush);
        }

        if (this->OnPrimaryPressDelegate.IsValid())
        {
            this->OnPrimaryPressDelegate.Invoke(this, InKeyEvent);
        }
        else
        {
            this->OnPrimaryPress();
        }

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->Style.PressBrush);
        }

        if (this->OnSecondaryPressDelegate.IsValid())
        {
            this->OnSecondaryPressDelegate.Invoke(this, InKeyEvent);
        }
        else
        {
            this->OnSecondaryPress();
        }

        return LReply::Handled();
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WVButton::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->IsEnabled() == false)
    {
        return Super::OnKeyUp(InViewport, InKeyEvent);
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->Style.HoverBrush);
        }

        if (this->OnPrimaryReleaseDelegate.IsValid())
        {
            this->OnPrimaryReleaseDelegate.Invoke(this, InKeyEvent);
        }
        else
        {
            this->OnPrimaryRelease();
        }

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        if (this->bLetUiReactToEvents)
        {
            this->SetBrush(this->Style.HoverBrush);
        }

        if (this->OnSecondaryReleaseDelegate.IsValid())
        {
            this->OnSecondaryReleaseDelegate.Invoke(this, InKeyEvent);
        }
        else
        {
            this->OnSecondaryRelease();
        }

        return LReply::Handled();
    }

    return Super::OnKeyUp(InViewport, InKeyEvent);
}

void Jafg::WVButton::SetEnabled(const bool bInEnabled)
{
    this->bEnabled = bInEnabled;

    if (this->bEnabled)
    {
        this->SetBrush(this->Style.NormalBrush);
        this->SetVisibility(EWidgetVisibility::DerivedHitTestInvisible);
    }
    else
    {
        this->SetBrush(this->Style.DisabledBrush);
        this->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    }

    return;
}
