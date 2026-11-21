// Copyright mzoesch. All rights reserved.

#include "Nodes/EditableTextButton.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/Surface.h"
#include "Framework/FontSubsystem.h"
#include "User/UserPreferences.h"

namespace Jafg
{
namespace
{

//# The hint for #LFrontend::Glfw_GetPhysicalKeyLocalizedRepr to resolve a key by its scancode only (GLFW_KEY_UNKNOWN).
inline constexpr i32 ResolveByScancode{ -1 };

NODISCARD bool IsKey(LNodeKeyEventInfo const& Info, LKeyEvent const& Event, ELogicalKey Key) noexcept
{
    return Event.Is<ERawInputStateBits::Press|ERawInputStateBits::Repeat>(Info.Frontend.GetPhysicalKey(Key));
}

//#
//# @return The latin letter (lower case) that the key of the event produces in the keyboard layout of the user. E.g.,
//#         Ctrl+Z is on a different physical key for QWERTY and QWERTZ layouts. Empty, if the key does not produce a
//#         latin letter (e.g., for Cyrillic layouts).
//#
NODISCARD std::optional<char> GetLayoutLetter(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Event.PhysicalKey.Scancode == INDEX_NONE)
    {
        return {};
    }
    /* Pressing a modifier while holding Ctrl lands here as well. Do not bother the platform with that. */
    for (ELogicalKey Modifier: {
        ELogicalKey::LeftShift, ELogicalKey::RightShift, ELogicalKey::LeftControl, ELogicalKey::RightControl,
        ELogicalKey::LeftAlt, ELogicalKey::RightAlt, ELogicalKey::LeftCommand, ELogicalKey::RightCommand,
        })
    {
        if (Event.PhysicalKey == Info.Frontend.GetPhysicalKey(Modifier))
        {
            return {};
        }
    }
    if (auto Name{Info.Frontend.Glfw_GetPhysicalKeyLocalizedRepr(Event.PhysicalKey, ResolveByScancode)}; Name && Name->size() == 1uz)
    {
        char Letter{(*Name)[0]};
        if (Letter >= 'A' && Letter <= 'Z')
        {
            Letter = static_cast<char>(Letter - 'A' + 'a');
        }
        if (Letter >= 'a' && Letter <= 'z')
        {
            return Letter;
        }
    }
    return {};
}

//# If the layout does not produce a latin letter, we fall back to the key location of the US layout.
NODISCARD bool IsShortcut(LNodeKeyEventInfo const& Info, LKeyEvent const& Event, std::optional<char> LayoutLetter, ELogicalKey Key, char Letter)
{
    if (LayoutLetter.has_value())
    {
        return *LayoutLetter == Letter;
    }
    return Event.PhysicalKey == Info.Frontend.GetPhysicalKey(Key);
}

} /* ~Namespace <Anonymous> */
} /* ~Namespace Jafg */

