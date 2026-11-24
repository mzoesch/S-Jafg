// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextButton.h"
#include "EditableTextButton.generated.h"

namespace Jafg
{

class WEditableTextButton;
class WEditableTextButtonIconizedLeft;
struct LFactoryEditableTextButton;
typedef TFactoryIconized<WEditableTextButtonIconizedLeft> LFactoryEditableTextButtonIconizedLeft;

SERDE_DECLARE_TRIVIAL_ENUM(ETextCommit, u8,
    //# Text was committed via an enter key (or similar) press. This does not mean a loss of focus.
    OnEnter,
    //# Focus was lost due to a press of the escape key.
    OnCleared,
    //# Focus was lost due to some other reason.
    FocusLost
    )

//# Platform independent helpers for editing UTF-8 text. All indices are byte indices.
namespace TextEditing
{

enum struct ECharClass: u8 { Space, Punctuation, Word };
//# Non-ASCII bytes are always treated as word characters.
NODISCARD inline constexpr ECharClass Classify(char C) noexcept
{
    const auto U{static_cast<u8>(C)};
    if (U == ' ' || U == '\t' || U == '\n' || U == '\r' || U == '\v' || U == '\f')
    {
        return ECharClass::Space;
    }
    if (U >= 0x80 || U == '_' || (U >= '0' && U <= '9') || (U >= 'a' && U <= 'z') || (U >= 'A' && U <= 'Z'))
    {
        return ECharClass::Word;
    }
    return ECharClass::Punctuation;
}

NODISCARD inline constexpr bool IsContinuationByte(char C) noexcept { return (static_cast<u8>(C) & 0xC0) == 0x80; }

//# @return The closest code point boundary that is at or before the given index.
NODISCARD inline constexpr std::size_t SnapToCodePoint(LStringView Text, std::size_t Idx) noexcept
{
    Idx = maths::min(Idx, Text.size());
    while (Idx > 0uz && Idx < Text.size() && IsContinuationByte(Text[Idx]))
    {
        --Idx;
    }
    return Idx;
}

//# @return The beginning of the code point before the given index.
NODISCARD inline constexpr std::size_t PrevCodePoint(LStringView Text, std::size_t Idx) noexcept
{
    Idx = maths::min(Idx, Text.size());
    if (Idx == 0uz)
    {
        return 0uz;
    }
    --Idx;
    while (Idx > 0uz && IsContinuationByte(Text[Idx]))
    {
        --Idx;
    }
    return Idx;
}

//# @return The beginning of the code point after the one at the given index.
NODISCARD inline constexpr std::size_t NextCodePoint(LStringView Text, std::size_t Idx) noexcept
{
    if (Idx >= Text.size())
    {
        return Text.size();
    }
    ++Idx;
    while (Idx < Text.size() && IsContinuationByte(Text[Idx]))
    {
        ++Idx;
    }
    return Idx;
}

//# @return The beginning of the word in front of the given index. Skips leading spaces.
NODISCARD inline constexpr std::size_t PrevWordBoundary(LStringView Text, std::size_t Idx) noexcept
{
    Idx = SnapToCodePoint(Text, Idx);
    while (Idx > 0uz && Classify(Text[PrevCodePoint(Text, Idx)]) == ECharClass::Space)
    {
        Idx = PrevCodePoint(Text, Idx);
    }
    if (Idx == 0uz)
    {
        return 0uz;
    }
    const ECharClass Class{Classify(Text[PrevCodePoint(Text, Idx)])};
    while (Idx > 0uz && Classify(Text[PrevCodePoint(Text, Idx)]) == Class)
    {
        Idx = PrevCodePoint(Text, Idx);
    }
    return Idx;
}

//#
//# On Windows: The beginning of the next word after the given index.
//# Elsewhere:  The end of the word behind the given index. Skips leading spaces.
//#
NODISCARD inline constexpr std::size_t NextWordBoundary(LStringView Text, std::size_t Idx) noexcept
{
    Idx = SnapToCodePoint(Text, Idx);
#if JAFG_PLATFORM_WINDOWS
    if (Idx < Text.size() && Classify(Text[Idx]) != ECharClass::Space)
    {
        const ECharClass Class{Classify(Text[Idx])};
        while (Idx < Text.size() && Classify(Text[Idx]) == Class)
        {
            Idx = NextCodePoint(Text, Idx);
        }
    }
    while (Idx < Text.size() && Classify(Text[Idx]) == ECharClass::Space)
    {
        Idx = NextCodePoint(Text, Idx);
    }
#else /* JAFG_PLATFORM_WINDOWS */
    while (Idx < Text.size() && Classify(Text[Idx]) == ECharClass::Space)
    {
        Idx = NextCodePoint(Text, Idx);
    }
    if (Idx < Text.size())
    {
        const ECharClass Class{Classify(Text[Idx])};
        while (Idx < Text.size() && Classify(Text[Idx]) == Class)
        {
            Idx = NextCodePoint(Text, Idx);
        }
    }
#endif /* !JAFG_PLATFORM_WINDOWS */
    return Idx;
}

//#
//# @return The range [first, second) of characters around the given index that share the same character class.
//#         E.g., the word that was double-clicked.
//#
NODISCARD inline constexpr std::pair<std::size_t, std::size_t> WordAt(LStringView Text, std::size_t Idx) noexcept
{
    Idx = SnapToCodePoint(Text, Idx);
    if (Text.empty())
    {
        return {0uz, 0uz};
    }
    if (Idx == Text.size())
    {
        Idx = PrevCodePoint(Text, Idx);
    }
    const ECharClass Class{Classify(Text[Idx])};
    std::size_t Begin{Idx};
    while (Begin > 0uz && Classify(Text[PrevCodePoint(Text, Begin)]) == Class)
    {
        Begin = PrevCodePoint(Text, Begin);
    }
    std::size_t End{Idx};
    while (End < Text.size() && Classify(Text[End]) == Class)
    {
        End = NextCodePoint(Text, End);
    }
    return {Begin, End};
}

//# The replacement character (U+FFFD) in UTF-8.
inline constexpr LStringView ReplacementCharacter{ "\xEF\xBF\xBD" };

//#
//# @return The byte length of the well-formed UTF-8 sequence at the given index, or zero if the byte at the index is
//#         ill-formed. Follows the same rules as HarfBuzz (and ICU's U8_NEXT): overlong encodings, surrogates, and code
//#         points beyond U+10FFFF are ill-formed.
//#
NODISCARD inline constexpr std::size_t WellFormedSequenceLength(LStringView Text, std::size_t Idx) noexcept
{
    auto Byte{[&](std::size_t Offset) noexcept -> u32 { return static_cast<u8>(Text[Idx + Offset]); }};
    auto IsTrail{[&](std::size_t Offset) noexcept -> bool { return Idx + Offset < Text.size() && (Byte(Offset) & 0xC0u) == 0x80u; }};

    const u32 Lead{Byte(0uz)};
    if (Lead < 0x80u)
    {
        return 1uz;
    }
    if (Lead >= 0xC2u && Lead <= 0xDFu)
    {
        return IsTrail(1uz) ? 2uz : 0uz;
    }
    if (Lead >= 0xE0u && Lead <= 0xEFu)
    {
        if (!IsTrail(1uz) || !IsTrail(2uz))
        {
            return 0uz;
        }
        const u32 CodePoint{((Lead & 0x0Fu) << 12) | ((Byte(1uz) & 0x3Fu) << 6) | (Byte(2uz) & 0x3Fu)};
        return CodePoint < 0x800u || (CodePoint >= 0xD800u && CodePoint <= 0xDFFFu) ? 0uz : 3uz;
    }
    if (Lead >= 0xF0u && Lead <= 0xF4u)
    {
        if (!IsTrail(1uz) || !IsTrail(2uz) || !IsTrail(3uz))
        {
            return 0uz;
        }
        const u32 CodePoint{((Lead & 0x07u) << 18) | ((Byte(1uz) & 0x3Fu) << 12) | ((Byte(2uz) & 0x3Fu) << 6) | (Byte(3uz) & 0x3Fu)};
        return CodePoint < 0x10000u || CodePoint > 0x10FFFFu ? 0uz : 4uz;
    }
    return 0uz;
}

//#
//# @return The text with every ill-formed byte replaced by one U+FFFD. This is exactly how the shaper renders such
//#         text, so after this, every drawn replacement character is also one caret stop.
//#
NODISCARD inline LString ReplaceInvalidUtf8(LStringView Text)
{
    LString Result;
    Result.reserve(Text.size());
    for (std::size_t Idx{}; Idx < Text.size();)
    {
        if (const std::size_t Length{WellFormedSequenceLength(Text, Idx)}; Length > 0uz)
        {
            Result.append(Text.substr(Idx, Length));
            Idx += Length;
        }
        else
        {
            Result.append(ReplacementCharacter);
            ++Idx;
        }
    }
    return Result;
}

//#
//# Makes text that came from outside (e.g., the clipboard) safe for a single line text box.
//# Ill-formed UTF-8 is replaced (see #ReplaceInvalidUtf8), trailing line breaks are removed, inner line breaks and tabs
//# become spaces, and other control characters are removed.
//#
NODISCARD inline LString SanitizeSingleLine(LStringView InText)
{
    /* Replace first. Removing control characters must never join stray bytes into a new character. */
    const LString Valid{ReplaceInvalidUtf8(InText)};
    LStringView Text{Valid};

    while (!Text.empty() && (Text.back() == '\n' || Text.back() == '\r'))
    {
        Text.remove_suffix(1);
    }

    LString Result;
    Result.reserve(Text.size());
    for (char C: Text)
    {
        if (C == '\n' || C == '\t')
        {
            Result.push_back(' ');
            continue;
        }
        if (static_cast<u8>(C) < 0x20 || static_cast<u8>(C) == 0x7F)
        {
            continue;
        }
        Result.push_back(C);
    }
    return Result;
}

} /* ~Namespace TextEditing */

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which glyphs will be inserted.
//#
struct LCaretBrush final
{
    LColor Tint{ Colors::White };
    //# The size in percent from the default. Default is (2px x DesiredSize.y).
    LVec2F Size{ 1.0f, 0.75f };
    //# Horizontal offset.
    f32 HOffset{ 0.0f };
    //# The blinker speed of the caret in seconds.
    f32 CaretBlinkerSpeed{ 0.5f };
    //# The tint of the highlight behind selected text.
    LColor SelectionTint{ 51, 120, 230, 150 };
};

//# A text button which the user can edit.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextButton)
class ENGINE_API WEditableTextButton : public WTextButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditableTextButton)
    {
        this->Anchor = EAnchor::Fill;
        this->bTextAffectsDesiredWidth = false;
        this->TextCutoff = ETextCutoff::Cutoff;
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    virtual LNodeReply OnCursorEnter() override;
    virtual void OnCursorLeave() override;
    virtual void OnFocusReceived() override;
    virtual void OnFocusLost() override;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    LCaretBrush CaretBrush;

    //#
    //# Optional. A bar that fills the box from the left by this fraction [0,1] of its width, behind the text. E.g.,
    //# where a value is between a min and a max (@see LInputRange). Empty draws nothing.
    //#
    std::optional<f32> FillFraction;
    LColor FillTint{ 0x40, 0x57, 0x6F };
    LColor DisabledFillTint{ 0x30, 0x30, 0x30 };

    //# Whether all content is selected when this node receives focus (as if the user pressed Ctrl+A).
    bool bSelectAllOnFocus{};

    //# The placeholder text is a text that is displayed when no content is available.
    LString PlaceholderContent;
    LColor PlaceholderTint{ Colors::Gray };

    //#
    //# Delegate, which is invoked when the user tries to commit the content of the editable text box.
    //# @return True, if the user is allowed to commit the content, false otherwise.
    //# @note   This is useful if the content cannot be applied due to some validation, and handling the content validation
    //#         directly with the #LEditableTextBoxPredicateDelegate is not desired or viable.
    //#
    EVENT_DECL(OnAllowContentCommit, bool(WEditableTextButton& Self))

    //#
    //# Delegate, which is invoked when the user commits the content of the editable text box.
    //#
    EVENT_DECL(OnContentCommitted, void(WEditableTextButton& Self, LString const& Content, ETextCommit Commit))

    //#
    //# Delegate, which is invoked when the contents of the editable text box changed in any way.
    //#
    EVENT_DECL(OnContentChanged, void(WEditableTextButton& Self, LString const& Content))

    //#
    //# A delegate that is invoked, if bounded, when the user tries to change the content of the editable text box.
    //# @return True, if the content change is allowed, false otherwise. E.g.: you might only want to allow the user to
    //#         type in numbers, or you want to validate the content before allowing it. If false is returned, the
    //#         the content will not be changed.
    //# @note   After this delegate is invoked and the new content was accepted, the #LEditableTextBoxChangedDelegate
    //#         will still be invoked. Change events should be handled there. This should only be used for validation.
    //#         If this validation fails, the #LEditableTextBoxChangedDelegate will not be invoked.
    //#
    EVENT_DECL(ContentPredicate, bool(LString const& Content))

    //#
    //# The caret and the selection anchor are byte indices into the content and are always at code point boundaries.
    //# The selected range is everything in between them. If both are equal, nothing is selected.
    //#
    NODISCARD FORCEINLINE std::size_t GetCaretCursor() const noexcept { return this->CaretCursor; }
    NODISCARD FORCEINLINE std::size_t GetSelectionAnchor() const noexcept { return this->SelectionAnchor; }
    NODISCARD FORCEINLINE bool HasSelection() const noexcept { return this->CaretCursor != this->SelectionAnchor; }
    //# @return The selected range [first, second).
    NODISCARD FORCEINLINE std::pair<std::size_t, std::size_t> GetSelectionRange() const noexcept
    {
        return {maths::min(this->CaretCursor, this->SelectionAnchor), maths::max(this->CaretCursor, this->SelectionAnchor)};
    }
    NODISCARD FORCEINLINE LStringView GetSelectedContent() const noexcept
    {
        auto [Begin, End]{this->GetSelectionRange()};
        return LStringView{this->GetContent()}.substr(Begin, End - Begin);
    }
    //# Selects the range between the anchor and the caret. The caret is the end that moves.
    void SetSelection(std::size_t Anchor, std::size_t Caret) noexcept;
    FORCEINLINE void SelectAll() noexcept { this->SetSelection(0uz, this->GetContent().size()); }
    FORCEINLINE void ClearSelection() noexcept { this->SetSelection(this->CaretCursor, this->CaretCursor); }

    //# Moves the caret and clears the selection. @return The actual caret that was set.
    FORCEINLINE std::size_t SetCaretCursor(std::size_t Idx) noexcept
    {
        this->SetSelection(Idx, Idx);
        return this->CaretCursor;
    }
    FORCEINLINE std::size_t SetCaretCursorToBegin() noexcept { return this->SetCaretCursor(0uz); }
    FORCEINLINE std::size_t SetCaretCursorToEnd() noexcept { return this->SetCaretCursor(this->GetContent().size()); }
    //# Moves the caret one code point to the left.
    FORCEINLINE std::size_t ReduceCaretCursor() noexcept
    {
        return this->SetCaretCursor(TextEditing::PrevCodePoint(this->GetContent(), this->CaretCursor));
    }
    //# Moves the caret one code point to the right.
    FORCEINLINE std::size_t IncreaseCaretCursor() noexcept
    {
        return this->SetCaretCursor(TextEditing::NextCodePoint(this->GetContent(), this->CaretCursor));
    }
    //# Moves the caret to the glyph closest to the given location in surface space.
    std::size_t MoveCaretTo(LVec2F Location);

    //# Replaces the selection (or inserts at the caret) as if the user typed the text. Respects the #ContentPredicate.
    bool InsertText(LStringView Text);
    //# Removes the selected content. Respects the #ContentPredicate.
    bool DeleteSelection();
    //# Copies the selected content to the platform clipboard.
    void CopySelection() const;
    //# Copies the selected content to the platform clipboard and removes it.
    bool CutSelection();
    //# Inserts the text of the platform clipboard at the caret. Ill-formed UTF-8 is replaced with U+FFFD.
    bool PasteFromClipboard();
    bool Undo();
    bool Redo();
    NODISCARD FORCEINLINE bool CanUndo() const noexcept { return !this->UndoHistory.empty(); }
    NODISCARD FORCEINLINE bool CanRedo() const noexcept { return !this->RedoHistory.empty(); }

protected:

    virtual void OnChangedImpl() noexcept override;

private:

    //# Used to merge consecutive edits of the same kind into one undo step.
    enum struct EEditKind: u8 { Typing, Deletion, Other };
    struct LEditSnapshot final
    {
        LString Content;
        std::size_t CaretCursor;
        std::size_t SelectionAnchor;
    };
    inline static constexpr std::size_t MaxUndoHistory{ 100 };
    //# Max time in seconds between two clicks to be registered as a double (or triple) click.
    inline static constexpr f64 MultiClickTime{ 0.5 };
    //# Max distance in pixels between two clicks to be registered as a double (or triple) click.
    inline static constexpr f32 MultiClickDistance{ 4.0f };

    bool UserInterfaceTick();

    inline void OnTextCommit(ETextCommit Commit)
    {
        (void)this->OnContentCommitted.InvokeIfBound(*this, this->GetContent(), Commit);
    }

    LNodeReply HandleEditingKey(LNodeKeyEventInfo const& Info, LKeyEvent const& Event);
    void OnMousePressed(LNodeKeyEventInfo const& Info, LKeyEvent const& Event);
    //# Moves the caret. If extending, the anchor stays where it is, otherwise the selection is cleared.
    void MoveCaret(std::size_t Idx, bool bExtendSelection) noexcept;
    //#
    //# Applies a change that the user requested.
    //# @return False, if the #ContentPredicate rejected the new content.
    //#
    bool ApplyUserEdit(LString NewContent, std::size_t NewCaret, EEditKind Kind);
    bool ReplaceRange(std::size_t Begin, std::size_t End, LStringView With, EEditKind Kind);
    void RestoreSnapshot(LEditSnapshot Snapshot);

    //# Updates the glyph data if the content changed since the last layout pass.
    void EnsureTextLayout() const;
    //# @return The byte index of the caret stop that is the closest to the given location in surface space.
    NODISCARD std::size_t HitTestCaret(LVec2F Location) const;
    //# @return The horizontal pencil location (relative to the text origin, without scroll) of the caret stop.
    NODISCARD f32 GetCaretPencilLocation(std::size_t Idx) const;

    bool bBeamed{};
    //# Whether the current content change originates from this node (and not from #SetContent).
    bool bApplyingUserEdit{};
    //# Whether the user is currently selecting text by dragging the mouse.
    bool bMouseSelecting{};
    f32 CaretBlinker{};
    std::size_t CaretCursor{};
    std::size_t SelectionAnchor{};
    //# The content size after the last change. Used to detect a selection of everything when the content changes.
    std::size_t LastContentSize{};
    //# If set, the selection only changes to the location of this anchor, once the mouse moved while dragging.
    std::optional<std::size_t> PendingMouseSelectionAnchor;

    u64 FocusReceivedFrame{ std::numeric_limits<u64>::max() };
    u8 ClickCount{};
    f64 LastClickTime{};
    LVec2F LastClickLocation{ maths::zero_vector<LVec2F> };

    EEditKind LastEditKind{ EEditKind::Other };
    TArray<LEditSnapshot> UndoHistory;
    TArray<LEditSnapshot> RedoHistory;

    //# Horizontal scroll of the text in pixels when the text is wider than the box.
    mutable f32 HScroll{};
    //# Surface space data from the last draw call. Used for hit testing the mouse.
    mutable std::optional<f32> LastTextOriginX;
    mutable std::optional<LRect2F> LastDrawnRect;

    //# @see #FillFraction
    void DrawFill(LNodeRenderInfo const& Info) const;

    LDelegateHandle UserInterfaceTickDelegateHandle;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextButtonIconizedLeft)
