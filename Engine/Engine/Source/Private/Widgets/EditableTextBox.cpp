// Copyright mzoesch. All rights reserved.

#include "Widgets/EditableTextBox.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Core/CoreNames.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicTextShader.h"
#include "Containers/SimpleUtf8EncodingUtilStuffKindaRandom.h"

LString Jafg::LexToString(const ETextCommit::Type InType)
{
    switch (InType)
    {
    case ETextCommit::OnEnter:   { return "OnEnter"; }
    case ETextCommit::OnCleared: { return "OnCleared"; }
    case ETextCommit::FocusLost: { return "FocusLost"; }
    default:                     { checkNoEntry() return { }; }
    }
}

Jafg::WEditableTextBox::WEditableTextBox(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    this->SetAnchor(EAnchor::Fill);
    this->SetPadding({5.0, 4.0});
    this->SetType(ERegionBrush::Box);
    this->SetTint(Lal::LColor::Black);
    this->SetTextColor(Lal::LColor::White);

    return;
}

void Jafg::WEditableTextBox::Construct()
{
    Super::Construct();

    check( this->OnChanged.IsValid() == false )
    this->OnChanged.BindMember(this, &WEditableTextBox::OnSuperContentChanged);

    return;
}

void Jafg::WEditableTextBox::Draw(LViewport& Context) const
{
    if (this->GetContent().empty())
    {
        WBox::Draw(Context);

        GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText)->Draw
        (
            Context,
            this->GetAnchoredSize_v2(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->GetPadding(),
            this->GetDesiredSizeOfRawText(),
            this->GetTextHAlign(),
            this->GetTextVAlign(),
            this->PlaceholderColor,
            this->GetTextScale().InSpt(Context),
            this->PlaceholderContent
        );
    }
    else
    {
        Super::Draw(Context);
    }

    if (this->CaretBlinker < this->CaretBrush.CaretBlinkerSpeed && this->IsFocusWidget())
    {
        const LVector2D AnchoredTopLeftFromMostOuter { this->GetAnchoredTopLeftFromMostOuter(Context) };

        const LVector2D CaretSize{LVector2D{2.0, this->GetDesiredSize_v2().Y} * this->CaretBrush.Size };

        LVector2D CaretTopLeft
        {
              AnchoredTopLeftFromMostOuter
            + LVector2D{0.0, (this->GetDesiredSize_v2().Y - CaretSize.Y) * 0.5}
            + LVector2D{this->GetBrush().Padding.Left, 0.0}
        }
        ;

        LString CaretContent;
        if (static_cast<i32>(this->GetContent().size()) != this->CaretCursor)
        {
            CaretContent = this->GetContent().substr(0, this->CaretCursor);
        }
        else
        {
            CaretContent = this->GetContent();
        }

        CaretTopLeft.X += this->GetDesiredWidthForString(CaretContent) + this->CaretBrush.HOffset;

        GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
        (
            Context,
            CaretSize,
            CaretTopLeft,
            this->CaretBrush.Tint
        );
    }

    return;
}

void Jafg::WEditableTextBox::UpdateDesiredSize() const
{
    if (this->GetContent().empty())
    {
        this->UpdateDesiredSizeForString(this->PlaceholderContent);
    }
    else
    {
        Super::UpdateDesiredSize();
    }

    return;
}

void Jafg::WEditableTextBox::UserInterfaceTick(const LViewport& InViewport)
{
    bool bHandled{ false };

    if (InViewport.GetSurface().HasBufferedPlatformInput())
    {
        const LString BufferedInput{ InViewport.GetSurface().GetBufferedPlatformInputAsStr() };

        LString NewContent { this->GetContent() };
        NewContent.insert(this->CaretCursor, BufferedInput);

        if (this->ContentPredicate.IsValid() && this->ContentPredicate.Invoke(NewContent) == false)
        {
            LOG_VERBOSE(LogWidgets, "Content predicate failed for [{}]. Discarding content change request.", NewContent)
        }
        else
        {
            bHandled = true;

            this->GetMutableContent() = std::move(NewContent);

            const LString::size_type Length { BufferedInput.size() };
            for (LString::size_type Idx { 0 }; Idx < Length; ++Idx)
            {
                this->SafelyIncreaseCaretCursor();
            }

            ensureDiscard(this->OnChanged.InvokeIfBound(this->GetContent()));
        }
    }

    if (bHandled == false)
    {
        this->CaretBlinker += Application::GetDeltaTimeAsFloat();
        if (this->CaretBlinker > this->CaretBrush.CaretBlinkerSpeed * 2.0f)
        {
            this->CaretBlinker = 0.0f;
        }
    }

    return;
}

Jafg::LCursorReply Jafg::WEditableTextBox::OnCursorEnter()
{
    return { EMouseCursor::Beam };
}

Jafg::LCursorReply Jafg::WEditableTextBox::OnCursorLeave()
{
    return { EMouseCursor::Default };
}