void Jafg::WEditableTextButton::Draw(LNodeRenderInfo const& Info) const
{
    /*
     * We intentionally skip WTextBox::Draw and only draw the box. The text of an editable text button scrolls, and
     * it must be drawn on top of the selection highlight.
     */
    WBox::Draw(Info);

    this->EnsureTextLayout();

    auto const& RenderData{this->GetTextRenderData()};
    const bool bFocused{this->IsFocusWidget()};
    const bool bHasContent{!this->GetContent().empty()};

    const LVec2F TopLeft{this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)};
    //# Where the pencil of the text starts (alignment is already applied).
    const LVec2F Origin{TopLeft + this->GetRelativeTextTopLeft()};
    //# The space that is available to the text.
    const LVec2F Extent{this->GetAnchoredSize_v2()
        - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
        - this->TextDrawOffset
        - this->TextPlayroomReduction
        };
    const f32 ContentLeft{TopLeft.x + this->Brush.Padding.GetLeftOffset().InStaticPoints(this->GetViewport()) + this->TextDrawOffset.x};

    const LVec2F CaretSize{maths::round(LVec2F{2.0f, RenderData.DesiredSize.y} * this->CaretBrush.Size)};
    const f32 CaretTop{Origin.y + (RenderData.DesiredSize.y - CaretSize.y) * 0.5f};

    this->LastDrawnRect = LRect2F{.offset = TopLeft, .extent = this->GetAnchoredSize_v2()};
    this->LastTextOriginX = Origin.x;

    /* Scroll the text such that the caret is always visible. */
    if (bFocused && bHasContent && this->TextCutoff != ETextCutoff::NoBounds)
    {
        const f32 TextWidth{RenderData.Collection.PencilEnd.x};
        const f32 CaretWidth{CaretSize.x + maths::max(this->CaretBrush.HOffset, 0.0f)};
        const f32 MaxScroll{TextWidth > Extent.x ? TextWidth + CaretWidth - Extent.x : 0.0f};
        const f32 CaretX{this->GetCaretPencilLocation(this->CaretCursor)};
        if (CaretX - this->HScroll < 0.0f)
        {
            this->HScroll = CaretX;
        }
        else if (CaretX + CaretWidth - this->HScroll > Extent.x)
        {
            this->HScroll = CaretX + CaretWidth - Extent.x;
        }
        this->HScroll = maths::clamp(this->HScroll, 0.0f, MaxScroll);
    }
    else
    {
        this->HScroll = 0.0f;
    }

    if (bFocused && bHasContent && this->HasSelection())
    {
        auto [Begin, End]{this->GetSelectionRange()};
        f32 Left{maths::round(Origin.x + this->GetCaretPencilLocation(Begin) - this->HScroll)};
        f32 Right{maths::round(Origin.x + this->GetCaretPencilLocation(End) - this->HScroll)};
        if (this->TextCutoff != ETextCutoff::NoBounds)
        {
            Left = maths::max(Left, ContentLeft);
            Right = maths::min(Right, ContentLeft + Extent.x);
        }
        if (Right > Left)
        {
            Info.AddInstance({
                .Rect = {maths::round(LVec2F{Left, CaretTop}), LVec2F{Right - Left, CaretSize.y}},
                .Tint = this->CaretBrush.SelectionTint,
                });
        }
    }

    if (!this->TextBrush.bSkipBrushDraw)
    {
        if (bHasContent)
        {
            ETextCutoff Cutoff{this->TextCutoff};
            if (bFocused && Cutoff == ETextCutoff::TribbleDot)
            {
                /* While editing, the user has to see the actual content. */
                Cutoff = ETextCutoff::Cutoff;
            }
            RenderData.Render(Info, {.offset = Origin, .extent = Extent}, {
                .Cutoff = Cutoff,
                .Tint = this->TextBrush.Tint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                .HScroll = this->HScroll,
                });
        }
        else if (!this->PlaceholderContent.empty())
        {
            RenderData.Render(Info, {.offset = Origin, .extent = Extent}, {
                .Cutoff = this->TextCutoff,
                .Tint = this->PlaceholderTint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                });
        }
    }

    if (bFocused && this->CaretBlinker < this->CaretBrush.CaretBlinkerSpeed)
    {
        f32 CaretLeft{Origin.x - this->HScroll + this->CaretBrush.HOffset};
        if (bHasContent)
        {
            CaretLeft += this->GetCaretPencilLocation(this->CaretCursor);
        }
        Info.AddInstance({.Rect = {maths::round(LVec2F{CaretLeft, CaretTop}), CaretSize}, .Tint = this->CaretBrush.Tint});
    }

    return;
}

void Jafg::WEditableTextButton::UpdateDesiredSize() const
{
    /* TODO: Not optimal. Can we cache this? */
    if (this->GetContent().empty() && !this->PlaceholderContent.empty())
    {
        this->GetTextRenderData().Update(this->GetViewport()
            , *this->GetFrontend().GetSubsystemChecked<JFontSubsystem>()
            , this->TextBrush
            , this->PlaceholderContent
            , this->TextCutoff
            );
    }

    Super::UpdateDesiredSize();

    return;
}

