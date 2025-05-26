// Copyright mzoesch. All rights reserved.

#include "Widgets/Button.h"
#include "Widgets/TextBlock.h"

Jafg::WButton::WButton(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
    if (LCursorReply Reply = Super::OnCursorEnter(); Reply.IsHandled())
    {
        return Reply;
    }

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.HoverBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WButton::OnCursorLeave()
{
    if (LCursorReply Reply = Super::OnCursorLeave(); Reply.IsHandled())
    {
        return Reply;
    }

    if (this->IsEnabled() && this->bLetUiReactToEvents)
    {
        this->SetBrush(this->Style.NormalBrush);
    }

    return LCursorReply::Handled();
}

Jafg::LReply Jafg::WButton::OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

Jafg::LReply Jafg::WButton::OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

void Jafg::WButton::SetEnabled(const bool bInEnabled)
{
    this->bEnabled = bInEnabled;

    if (this->bEnabled)
    {
        this->SetBrush(this->Style.NormalBrush);
    }
    else
    {
        this->SetBrush(this->Style.DisabledBrush);
    }

    return;
}

Jafg::WTextButton::WTextButton(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetPadding({15, 8});
    return;
}

void Jafg::WTextButton::Construct()
{
    if (this->ButtonText == nullptr)
    {
        NewNode(WTextBlock).SaveTo(&this->ButtonText)
            .Align(ETextHAlign::Center)
            .Align(ETextVAlign::Center)
            .Brush(LTextBlockBrush::SubHeader());

        this->AddChild(this->ButtonText);
    }

    check( this->GetChildren().GetSize() > 0 )

    if (this->IntermediateContent.IsEmpty() == false)
    {
        this->ButtonText->SetContent(std::move(this->IntermediateContent));
        check( this->IntermediateContent.IsEmpty() )
    }

    if (this->IntermediateTextBlockBrush.IsSet())
    {
        this->ButtonText->SetBrush(this->IntermediateTextBlockBrush.GetValue());
        this->IntermediateTextBlockBrush.Reset();
        check( this->IntermediateTextBlockBrush.IsSet() == false )
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

bool Jafg::WTextButton::SetTextBlockBrush(const LTextBlockBrush& InBrush)
{
    if (this->ButtonText)
    {
        this->ButtonText->SetBrush(InBrush);
        return true;
    }

    this->IntermediateTextBlockBrush = InBrush;
    return false;
}

bool Jafg::WTextButton::LoadIntermediateContent()
{
    if (this->IntermediateContent.IsEmpty() && this->ButtonText)
    {
        this->ButtonText->SetContent(std::move(this->IntermediateContent));
        check( this->IntermediateContent.IsEmpty() )
        return true;
    }

    return false;
}