void Jafg::WEditableTextBox::OnFocusReceived()
{
    Super::OnFocusReceived();
    this->CaretBlinker = 0.0f;

    this->UserInterfaceTickDelegateHandle = this->GetViewport().OnLateTick.Emplace(this, &WEditableTextBox::UserInterfaceTick);

    if (this->GetViewport().GetSurface().HasMouseLocation())
    {
        this->MoveCaretToMouseCursor(this->GetViewport());
    }
    else
    {
        this->SetCaretCursorToEnd();
    }

    return;
}

void Jafg::WEditableTextBox::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->GetViewport().OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
    }

    this->OnTextCommit(this->GetContent(), ETextCommit::FocusLost);

    return;
}

Jafg::LReply Jafg::WEditableTextBox::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::BackSpace || InKeyEvent.GetKey() == EKeys::PlatformDelete)
    {
        if (this->GetContent().empty() == false && this->CaretCursor > 0)
        {
            const LString::size_type RemoveNum { Lal::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor - 1]) };
            this->GetMutableContent().erase(this->GetContent().begin() + (this->CaretCursor - 1), this->GetContent().begin() + (this->CaretCursor - 1 + RemoveNum));
            for (LString::size_type I { 0 }; I < RemoveNum; ++I)
            {
                this->SafelyReduceCaretCursor();
            }

            ensureDiscard(this->OnChanged.InvokeIfBound(this->GetContent()));
        }

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Left)
    {
        if (algo::is_valid_index(this->GetContent(), this->CaretCursor - 1))
        {
            const LString::size_type Size { Lal::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor]) };
            for (LString::size_type I { 0 }; I < Size; ++I)
            {
                this->SafelyReduceCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Right)
    {
        if (algo::is_valid_index(this->GetContent(), this->CaretCursor))
        {
            const LString::size_type Size { Lal::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor]) };
            for (LString::size_type I { 0 }; I < Size; ++I)
            {
                this->SafelyIncreaseCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::NumPadEnter)
    {
        if (this->OnAllowContentCommit.IsValid())
        {
            if (this->OnAllowContentCommit.Invoke() == false)
            {
                return LReply::Unhandled();
            }
        }

        this->OnTextCommit(this->GetContent(), ETextCommit::OnEnter);

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (InViewport.GetSurface().HasMouseLocation())
        {
            this->MoveCaretToMouseCursor(InViewport);
        }

        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        if (this->GetViewport().GetFocusedWidget() == this)
        {
            this->OnTextCommit(this->GetContent(), ETextCommit::OnCleared);
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

void Jafg::WEditableTextBox::OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType)
{
    (void)this->OnContentCommitted.InvokeIfBound(InText, InCommitType);
}

i32 Jafg::WEditableTextBox::SetCaretCursor(const i32 InCaretCursor)
{
    this->CaretCursor = Maths::Clamp(InCaretCursor, 0, static_cast<i32>(this->GetContent().size()));
    return this->CaretCursor;
}

i32 Jafg::WEditableTextBox::SetCaretCursorToBegin()
{
    this->CaretCursor = 0;
    return this->CaretCursor;
}

i32 Jafg::WEditableTextBox::SetCaretCursorToEnd()
{
    this->CaretCursor = this->GetContent().size();
    return this->CaretCursor;
}

bool Jafg::WEditableTextBox::IsContentFloatingPoint(const LString& InContent) noexcept
{
    for (auto& Rune : InContent)
    {
        if (Rune != '.' && (Rune < '0' || Rune > '9'))
        {
            return false;
        }

        continue;
    }

    return true;
}

void Jafg::WEditableTextBox::OnSuperContentChanged(const LString& InNewContent)
{
    this->CaretBlinker = 0.0f;
    this->CaretCursor = Maths::Min(this->CaretCursor, static_cast<i32>(this->GetContent().size()));
    (void)this->OnContentChanged.InvokeIfBound(InNewContent);

    return;
}

void Jafg::WEditableTextBox::MoveCaretToMouseCursor(LViewport const& Viewport)
{
    check( Viewport.GetSurface().HasMouseLocation() )

    /*
     * This is a workaround and bugprone. We are using the widget location data from the last frame.
     * Meaning that things could have changed in the meantime... It is maybe safer to update the caret in the
     * draw method of this widget instead? But who gives a shit right now? The user would need to click / tap keys
     * in the same frame. I do not think that my users will have the brainpower to actually operate computers fast
     * and right.
     */
    const f64 BaseTopLeft
    {
        this->GetAnchoredTopLeftFromMostOuter(Viewport).X // + this->CaretBrush.HOffset
    };

    const f64 RelativeTopLeft { Viewport.GetSurface().GetMouseLocationValue().X - BaseTopLeft };

    if (RelativeTopLeft < 0.0)
    {
        this->SetCaretCursorToEnd();
    }
    else
    {
        const i32 Rune{this->GoToWidth(this->GetContent(), RelativeTopLeft)};
        this->SetCaretCursor(Rune);
    }

    return;
}

void Jafg::WEditableTextBox::SafelyReduceCaretCursor()
{
    this->CaretCursor = Maths::Max(this->CaretCursor - 1, 0);
}

void Jafg::WEditableTextBox::SafelyIncreaseCaretCursor()
{
    this->CaretCursor = Maths::Min(this->CaretCursor + 1, static_cast<i32>(this->GetContent().size()));
}
