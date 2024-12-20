// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/EditableTextBlock.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
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
    this->SetShouldTick(false);
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);

    return;
}

void Jafg::WEditableTextBlock::Construct()
{
    Super::Construct();

    this->ShaderContext.Make();
    this->CaretShaderContext.Make();
    this->Content = "";

    return;
}

void Jafg::WEditableTextBlock::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    LVector2 AnchoredTopLeftFromMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context, this);

    if (this->Content.IsEmpty() == false && (this->GetDesiredSize().X > 0.0f && this->GetDesiredSize().Y > 0.0f))
    {
        const LSimpleString ConvContent = Str::ToSimpleString(this->Content);

        LFontShaderContextDrawArgs Args;
        Args.Content     = &ConvContent;
        Args.Offset      = AnchoredTopLeftFromMostOuter;
        Args.Padding     = this->GetPadding();
        Args.DesiredSize = this->GetDesiredSize();
        Args.Color       = this->Color;
        Args.Scale       = this->Scale;
        this->ShaderContext.Draw(Context, Args);
    }

    if (this->CaretBlinker < this->CaretBlinkerSpeed && this->IsFocusWidget())
    {
        LVector2 CaretSize    = LVector2(2.0f, this->GetDesiredSize().Y) * this->CaretBrush.Size;
        LVector2 CaretTopLeft = AnchoredTopLeftFromMostOuter + LVector2(0.0f, (this->GetDesiredSize().Y - CaretSize.Y) * 0.5f);

        LString CaretContent;
        if (this->Content.GetRuneCount() != this->CaretCursor)
        {
            CaretContent = this->Content.Cut(this->CaretCursor);
        }
        else
        {
            CaretContent = this->Content;
        }

        CaretTopLeft.X += LFontShaderContext::GetDesiredWidth(CaretContent, this->Scale) + this->CaretBrush.HOffset;

        this->CaretShaderContext.Draw(
            Context,
            CaretSize,
            CaretTopLeft,
            this->CaretBrush.Color
        );
    }

    return;
}

void Jafg::WEditableTextBlock::Tick()
{
    Super::Tick();

    if (this->GetLocalEgo()->GetPrimarySurface()->HasBufferedPlatformInput())
    {
        const char* Input = this->GetLocalEgo()->GetPrimarySurface()->GetPlatformInput().ToPtr();
        this->Content.AppendAt(this->CaretCursor, Input);
        const LEightStringTraits::SizeType Count = LEightStringTraits::GetRuneCount(Input);
        for (LEightStringTraits::SizeType I = 0; I < Count; ++I)
        {
            this->SafelyIncreaseCaretCursor();
        }
        this->CaretBlinker = 0.0f;
    }
    else
    {
        this->CaretBlinker += Application::GetDeltaTimeAsFloat();
        if (this->CaretBlinker > this->CaretBlinkerSpeed * 2.0f)
        {
            this->CaretBlinker = 0.0f;
        }
    }

    return;
}

void Jafg::WEditableTextBlock::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    this->SetDesiredSize(LFontShaderContext::GetDesiredSize(this->Content, this->Scale) + this->GetDesiredSize());
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
    this->SetShouldTick(true);
    this->CaretBlinker = 0.0f;
    return;
}

void Jafg::WEditableTextBlock::OnFocusLost()
{
    Super::OnFocusLost();
    this->SetShouldTick(false);
    return;
}

Jafg::LReply Jafg::WEditableTextBlock::OnKeyDown(LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::BackSpace || InKeyEvent.GetKey() == EKeys::PlatformDelete)
    {
        if (this->Content.IsEmpty() == false && this->CaretCursor > 0)
        {
            this->Content.RemoveAt(this->CaretCursor - 1);
            this->SafelyReduceCaretCursor();
            this->CaretBlinker = 0.0f;
        }
    }

    if (InKeyEvent.GetKey() == EKeys::Left)
    {
        this->SafelyReduceCaretCursor();
        this->CaretBlinker = 0.0f;
    }

    if (InKeyEvent.GetKey() == EKeys::Right)
    {
        this->SafelyIncreaseCaretCursor();
        this->CaretBlinker = 0.0f;
    }

    return LReply::Handled();
}

void Jafg::WEditableTextBlock::OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType)
{
    LOG_WARNING(LogTemporal, "{}: {}", LexToString(InCommitType), InText)
}

void Jafg::WEditableTextBlock::SafelyReduceCaretCursor()
{
    this->CaretCursor = Maths::Max(this->CaretCursor - 1, 0);
}

void Jafg::WEditableTextBlock::SafelyIncreaseCaretCursor()
{
    this->CaretCursor = Maths::Min(this->CaretCursor + 1, this->Content.GetRuneCount());
}
