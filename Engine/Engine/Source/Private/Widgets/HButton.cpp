// Copyright mzoesch. All rights reserved.

#include "Widgets/HButton.h"

Jafg::WHButton::WHButton(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    return;
}

Jafg::LCursorReply Jafg::WHButton::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    return Super::SweepMouse(Context, InLocation);
}

void Jafg::WHButton::Construct()
{
    Super::Construct();

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    return;
}

Jafg::LCursorReply Jafg::WHButton::OnCursorEnter()
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

Jafg::LCursorReply Jafg::WHButton::OnCursorLeave()
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

Jafg::LReply Jafg::WHButton::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

        if (this->OnPrimaryPressDelegate.IsBound())
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

        if (this->OnSecondaryPressDelegate.IsBound())
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

Jafg::LReply Jafg::WHButton::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

        if (this->OnPrimaryReleaseDelegate.IsBound())
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

        if (this->OnSecondaryReleaseDelegate.IsBound())
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

void Jafg::WHButton::SetEnabled(const bool bInEnabled)
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

    return;}