bool Jafg::WEditableTextButton::UserInterfaceTick()
{
    auto& Surface{this->GetViewport().GetSurface()};

    if (Surface.HasBufferedPlatformInput())
    {
        LString BufferedInput{Surface.ConsumeBufferedPlatformInput()};
        check(!BufferedInput.empty())
        if (this->bEnabled)
        {
            auto [Begin, End]{this->GetSelectionRange()};
            (void)this->ReplaceRange(Begin, End, BufferedInput, EEditKind::Typing);
            this->CaretBlinker = 0.0f;
        }
    }

    if (this->bMouseSelecting)
    {
        if (!Surface.HasPlatformKeyState(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton), ERawInputStateBits::Press|ERawInputStateBits::Hold))
        {
            this->bMouseSelecting = false;
            this->PendingMouseSelectionAnchor.reset();
        }
        /* If the mouse cursor is hidden, someone else uses the drag (e.g., value scrubbing). */
        else if (Surface.HasMouseLocationForOrtho())
        {
            const std::size_t Idx{this->HitTestCaret(Surface.GetMouseLocationValue())};
            if (this->PendingMouseSelectionAnchor.has_value())
            {
                if (Idx != *this->PendingMouseSelectionAnchor)
                {
                    this->SetSelection(*this->PendingMouseSelectionAnchor, Idx);
                    this->PendingMouseSelectionAnchor.reset();
                }
            }
            else if (Idx != this->CaretCursor)
            {
                this->SetSelection(this->SelectionAnchor, Idx);
            }
        }
    }

    this->CaretBlinker += static_cast<f32>(GEngine->DeltaTime);
    if (this->CaretBlinker > this->CaretBrush.CaretBlinkerSpeed * 2.0f)
    {
        this->CaretBlinker = 0.0f;
    }

    return {};
}

Jafg::LNodeReply Jafg::WEditableTextButton::OnCursorEnter()
{
    if (auto Reply{Super::OnCursorEnter()}; Reply.IsHandled())
    {
        if (Reply.GetCursor() != ECursor::None)
        {
            return Reply;
        }
        this->bBeamed = true;
        return {ECursor::Beam};
    }
    return {};
}

void Jafg::WEditableTextButton::OnCursorLeave()
{
    if (this->bBeamed)
    {
        this->bBeamed = false;
        this->GetViewport().GetSurface()._SetMouseCursor(ECursor::Default);
    }

    Super::OnCursorLeave();
    return;
}

void Jafg::WEditableTextButton::OnFocusReceived()
{
    Super::OnFocusReceived();
    this->CaretBlinker = 0.0f;
    this->FocusReceivedFrame = GEngine->FrameCount;
    this->bMouseSelecting = false;
    this->PendingMouseSelectionAnchor.reset();
    this->ClickCount = 0;

    check(!this->UserInterfaceTickDelegateHandle)
    this->UserInterfaceTickDelegateHandle = this->GetViewport().OnLateTick.Emplace(this, &WEditableTextButton::UserInterfaceTick);

    /* If the focus was received by a mouse click, the caret is moved to the click location in #OnMousePressed. */
    if (this->bSelectAllOnFocus)
    {
        this->SelectAll();
    }
    else
    {
        this->SetCaretCursorToEnd();
    }

    return;
}

void Jafg::WEditableTextButton::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->GetViewport().OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
    }

    this->bMouseSelecting = false;
    this->PendingMouseSelectionAnchor.reset();
    this->ClearSelection();
    this->HScroll = 0.0f;
    this->UndoHistory.clear();
    this->RedoHistory.clear();

    this->OnTextCommit(ETextCommit::FocusLost);

    return;
}

