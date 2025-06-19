// Copyright mzoesch. All rights reserved.

#include "Widgets/EditableTextBlock.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Core/CoreNames.h"
#include "Rhi/OrthographicBoxShader.h"

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
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);
    this->SetPadding({10.0, 8.0});
    this->SetType(ERegionBrush::Box);
    this->SetTint(LColor::Black);
    this->SetTextTint(LColor::White);

    return;
}

void Jafg::WEditableTextBlock::Construct()
{
    Super::Construct();

    this->ShaderContext.Make();

    return;
}

void Jafg::WEditableTextBlock::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    LVector2 AnchoredTopLeftFromMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context);

    if (this->Content.IsEmpty())
    {
        LFontShaderContextDrawArgs Args;
        Args.Content     = &this->Placeholder;
        Args.Offset      = AnchoredTopLeftFromMostOuter;
        Args.Padding     = this->GetBrush().Padding;
        Args.DesiredSize = this->GetDesiredSize();
        Args.Color       = this->PlaceholderColor;
        Args.Scale       = this->TextScale;
        this->ShaderContext.Draw(Context, Args);
    }
    else
    {
        LFontShaderContextDrawArgs Args;
        Args.Content     = &this->Content;
        Args.Offset      = AnchoredTopLeftFromMostOuter;
        Args.Padding     = this->GetBrush().Padding;
        Args.DesiredSize = this->GetDesiredSize();
        Args.Color       = this->TextTint;
        Args.Scale       = this->TextScale;
        this->ShaderContext.Draw(Context, Args);
    }

    if (this->CaretBlinker < this->CaretBlinkerSpeed && this->IsFocusWidget())
    {
        const LVector2 CaretSize { LVector2{2.0f, this->GetDesiredSize().Y} * this->CaretBrush.Size };

        LVector2 CaretTopLeft
        {
              AnchoredTopLeftFromMostOuter
            + LVector2{0.0f, (this->GetDesiredSize().Y - CaretSize.Y) * 0.5f }
            + LVector2{this->GetBrush().Padding.Left, 0.0f}
        }
        ;

        LString CaretContent;
        if (this->Content.GetRuneCount() != this->CaretCursor)
        {
            CaretContent = this->Content.Cut(this->CaretCursor);
        }
        else
        {
            CaretContent = this->Content;
        }

        CaretTopLeft.X += LFontShaderContext::GetDesiredWidth(CaretContent, this->TextScale) + this->CaretBrush.HOffset;

        GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
        (
            Context,
            CaretSize,
            CaretTopLeft,
            this->CaretBrush.Color
        );
    }

    return;
}

void Jafg::WEditableTextBlock::UserInterfaceTick(const LViewport& InViewport)
{
    if (this->GetLocalEgo()->GetUserInput()->HasBufferedPlatformInput())
    {
        const LString BufferedInput { this->GetLocalEgo()->GetUserInput()->GetBufferedPlatformInputAsStr() };
        this->Content.AppendAt(this->CaretCursor, BufferedInput);

        const LString::SizeType Length = LString::Traits::GetStringLength<LString::SizeType>(BufferedInput.ToPtr());
        for (LString::SizeType I = 0; I < Length; ++I)
        {
            this->SafelyIncreaseCaretCursor();
        }

        this->CaretBlinker = 0.0f;
        this->OnContentChanged.InvokeIfBound(this->Content);
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

    if (this->Content.IsEmpty())
    {
        this->SetDesiredSize(LFontShaderContext::GetDesiredSize(this->Placeholder, this->TextScale) + this->GetDesiredSize());
    }
    else
    {
        this->SetDesiredSize(LFontShaderContext::GetDesiredSize(this->Content, this->TextScale) + this->GetDesiredSize());
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
    this->CaretBlinker = 0.0f;

    if (const LViewport* Viewport = this->GetViewport(); Viewport)
    {
        this->UserInterfaceTickDelegateHandle = Viewport->OnLateTick.AddMember(this, &WEditableTextBlock::UserInterfaceTick);
    }

    return;
}

void Jafg::WEditableTextBlock::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        if (const LViewport* Viewport = this->GetViewport(); Viewport)
        {
            Viewport->OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
        }
        else
        {
            LOG_ERROR(LogWidgets, "Viewport is invalid. Cannot remove handle.")
        }
    }

    return;
}

Jafg::LReply Jafg::WEditableTextBlock::OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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
            this->OnContentChanged.InvokeIfBound(this->Content);
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
        if (this->OnAllowContentCommit.IsBound())
        {
            if (this->OnAllowContentCommit.Invoke() == false)
            {
                return LReply::Unhandled();
            }
        }

        this->OnTextCommit(this->Content, ETextCommit::OnEnter);
        return LReply::Handled();
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

void Jafg::WEditableTextBlock::OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType)
{
    this->OnContentCommitted.InvokeIfBound(InText, InCommitType);
}

void Jafg::WEditableTextBlock::SetContent(const LString& InContent)
{
    this->Content = InContent;
    this->CaretCursor = Maths::Min(this->CaretCursor, this->Content.GetRuneCount());

    if (this->Content.GetByteSize() == 0)
    {
        this->Content = "";
        check( this->Content.GetByteSize() > 0 )
    }

    this->SetCaretCursorToEnd();

    this->OnContentChanged.InvokeIfBound(this->Content);

    return;
}

void Jafg::WEditableTextBlock::SetContent(LString&& InContent)
{
    this->Content = std::move(InContent);
    this->CaretCursor = Maths::Min(this->CaretCursor, this->Content.GetRuneCount());
    this->SetCaretCursorToEnd();

    this->OnContentChanged.InvokeIfBound(this->Content);

    return;
}

void Jafg::WEditableTextBlock::ClearContent()
{
    this->Content.Empty();
    this->CaretCursor = 0;

    this->OnContentChanged.InvokeIfBound(this->Content);

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
