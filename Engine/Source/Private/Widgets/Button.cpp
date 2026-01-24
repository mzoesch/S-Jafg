// Copyright mzoesch. All rights reserved.

#include "Widgets/Button.h"

Jafg::WButton::WButton(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::DerivedHitTestInvisible);
    return;
}

void Jafg::WButton::Construct()
{
    Super::Construct();

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    return;
}

Jafg::LCursorReply Jafg::WButton::OnCursorEnter()
{
    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.HoverBrush);
    }

    if (this->OnCursorEnterEvent.IsValid())
    {
        if (LCursorReply Reply{ this->OnCursorEnterEvent.Invoke(*this) }; Reply.IsHandled())
        {
            return Reply;
        }
    }

    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WButton::OnCursorLeave()
{
    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    if (this->OnCursorLeaveEvent.IsValid())
    {
        if (LCursorReply Reply{ this->OnCursorLeaveEvent.Invoke(*this) }; Reply.IsHandled())
        {
            return Reply;
        }
    }

    return LCursorReply::Handled();
}

Jafg::LReply Jafg::WButton::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

Jafg::LReply Jafg::WButton::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

void Jafg::WButton::SetEnabled(const bool bInEnabled)
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

Jafg::WTextButton::WTextButton(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetPadding({4, 2});
    return;
}

void Jafg::WTextButton::Construct()
{
    if (this->ButtonText == nullptr)
    {
        NewNode(WTextBox).SaveTo(&this->ButtonText)
            .TextAlign(ETextHAlign::Center)
            .TextAlign(ETextVAlign::Center)
            .Brush(LTextBoxBrush::SubHeader());

        this->AddChild(this->ButtonText);
    }

    check( this->GetChildren().size() > 0 )

    if (this->IntermediateContent.empty() == false)
    {
        this->ButtonText->SetContent(std::move(this->IntermediateContent));
        check( this->IntermediateContent.empty() )
    }

    if (this->IntermediateTextBoxBrush.has_value())
    {
        this->ButtonText->SetBrush(this->IntermediateTextBoxBrush.value());
        this->IntermediateTextBoxBrush.reset();
        check( this->IntermediateTextBoxBrush.has_value() == false )
    }

    Super::Construct();

    return;
}

void Jafg::WTextButton::Draw(LViewport& Context) const
{
    Super::Draw(Context);
}

bool Jafg::WTextButton::SetContent(const LString& InContent)
{
    if (this->ButtonText)
    {
        this->ButtonText->SetContent(InContent);
        return true;
    }

    this->IntermediateContent = InContent;
    return false;
}

bool Jafg::WTextButton::SetContent(LString&& InContent)
{
    if (this->ButtonText)
    {
        this->ButtonText->SetContent(std::move(InContent));
        return true;
    }

    this->IntermediateContent = std::move(InContent);
    return false;
}

bool Jafg::WTextButton::SetTextBoxBrush(const LTextBoxBrush& InBrush)
{
    if (this->ButtonText)
    {
        this->ButtonText->SetBrush(InBrush);
        return true;
    }

    this->IntermediateTextBoxBrush = InBrush;
    return false;
}

bool Jafg::WTextButton::LoadIntermediateContent()
{
    if (this->IntermediateContent.empty() && this->ButtonText)
    {
        this->ButtonText->SetContent(std::move(this->IntermediateContent));
        check( this->IntermediateContent.empty() )
        return true;
    }

    return false;
}