Jafg::LNodeReply Jafg::WEditableTextButton::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        /* The button base handles mouse presses (brushes and the bound delegate), but the caret must move regardless. */
        auto Reply{Super::OnKeyEventFocused(Info, Event)};
        if (this->bEnabled && Info.CursorLocation && this->LastDrawnRect && maths::aabb_point(*this->LastDrawnRect, *Info.CursorLocation))
        {
            this->OnMousePressed(Info, Event);
            if (Reply.IsHandled())
            {
                return Reply;
            }
            return LNodeReply::Handled();
        }
        return Reply;
    }

    if (auto Reply{Super::OnKeyEventFocused(Info, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (!this->bEnabled || !Event.Is<ERawInputStateBits::Press|ERawInputStateBits::Repeat>())
    {
        return LNodeReply::Unhandled();
    }

    return this->HandleEditingKey(Info, Event);
}

Jafg::LNodeReply Jafg::WEditableTextButton::HandleEditingKey(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    LStringView Content{this->GetContent()};
    const bool bShift{!!(Event.Mods & EModBits::Shift)};
    const bool bControl{!!(Event.Mods & EModBits::Control)};
    const bool bAlt{!!(Event.Mods & EModBits::Alt)};
    /* AltGr is reported as Ctrl+Alt on some platforms. That must never be mistaken as a shortcut. */
    const bool bShortcut{bControl && !bAlt};

    auto Handled{[this]
    {
        this->CaretBlinker = 0.0f;
        return LNodeReply::Handled();
    }};

    /* Navigation. */
    if (IsKey(Info, Event, ELogicalKey::Left))
    {
        if (this->HasSelection() && !bShift && !bShortcut)
        {
            this->SetCaretCursor(this->GetSelectionRange().first);
        }
        else
        {
            this->MoveCaret(bShortcut
                ? TextEditing::PrevWordBoundary(Content, this->CaretCursor)
                : TextEditing::PrevCodePoint(Content, this->CaretCursor)
                , bShift);
        }
        return Handled();
    }
    if (IsKey(Info, Event, ELogicalKey::Right))
    {
        if (this->HasSelection() && !bShift && !bShortcut)
        {
            this->SetCaretCursor(this->GetSelectionRange().second);
        }
        else
        {
            this->MoveCaret(bShortcut
                ? TextEditing::NextWordBoundary(Content, this->CaretCursor)
                : TextEditing::NextCodePoint(Content, this->CaretCursor)
                , bShift);
        }
        return Handled();
    }
    if (IsKey(Info, Event, ELogicalKey::Home))
    {
        this->MoveCaret(0uz, bShift);
        return Handled();
    }
    if (IsKey(Info, Event, ELogicalKey::End))
    {
        this->MoveCaret(Content.size(), bShift);
        return Handled();
    }

    /* Deletion. */
    if (IsKey(Info, Event, ELogicalKey::BackSpace))
    {
        if (this->HasSelection())
        {
            (void)this->DeleteSelection();
        }
        else if (this->CaretCursor > 0uz)
        {
            const std::size_t Begin{bShortcut
                ? TextEditing::PrevWordBoundary(Content, this->CaretCursor)
                : TextEditing::PrevCodePoint(Content, this->CaretCursor)
                };
            (void)this->ReplaceRange(Begin, this->CaretCursor, {}, EEditKind::Deletion);
        }
        return Handled();
    }
    if (IsKey(Info, Event, ELogicalKey::Delete))
    {
        if (bShift && !bControl && this->HasSelection())
        {
            (void)this->CutSelection();
        }
        else if (this->HasSelection())
        {
            (void)this->DeleteSelection();
        }
        else if (this->CaretCursor < Content.size())
        {
            const std::size_t End{bShortcut
                ? TextEditing::NextWordBoundary(Content, this->CaretCursor)
                : TextEditing::NextCodePoint(Content, this->CaretCursor)
                };
            (void)this->ReplaceRange(this->CaretCursor, End, {}, EEditKind::Deletion);
        }
        return Handled();
    }

    /* Legacy clipboard shortcuts. */
    if (IsKey(Info, Event, ELogicalKey::Insert))
    {
        if (bControl && !bShift)
        {
            this->CopySelection();
            return Handled();
        }
        if (bShift && !bControl)
        {
            (void)this->PasteFromClipboard();
            return Handled();
        }
        return LNodeReply::Unhandled();
    }

    if (bShortcut)
    {
        const std::optional<char> Letter{GetLayoutLetter(Info, Event)};
        if (IsShortcut(Info, Event, Letter, ELogicalKey::A, 'a'))
        {
            this->SelectAll();
            return Handled();
        }
        if (IsShortcut(Info, Event, Letter, ELogicalKey::C, 'c'))
        {
            this->CopySelection();
            return Handled();
        }
        if (IsShortcut(Info, Event, Letter, ELogicalKey::X, 'x'))
        {
            (void)this->CutSelection();
            return Handled();
        }
        if (IsShortcut(Info, Event, Letter, ELogicalKey::V, 'v'))
        {
            (void)this->PasteFromClipboard();
            return Handled();
        }
        if (IsShortcut(Info, Event, Letter, ELogicalKey::Z, 'z'))
        {
            (void)(bShift ? this->Redo() : this->Undo());
            return Handled();
        }
        if (IsShortcut(Info, Event, Letter, ELogicalKey::Y, 'y'))
        {
            (void)this->Redo();
            return Handled();
        }
    }

    if (Event.Is<ERawInputStateBits::Press>(
          Info.Frontend.GetPhysicalKey(ELogicalKey::Enter)
        , Info.Frontend.GetPhysicalKey(ELogicalKey::NumPadEnter)))
    {
        if (this->OnAllowContentCommit.IsValid())
        {
            if (!this->OnAllowContentCommit.Invoke(*this))
            {
                return LNodeReply::Unhandled();
            }
        }

        this->OnTextCommit(ETextCommit::OnEnter);
        return LNodeReply::Unhandled();
    }

    if (Event.Is<ERawInputStateBits::Press>(Info.Frontend.GetPhysicalKey(ELogicalKey::Escape)))
    {
        if (this->GetViewport().GetFocusedWidget() == this)
        {
            this->OnTextCommit(ETextCommit::OnCleared);
            return LNodeReply{TClassStorage<WNode>{}};
        }
    }

    return LNodeReply::Unhandled();
}

void Jafg::WEditableTextButton::OnMousePressed(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Info.CursorLocation)

    const LVec2F Location{*Info.CursorLocation};
    const f64 Now{GEngine->FrameStartElapsedTime};
    if (this->ClickCount > 0
        && Now - this->LastClickTime <= WEditableTextButton::MultiClickTime
        && maths::magnitude(Location - this->LastClickLocation) <= WEditableTextButton::MultiClickDistance)
    {
        this->ClickCount = static_cast<u8>(maths::min(this->ClickCount + 1, 3));
    }
    else
    {
        this->ClickCount = 1;
    }
    this->LastClickTime = Now;
    this->LastClickLocation = Location;

    const std::size_t Idx{this->HitTestCaret(Location)};
    const bool bFocusingClick{this->FocusReceivedFrame == GEngine->FrameCount};

    this->bMouseSelecting = false;
    this->PendingMouseSelectionAnchor.reset();

    if (bFocusingClick && this->bSelectAllOnFocus)
    {
        /* Keep everything selected, unless the user starts to drag a selection. */
        this->bMouseSelecting = true;
        this->PendingMouseSelectionAnchor = Idx;
        return;
    }

    switch (this->ClickCount)
    {
    case 1:
    {
        this->MoveCaret(Idx, !bFocusingClick && !!(Event.Mods & EModBits::Shift));
        this->bMouseSelecting = true;
        break;
    }
    case 2:
    {
        auto [Begin, End]{TextEditing::WordAt(this->GetContent(), Idx)};
        this->SetSelection(Begin, End);
        break;
    }
    default:
    {
        this->SelectAll();
        break;
    }
    }

    return;
}

void Jafg::WEditableTextButton::OnChangedImpl() noexcept
{
    this->CaretBlinker = 0.0f;

    if (!this->bApplyingUserEdit)
    {
        /* The content was changed from the outside. Previous states do not make sense anymore. */
        auto [Begin, End]{this->GetSelectionRange()};
        const bool bWasEverythingSelected{this->LastContentSize > 0uz && Begin == 0uz && End >= this->LastContentSize};

        this->UndoHistory.clear();
        this->RedoHistory.clear();
        this->LastEditKind = EEditKind::Other;

        if (bWasEverythingSelected)
        {
            this->SelectionAnchor = 0uz;
            this->CaretCursor = this->GetContent().size();
        }
        else
        {
            this->SelectionAnchor = TextEditing::SnapToCodePoint(this->GetContent(), this->SelectionAnchor);
            this->CaretCursor = TextEditing::SnapToCodePoint(this->GetContent(), this->CaretCursor);
        }
        if (this->PendingMouseSelectionAnchor.has_value())
        {
            this->PendingMouseSelectionAnchor = TextEditing::SnapToCodePoint(this->GetContent(), *this->PendingMouseSelectionAnchor);
        }
    }

    this->LastContentSize = this->GetContent().size();

    Super::OnChangedImpl();

    return;
}

void Jafg::WEditableTextButton::SetSelection(std::size_t Anchor, std::size_t Caret) noexcept
{
    this->SelectionAnchor = TextEditing::SnapToCodePoint(this->GetContent(), Anchor);
    this->CaretCursor = TextEditing::SnapToCodePoint(this->GetContent(), Caret);
    this->CaretBlinker = 0.0f;
    /* Moving the caret ends the current undo step. */
    this->LastEditKind = EEditKind::Other;
    return;
}

std::size_t Jafg::WEditableTextButton::MoveCaretTo(LVec2F Location)
{
    return this->SetCaretCursor(this->HitTestCaret(Location));
}

void Jafg::WEditableTextButton::MoveCaret(std::size_t Idx, bool bExtendSelection) noexcept
{
    this->SetSelection(bExtendSelection ? this->SelectionAnchor : Idx, Idx);
    return;
}

bool Jafg::WEditableTextButton::InsertText(LStringView Text)
{
    auto [Begin, End]{this->GetSelectionRange()};
    return this->ReplaceRange(Begin, End, Text, EEditKind::Other);
}

bool Jafg::WEditableTextButton::DeleteSelection()
{
    if (!this->HasSelection())
    {
        return false;
    }
    auto [Begin, End]{this->GetSelectionRange()};
    return this->ReplaceRange(Begin, End, {}, EEditKind::Other);
}

void Jafg::WEditableTextButton::CopySelection() const
{
    if (this->HasSelection())
    {
        this->GetViewport().GetSurface().SetClipboardText(LString{this->GetSelectedContent()});
    }
    return;
}

bool Jafg::WEditableTextButton::CutSelection()
{
    if (!this->HasSelection())
    {
        return false;
    }
    this->CopySelection();
    return this->DeleteSelection();
}

bool Jafg::WEditableTextButton::PasteFromClipboard()
{
    LString Text{TextEditing::SanitizeSingleLine(this->GetViewport().GetSurface().GetClipboardText())};
    if (Text.empty())
    {
        return false;
    }
    return this->InsertText(Text);
}

bool Jafg::WEditableTextButton::Undo()
{
    if (this->UndoHistory.empty())
    {
        return false;
    }

    this->RedoHistory.push_back({.Content = this->GetContent(), .CaretCursor = this->CaretCursor, .SelectionAnchor = this->SelectionAnchor});
    LEditSnapshot Snapshot{std::move(this->UndoHistory.back())};
    this->UndoHistory.pop_back();
    this->RestoreSnapshot(std::move(Snapshot));

    return true;
}

bool Jafg::WEditableTextButton::Redo()
{
    if (this->RedoHistory.empty())
    {
        return false;
    }

    this->UndoHistory.push_back({.Content = this->GetContent(), .CaretCursor = this->CaretCursor, .SelectionAnchor = this->SelectionAnchor});
    LEditSnapshot Snapshot{std::move(this->RedoHistory.back())};
    this->RedoHistory.pop_back();
    this->RestoreSnapshot(std::move(Snapshot));

    return true;
}

bool Jafg::WEditableTextButton::ApplyUserEdit(LString NewContent, std::size_t NewCaret, EEditKind Kind)
{
    if (NewContent == this->GetContent())
    {
        this->SetCaretCursor(NewCaret);
        return false;
    }

    if (this->ContentPredicate.IsValid() && !this->ContentPredicate.Invoke(NewContent))
    {
        LOG_VERBOSE(LogWidgets, "Content predicate failed for [{}]. Discarding content change request.", NewContent)
        return false;
    }

    /* Consecutive typing (or deleting) is merged into one undo step. */
    if (Kind == EEditKind::Other || Kind != this->LastEditKind || this->UndoHistory.empty())
    {
        this->UndoHistory.push_back({.Content = this->GetContent(), .CaretCursor = this->CaretCursor, .SelectionAnchor = this->SelectionAnchor});
        if (this->UndoHistory.size() > WEditableTextButton::MaxUndoHistory)
        {
            this->UndoHistory.erase(this->UndoHistory.begin());
        }
    }
    this->RedoHistory.clear();

    {
        this->bApplyingUserEdit = true;
        algo::raii_leave _{[this]{ this->bApplyingUserEdit = false; }};
        auto Writer{this->GetMutableTextContent()};
        Writer.first = std::move(NewContent);
    }

    this->SetCaretCursor(NewCaret);
    this->LastEditKind = Kind;
    this->OnContentChanged.InvokeIfBound(*this, this->GetContent());

    return true;
}

bool Jafg::WEditableTextButton::ReplaceRange(std::size_t Begin, std::size_t End, LStringView With, EEditKind Kind)
{
    Begin = TextEditing::SnapToCodePoint(this->GetContent(), Begin);
    End = TextEditing::SnapToCodePoint(this->GetContent(), End);
    if (Begin > End)
    {
        std::swap(Begin, End);
    }

    LString NewContent{this->GetContent()};
    NewContent.replace(Begin, End - Begin, With);
    return this->ApplyUserEdit(std::move(NewContent), Begin + With.size(), Kind);
}

void Jafg::WEditableTextButton::RestoreSnapshot(LEditSnapshot Snapshot)
{
    {
        this->bApplyingUserEdit = true;
        algo::raii_leave _{[this]{ this->bApplyingUserEdit = false; }};
        auto Writer{this->GetMutableTextContent()};
        Writer.first = std::move(Snapshot.Content);
    }

    this->SetSelection(Snapshot.SelectionAnchor, Snapshot.CaretCursor);
    this->OnContentChanged.InvokeIfBound(*this, this->GetContent());

    return;
}

void Jafg::WEditableTextButton::EnsureTextLayout() const
{
    if (this->IsTextRenderDataDirty<&WTextBox::TextBrush>())
    {
        this->GetTextRenderData().Update(this->GetViewport()
            , *this->GetFrontend().GetSubsystemChecked<JFontSubsystem>()
            , this->TextBrush
            , this->GetContent().empty() ? LStringView{this->PlaceholderContent} : LStringView{this->GetContent()}
            , this->TextCutoff
            );
    }
    return;
}

std::size_t Jafg::WEditableTextButton::HitTestCaret(LVec2F Location) const
{
    /*
     * We use the location of the text from the last frame as the node might be translated in ways that we cannot know
     * here (e.g., inside scroll regions). Things could have changed in the meantime, but the user would need to click
     * and change the layout in the same frame.
     */
    if (this->GetContent().empty())
    {
        return 0uz;
    }

    this->EnsureTextLayout();

    const f32 OriginX{this->LastTextOriginX.has_value()
        ? *this->LastTextOriginX
        : this->GetAnchoredTopLeftFromMostOuter().x + this->GetRelativeTextTopLeft().x
        };
    return this->GetTextRenderData().Collection.caret_index(this->GetContent(), Location.x - OriginX + this->HScroll);
}

f32 Jafg::WEditableTextButton::GetCaretPencilLocation(std::size_t Idx) const
{
    if (this->GetContent().empty())
    {
        return 0.0f;
    }
    return this->GetTextRenderData().Collection.caret_location(this->GetContent(), Idx);
}

void Jafg::WEditableTextButtonIconizedLeft::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WEditableTextButtonIconizedLeft::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextDrawOffset = Size;
    return;
}
