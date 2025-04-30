// Copyright mzoesch. All rights reserved.

#include "Widgets/EditableTextBlock.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Widgets/Viewport.h"

Jafg::LString Jafg::LexToString(const ETextCommit::Type InType)
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

    return;
}

void Jafg::WEditableTextBlock::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    LVector2 AnchoredTopLeftFromMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context);

    if (this->Content.IsEmpty() == false && (this->GetDesiredSize().X > 0.0f && this->GetDesiredSize().Y > 0.0f))
    {
        LFontShaderContextDrawArgs Args;
        Args.Content     = &this->Content;
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

    if (this->GetLocalEgo()->GetUserInput()->HasBufferedPlatformInput())
    {
        const LString::T* Input = this->GetLocalEgo()->GetUserInput()->GetBufferedPlatformInput().ToPtr();
        this->Content.AppendAt(this->CaretCursor, Input);

        const LString::SizeType Length = LString::Traits::GetStringLength<LString::SizeType>(Input);
        for (LString::SizeType I = 0; I < Length; ++I)
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
            const LString::SizeType Removed = this->Content.RemoveCharacterAt(this->CaretCursor - 1);
            for (LString::SizeType I = 0; I < Removed; ++I)
            {
                this->SafelyReduceCaretCursor();
            }
            this->CaretBlinker = 0.0f;
        }
        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Left)
    {
        if (this->Content.IsValidIndex(this->CaretCursor-1))
        {
            const LString::SizeType Size = this->Content.GetRuneCountOfCharacterAt(this->CaretCursor);
            for (LString::SizeType I = 0; I < Size; ++I)
            {
                this->SafelyReduceCaretCursor();
            }
        }
        this->CaretBlinker = 0.0f;
        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Right)
    {
        if (this->Content.IsValidIndex(this->CaretCursor))
        {
            const LString::SizeType Size = this->Content.GetRuneCountOfCharacterAt(this->CaretCursor);
            for (LString::SizeType I = 0; I < Size; ++I)
            {
                this->SafelyIncreaseCaretCursor();
            }
        }
        this->CaretBlinker = 0.0f;
        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::NumPadEnter)
    {
        this->OnTextCommit(this->Content, ETextCommit::OnEnter);
        return LReply::Handled();
    }

    return Super::OnKeyDown(InKeyEvent);
}

void Jafg::WEditableTextBlock::OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType)
{
    this->OnTextCommitted.InvokeIfBound(InText, InCommitType);
}

void Jafg::WEditableTextBlock::SetText(const LString& InText)
{
    this->Content = InText;
    this->CaretCursor = Maths::Min(this->CaretCursor, this->Content.GetRuneCount());

    if (this->Content.GetByteSize() == 0)
    {
        this->Content = "";
        check( this->Content.GetByteSize() > 0 )
    }

    this->SetCaretCursorToEnd();

    return;
}

void Jafg::WEditableTextBlock::SetText(LString&& InText)
{
    this->Content = std::move(InText);
    this->CaretCursor = Maths::Min(this->CaretCursor, this->Content.GetRuneCount());
    this->SetCaretCursorToEnd();

    return;
}

void Jafg::WEditableTextBlock::ClearText()
{
    this->Content.Empty();
    this->CaretCursor = 0;

    return;
}

i32 Jafg::WEditableTextBlock::SetCaretCursor(const i32 InCaretCursor)
{
    this->CaretCursor = Maths::Clamp(InCaretCursor, 0, this->Content.GetRuneCount());
    return this->CaretCursor;
}

i32 Jafg::WEditableTextBlock::SetCaretCursorToBegin()
{
    this->CaretCursor = 0;
    return this->CaretCursor;
}

i32 Jafg::WEditableTextBlock::SetCaretCursorToEnd()
{
    this->CaretCursor = this->Content.GetRuneCount();
    return this->CaretCursor;
}

void Jafg::WEditableTextBlock::SafelyReduceCaretCursor()
{
    this->CaretCursor = Maths::Max(this->CaretCursor - 1, 0);
}

void Jafg::WEditableTextBlock::SafelyIncreaseCaretCursor()
{
    this->CaretCursor = Maths::Min(this->CaretCursor + 1, this->Content.GetRuneCount());
}
