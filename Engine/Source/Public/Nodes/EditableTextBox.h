// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "EditableTextBox.generated.h"

namespace Jafg
{

struct LFactoryEditableTextBox;

enum struct ETextCommit : u8
{
    //# Text was committed via an enter key (or similar) press. This does not mean a loss of focus.
    OnEnter,
    //# Focus was lost due to a press of the escape key.
    OnCleared,
    //# Focus was lost due to some other reason.
    FocusLost,
};
inline LStringView LexToString(ETextCommit Enum) noexcept
{
    switch (Enum)
    {
    case ETextCommit::OnEnter:   { return "OnEnter"; }
    case ETextCommit::OnCleared: { return "OnCleared"; }
    case ETextCommit::FocusLost: { return "FocusLost"; }
    default:                     { unreachable() }
    }
}

struct LEditableTextBrush : public LTextBrush
{
    LColor PlaceholderTint{ Colors::Gray };
};

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which glyphes will be inserted.
//#
struct LCaretBrush final
{
    LColor Tint{ Colors::White };
    //# The size in percent from the default. Default is (2px x DesiredSize.Y).
    LVec2F Size{ 1.0f, 0.85f };
    f32 HOffset{ 0.0f };
    //# The blinker speed of the caret in seconds.
    f32 CaretBlinkerSpeed{ 0.5f };
};

//# A text box which the user can edit.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextBox)
class WEditableTextBox : public WTextBox
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditableTextBox)
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetAnchor(EAnchor::Fill);
        this->SetTint(Colors::Black);
        this->SetPadding({5.0, 4.0});
    }

public:

    virtual void Construct() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void         OnFocusReceived() override;
    virtual void         OnFocusLost() override;
    virtual LReply       OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;

    //#
    //# Delegate, which is invoked when the user tries to commit the content of the editable text box.
    //# @return True, if the user is allowed to commit the content, false otherwise.
    //# @note   This is useful if the content cannot be applied due to some validation, and handling the content validation
    //#         directly with the #LEditableTextBoxPredicateDelegate is not desired or viable.
    //#
    EVENT_DECL(OnAllowContentCommit, bool())

    //#
    //# Delegate, which is invoked when the user commits the content of the editable text box.
    //#
    EVENT_DECL(OnContentCommitted, void(LString const&, ETextCommit))

    //#
    //# Delegate, which is invoked when the contents of the editable text box changed in any way.
    //#
    EVENT_DECL(OnContentChanged, void(LString const&))

    //#
    //# A delegate that is invoked, if bounded, when the user tries to change the content of the editable text box.
    //# @return True, if the content change is allowed, false otherwise. E.g.: you might only want to allow the user to
    //#         type in numbers, or you want to validate the content before allowing it. If false is returned, the
    //#         the content will not be changed.
    //# @note   After this delegate is invoked and the new content was accepted, the #LEditableTextBoxChangedDelegate
    //#         will still be invoked. Change events should be handled there. This should only be used for validation.
    //#         If this validation fails, the #LEditableTextBoxChangedDelegate will not be invoked.
    //#
    EVENT_DECL(ContentPredicate, bool(LString const&))

    constexpr void SetPlaceholderContent(LString const& InPlaceholderContent) noexcept { this->PlaceholderContent = InPlaceholderContent; }
    constexpr void SetPlaceholderContent(LString&& InPlaceholderContent) noexcept { this->PlaceholderContent = std::move(InPlaceholderContent); }
    constexpr LString const& GetPlaceHolderContent() const noexcept { return this->PlaceholderContent; }

    constexpr void SetEditableTextBrush(LEditableTextBrush const& InBrush) noexcept
    {
        this->PlaceholderTint = InBrush.PlaceholderTint;
        Super::SetTextBrush(InBrush);
    }
    constexpr LEditableTextBrush CopyEditableTextBrush() const noexcept
    {
        LEditableTextBrush Brush{this->GetTextBrush()};
        Brush.PlaceholderTint = this->PlaceholderTint;
        return Brush;
    }

    constexpr void SetPlaceHolderTint(LColor const& InPlaceholderTint) noexcept { this->PlaceholderTint = InPlaceholderTint; }
    constexpr LColor const& GetPlaceHolderTint() const noexcept { return this->PlaceholderTint; }

    constexpr void SetCaretBrush(LCaretBrush const& InBrush) noexcept { this->CaretBrush = InBrush; }
    constexpr LCaretBrush& GetMutableCaretBrush() noexcept { return this->CaretBrush; }
    constexpr LCaretBrush const& GetCaretBrush() const noexcept { return this->CaretBrush; }
    constexpr void SetCaretTint(LColor const& InTint) noexcept { this->CaretBrush.Tint = InTint; }
    constexpr void SetCaretSize(const LVec2F& InSize) noexcept { this->CaretBrush.Size = InSize; }
    constexpr void SetCaretHOffset(f32 InOffset) noexcept { this->CaretBrush.HOffset = InOffset; }
    constexpr void SetCaretBlinkerSpeed(f32 InSpeed) noexcept { this->CaretBrush.CaretBlinkerSpeed = InSpeed; }
    constexpr LColor const& GetCaretTint() const noexcept { return this->CaretBrush.Tint; }
    constexpr LVec2F GetCaretSize() const noexcept { return this->CaretBrush.Size; }
    constexpr f32 GetCaretHOffset() const noexcept { return this->CaretBrush.HOffset; }
    constexpr f32 GetCaretBlinkerSpeed() const noexcept { return this->CaretBrush.CaretBlinkerSpeed; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

private:

    void UserInterfaceTick();

    void OnTextCommit(LString const& InText, ETextCommit InCommitType);

    void OnSuperContentChanged(const LString& InNewContent);

    void MoveCaretToMouseCursor(const LViewport& Viewport);
    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    //#
    //# The placeholder text is a text that is displayed when no content is available.
    //#
    LString PlaceholderContent;
    LColor PlaceholderTint{ Colors::Gray };

    LCaretBrush CaretBrush;
    i32 CaretCursor{};
    f32 CaretBlinker{};

    LDelegateHandle UserInterfaceTickDelegateHandle{ nullptr };
};

