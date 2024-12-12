// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"

Jafg::LSimpleString Jafg::LexToString(const ETextCommit::Type InType)
{
    switch (InType)
    {
    case ETextCommit::OnEnter:   { return "OnEnter"; }
    case ETextCommit::OnCleared: { return "OnCleared"; }
    case ETextCommit::FocusLost: { return "FocusLost"; }
    default:                     { checkNoEntry() return { }; }
    }
}

Jafg::WEditableTextBlock::WEditableTextBlock(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WEditableTextBlock::Construct()
{
    Super::Construct();
    this->ShaderContext.Make();
    this->Content = "";

    return;
}

void Jafg::WEditableTextBlock::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    if (this->Content.IsEmpty() == false && (this->GetDesiredSize().X > 0.0f && this->GetDesiredSize().Y > 0.0f))
    {
        LFontShaderContextDrawArgs Args;
        Args.Content     = &this->Content;
        Args.Offset      = this->GetAnchoredTopLeftFromMostOuter(Context, this);
        Args.Padding     = this->GetPadding();
        Args.DesiredSize = this->GetDesiredSize();
        Args.Color       = this->Color;
        Args.Scale       = this->Scale;
        this->ShaderContext.Draw(Context, Args);
    }

    return;
}

void Jafg::WEditableTextBlock::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize;
    if (LFontShaderContext::GetDesiredSize(this->Content, this->Scale, DesiredSize))
    {
        this->SetDesiredSize(this->GetDesiredSize() + DesiredSize);
    }
    else
    {
        this->SetDesiredSize(this->GetDesiredSize() + LVector2(0.0f, LFontShaderContext::GetApproximateHeight(this->Scale)));
    }

    return;
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorEnter()
{
    return { EMouseCursor::Beam };
}

Jafg::LCursorReply Jafg::WEditableTextBlock::OnCursorLeave()
{
    return { EMouseCursor::Default };
}

void Jafg::WEditableTextBlock::OnFocusReceived()
{
    Super::OnFocusReceived();
}

void Jafg::WEditableTextBlock::OnFocusLost()
{
    Super::OnFocusLost();
}

Jafg::LReply Jafg::WEditableTextBlock::OnKeyDown(LKeyEvent& InKeyEvent)
{
    LOG_WARNING(LogTemporal, "{}", LexToString(InKeyEvent.GetKey()))
    this->Content += LexToString(InKeyEvent.GetKey());
    return LReply::Handled();
}

void Jafg::WEditableTextBlock::OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType)
{
    LOG_WARNING(LogTemporal, "{}: {}", LexToString(InCommitType), InText)
}