class ENGINE_API WEditableTextButtonIconizedLeft : public WEditableTextButton, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditableTextButtonIconizedLeft)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    // TODO: Also add decoupled icon; Extra class?

private:

    NODISCARD LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeLeftIconTopLeft
                <&WEditableTextButtonIconizedLeft::Icon, &WEditableTextButtonIconizedLeft::IconBrush>(*this, this->Brush.Padding);
    }
};

struct LFactoryEditableTextButton : NODE_FACTORY_PARENT(WEditableTextButton)
{
    NODE_FACTORY_BODY(WEditableTextButton)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnAllowContentCommit, OnAllowContentCommit)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnContentCommitted, OnContentCommitted)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnContentChanged, OnContentChanged)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(ContentPredicate, ContentPredicate)

    decltype(auto) PlaceholderContent(this auto&& Self, LString Content) noexcept
    {
        NODE_FACTORY_SELF().PlaceholderContent = std::move(Content);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PlaceHolderTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().PlaceholderTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBrush(this auto&& Self, LCaretBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush = Brush;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CaretTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretSize(this auto&& Self, LVec2F const& Size) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.Size = Size;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretHOffset(this auto&& Self, f32 Offset) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.HOffset = Offset;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBlinkerSpeed(this auto&& Self, f32 Speed) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.CaretBlinkerSpeed = Speed;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectionTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().CaretBrush.SelectionTint = Tint;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) SelectAllOnFocus(this auto&& Self, bool bSelectAll) noexcept
    {
        NODE_FACTORY_SELF().bSelectAllOnFocus = bSelectAll;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