struct LFactoryEditableTextBox : NODE_FACTORY_PARENT(WEditableTextBox)
{
    NODE_FACTORY_BODY(WEditableTextBox)

    decltype(auto) OnAllowCommit(this auto&& Self, auto&& InCallback)
    {
        NODE_FACTORY_SELF().OnAllowContentCommit.Bind(std::forward<decltype(InCallback)>(InCallback));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OnCommit(this auto&& Self, auto&& InCallback)
    {
        NODE_FACTORY_SELF().OnContentCommitted.Bind(std::forward<decltype(InCallback)>(InCallback));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OnChanged(this auto&& Self, auto&& InCallback)
    {
        NODE_FACTORY_SELF().OnContentChanged.Bind(std::forward<decltype(InCallback)>(InCallback));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ContentPredicate(this auto&& Self, auto&& InCallback)
    {
        NODE_FACTORY_SELF().ContentPredicate.Bind(std::forward<decltype(InCallback)>(InCallback));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) PlaceholderContent(this auto&& Self, LString const& InPlaceholderContent) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceholderContent(InPlaceholderContent);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PlaceholderContent(this auto&& Self, LString&& InPlaceholderContent) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceholderContent(std::move(InPlaceholderContent));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) PlaceHolderTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceHolderTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) EditableTextBrush(this auto&& Self, LEditableTextBrush const& InBrush)
    {
        NODE_FACTORY_SELF().SetEditableTextBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBrush(this auto&& Self, LCaretBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetCaretBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretTint(this auto&& Self, const LColor& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetCaretTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretSize(this auto&& Self, LVec2F const& InSize) noexcept
    {
        NODE_FACTORY_SELF().SetCaretSize(InSize);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretHOffset(this auto&& Self, f32 InOffset) noexcept
    {
        NODE_FACTORY_SELF().SetCaretHOffset(InOffset);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBlinkerSpeed(this auto&& Self, f32 InSpeed) noexcept
    {
        NODE_FACTORY_SELF().SetCaretBlinkerSpeed(InSpeed);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
