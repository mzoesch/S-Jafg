// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/TextBox.h"
#include "Widgets/EditableTextBoxForward.h"
#include "EditableTextBox.generated.h"

namespace Jafg
{

struct LFactoryEditableTextBox;

//#
//# A simple text box that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
//# pasting, etc.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditableTextBox)
class WEditableTextBox : public WTextBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBox(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetAnchor(EAnchor::Fill);
        this->SetPadding({5.0, 4.0});
        this->SetType(ERegionBrush::Box);
        this->SetTint(Colors::Black);
        this->SetTextColor(Colors::White);
    }
    template<typename TCxxClass>
    explicit WEditableTextBox(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetAnchor(EAnchor::Fill);
        this->SetPadding({5.0, 4.0});
        this->SetType(ERegionBrush::Box);
        this->SetTint(Colors::Black);
        this->SetTextColor(Colors::White);
    }

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;

    //# This only ticks the user interface.
    void UserInterfaceTick(const LViewport& InViewport);

    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void         OnFocusReceived() override;
    virtual void         OnFocusLost() override;
    virtual LReply       OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    void OnTextCommit(const LString& InText, const ETextCommit InCommitType);

    //# @see #LEditableTextBoxAllowCommitDelegate
    LEditableTextBoxAllowCommitDelegate OnAllowContentCommit;

    //# @see #LEditableTextBoxCommitDelegate
    LEditableTextBoxCommitDelegate  OnContentCommitted;

    //# @see #LEditableTextBoxChangedDelegate
    LEditableTextBoxChangedDelegate OnContentChanged;

    //# @see #LEditableTextBoxPredicateDelegate
    LEditableTextBoxPredicateDelegate ContentPredicate;

    FORCEINLINE void SetPlaceholderContent(const LString& InText) noexcept { this->PlaceholderContent = InText; }
    FORCEINLINE void SetPlaceholderContent(LString&& InText) noexcept { this->PlaceholderContent = std::move(InText); }
    FORCEINLINE const LString& GetPlaceHolderContent() const noexcept { return this->PlaceholderContent; }

    FORCEINLINE constexpr void SetPlaceholderColor(const LColor& InColor) noexcept { this->PlaceholderColor = InColor; }
    FORCEINLINE constexpr void SetCaretColor(const LColor& InColor) noexcept { this->CaretBrush.Tint = InColor; }
    FORCEINLINE constexpr void SetCaretSize(const LVec2F& InSize) noexcept { this->CaretBrush.Size = InSize; }
    FORCEINLINE constexpr void SetCaretHOffset(const f64 InOffset) noexcept { this->CaretBrush.HOffset = InOffset; }
    FORCEINLINE constexpr void SetCaretBlinkerSpeed(const f64 InSpeed) noexcept { this->CaretBrush.CaretBlinkerSpeed = InSpeed; }

    FORCEINLINE constexpr void SetBrush(const LEditableTextBoxBrush& InBrush) noexcept;

    FORCEINLINE constexpr void SetCaretBrush(const LCaretBrush& InBrush) noexcept { this->CaretBrush = InBrush; }
    FORCEINLINE constexpr LCaretBrush& GetMutableCaretBrush() noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const LCaretBrush& GetCaretBrush() const noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const LColor& GetPlaceholderColor() const noexcept { return this->PlaceholderColor; }
    FORCEINLINE constexpr const LColor& GetCaretColor() const noexcept { return this->CaretBrush.Tint; }
    FORCEINLINE constexpr const LVec2F& GetCaretSize() const noexcept { return this->CaretBrush.Size; }
    FORCEINLINE constexpr f64 GetCaretHOffset() const noexcept { return this->CaretBrush.HOffset; }
    FORCEINLINE constexpr f64 GetCaretBlinkerSpeed() const noexcept { return this->CaretBrush.CaretBlinkerSpeed; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

    //# Checks whether the content can be interpreted as a floating point number or the content is not empty.
    static bool IsContentFloatingPoint(LString const& InContent) noexcept;

private:

    void OnSuperContentChanged(const LString& InNewContent);

    void MoveCaretToMouseCursor(const LViewport& Viewport);
    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    //#
    //# The placeholder text is a text that is displayed when no content is available.
    //#
    LString PlaceholderContent;
    LColor PlaceholderColor{ Colors::Gray };

    LCaretBrush CaretBrush;
    i32 CaretCursor{};
    f32 CaretBlinker{};

    LDelegateHandle UserInterfaceTickDelegateHandle{ nullptr };
};


struct LFactoryEditableTextBox : NODE_FACTORY_PARENT(WEditableTextBox)
{
    NODE_FACTORY_BODY(WEditableTextBox)


    decltype(auto) PlaceholderContent(this auto&& Self, const LString& InText) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceholderContent(InText);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PlaceholderContent(this auto&& Self, LString&& InText) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceholderContent(std::move(InText));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PlaceholderColor(this auto&& Self, const LColor& InColor) noexcept
    {
        NODE_FACTORY_SELF().SetPlaceholderColor(InColor);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CaretColor(this auto&& Self, const LColor& InColor) noexcept
    {
        NODE_FACTORY_SELF().SetCaretColor(InColor);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretSize(this auto&& Self, const LVec2F& InSize) noexcept
    {
        NODE_FACTORY_SELF().SetCaretSize(InSize);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretHOffset(this auto&& Self, const f32 InOffset) noexcept
    {
        NODE_FACTORY_SELF().SetCaretHOffset(InOffset);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBlinkerSpeed(this auto&& Self, const f32 InSpeed) noexcept
    {
        NODE_FACTORY_SELF().SetCaretBlinkerSpeed(InSpeed);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) CaretBrush(this auto&& Self, const LCaretBrush& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetCaretBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Brush(this auto&& Self, const LEditableTextBoxBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Brush(this auto&& Self, LEditableTextBoxBrush&& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(std::move(InBrush));
        return NODE_FACTORY_RESULT();
    }

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
};

FORCEINLINE constexpr void WEditableTextBox::SetBrush(LEditableTextBoxBrush const& InBrush) noexcept
{
    this->PlaceholderColor = InBrush.PlaceholderColor;
    Super::SetBrush(InBrush);
    return;
}

} /* ~Namespace Jafg */
