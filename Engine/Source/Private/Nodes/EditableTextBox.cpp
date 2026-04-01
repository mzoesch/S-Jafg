// Copyright mzoesch. All rights reserved.

#include "Nodes/EditableTextBox.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Nodes/Viewport.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WEditableTextBox::Construct()
{
    Super::Construct();

    check( this->OnChanged.IsValid() == false )
    this->OnChanged.BindMember(this, &WEditableTextBox::OnSuperContentChanged);

    return;
}

void Jafg::WEditableTextBox::Draw(LNodeRenderInfo const& Info) const
{
    if (this->GetContent().empty())
    {
        WBox::Draw(Info);

        // GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText)->Draw
        // (
        //     Context,
        //     this->GetAnchoredSize_v2(),
        //     this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
        //     this->GetPadding(),
        //     this->GetDesiredSizeOfRawText(),
        //     this->GetTextHAlign(),
        //     this->GetTextVAlign(),
        //     this->PlaceholderColor,
        //     this->GetTextScale().InSpt(Context),
        //     this->PlaceholderContent
        // );
    }
    else
    {
        Super::Draw(Info);
    }

    if (this->CaretBlinker < this->CaretBrush.CaretBlinkerSpeed && this->IsFocusWidget())
    {
        const LVec2F AnchoredTopLeftFromMostOuter { this->GetAnchoredTopLeftFromMostOuter(Info.Surface.GetViewport()) };

        const LVec2F CaretSize{LVec2F{2.0, this->GetDesiredSize_v2().y} * this->CaretBrush.Size };

        LVec2F CaretTopLeft
        {
              AnchoredTopLeftFromMostOuter
            + LVec2F{0.0, (this->GetDesiredSize_v2().y - CaretSize.y) * 0.5}
            + LVec2F{this->GetBrush().Padding.Left, 0.0}
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

        // CaretTopLeft.x += this->GetDesiredWidthForString(CaretContent) + this->CaretBrush.HOffset;

        // GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
        // (
        //     Context,
        //     CaretSize,
        //     CaretTopLeft,
        //     this->CaretBrush.Tint
        // );
    }

    return;
}

void Jafg::WEditableTextBox::UpdateDesiredSize() const
{
    if (this->GetContent().empty())
    {
        // this->UpdateDesiredSizeForString(this->PlaceholderContent);
    }
    else
    {
        Super::UpdateDesiredSize();
    }

    return;
}

void Jafg::WEditableTextBox::UserInterfaceTick()
{
    bool bHandled{ false };

    if (this->GetViewport().GetSurface().HasBufferedPlatformInput())
    {
        LString BufferedInput{this->GetViewport().GetSurface().GetBufferedPlatformInputAsStr()};

        LString NewContent{this->GetContent()};
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
            for (auto Idx{ 0uz }; Idx < Length; ++Idx)
            {
                this->SafelyIncreaseCaretCursor();
            }

            ensureDiscard(this->InvokeOnChanged());
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

Jafg::LReply Jafg::WEditableTextBox::OnKeyDown(LNodeKeyDownData const& Data, LKeyEvent const& Event)
{
    if (Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::BackSpace)) // || PlatformDelete?
    {
        if (this->GetContent().empty() == false && this->CaretCursor > 0)
        {
            const LString::size_type RemoveNum { Jafg::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor - 1]) };
            this->GetMutableContent().erase(this->GetContent().begin() + (this->CaretCursor - 1), this->GetContent().begin() + (this->CaretCursor - 1 + RemoveNum));
            for (LString::size_type I { 0 }; I < RemoveNum; ++I)
            {
                this->SafelyReduceCaretCursor();
            }

            ensureDiscard(this->InvokeOnChanged());
        }

        return LReply::Handled();
    }

    if (Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::Left))
    {
        if (algo::is_valid_index(this->GetContent(), this->CaretCursor - 1))
        {
            const LString::size_type Size { Jafg::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor]) };
            for (LString::size_type I { 0 }; I < Size; ++I)
            {
                this->SafelyReduceCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;

        return LReply::Handled();
    }

    if (Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::Right))
    {
        if (algo::is_valid_index(this->GetContent(), this->CaretCursor))
        {
            const LString::size_type Size { Jafg::Utf8::GetCharacterLength(this->GetContent()[this->CaretCursor]) };
            for (LString::size_type I { 0 }; I < Size; ++I)
            {
                this->SafelyIncreaseCaretCursor();
            }
        }

        this->CaretBlinker = 0.0f;

        return LReply::Handled();
    }

    if (   Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::Enter)
        || Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::NumPadEnter))
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

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
    {
        if (Data.Surface.HasMouseLocation())
        {
            this->MoveCaretToMouseCursor(Data.Viewport);
        }

        return LReply::Handled();
    }

    if (Event.PhysicalKey == Data.Frontend.GetPhysicalKey(ENamedPhysicalKey::Escape))
    {
        if (this->GetViewport().GetFocusedWidget() == this)
        {
            this->OnTextCommit(this->GetContent(), ETextCommit::OnCleared);
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(Data, Event);
}

void Jafg::WEditableTextBox::OnTextCommit(const LString& InText, const ETextCommit InCommitType)
{
    (void)this->OnContentCommitted.InvokeIfBound(InText, InCommitType);
}

i32 Jafg::WEditableTextBox::SetCaretCursor(const i32 InCaretCursor)
{
    this->CaretCursor = maths::clamp(InCaretCursor, 0, static_cast<i32>(this->GetContent().size()));
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

void Jafg::WEditableTextBox::OnSuperContentChanged(const LString& InNewContent)
{
    this->CaretBlinker = 0.0f;
    this->CaretCursor = maths::min(this->CaretCursor, static_cast<i32>(this->GetContent().size()));
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
    const f32 BaseTopLeft
    {
        this->GetAnchoredTopLeftFromMostOuter(Viewport).x // + this->CaretBrush.HOffset
    };

    const f32 RelativeTopLeft { Viewport.GetSurface().GetMouseLocationValue().x - BaseTopLeft };

    if (RelativeTopLeft < 0.0)
    {
        this->SetCaretCursorToEnd();
    }
    else
    {
        // const i32 Rune{this->GoToWidth(this->GetContent(), RelativeTopLeft)};
        // this->SetCaretCursor(Rune);
    }

    return;
}

void Jafg::WEditableTextBox::SafelyReduceCaretCursor()
{
    this->CaretCursor = maths::max(this->CaretCursor - 1, 0);
}

void Jafg::WEditableTextBox::SafelyIncreaseCaretCursor()
{
    this->CaretCursor = maths::min(this->CaretCursor + 1, static_cast<i32>(this->GetContent().size()));
}
