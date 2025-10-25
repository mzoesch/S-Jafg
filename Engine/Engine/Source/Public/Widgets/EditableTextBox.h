// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/TextBox.h"
#include "Widgets/EditableTextBoxForward.h"
#include "EditableTextBox.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryEditableTextBox : public TWidgetFactoryTextBox<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryBox)

    FORCEINLINE TFactoryRetTy& PlaceholderContent(const LString& InText) noexcept { this->This()->SetPlaceholderContent(InText); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PlaceholderContent(LString&& InText) noexcept { this->This()->SetPlaceholderContent(std::move(InText)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& PlaceholderColor(const Lal::LColor& InColor) noexcept { this->This()->SetPlaceholderColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretColor(const Lal::LColor& InColor) noexcept { this->This()->SetCaretColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretSize(const LVector2& InSize) noexcept { this->This()->SetCaretSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretHOffset(const f32 InOffset) noexcept { this->This()->SetCaretHOffset(InOffset); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBlinkerSpeed(const f32 InSpeed) noexcept { this->This()->SetCaretBlinkerSpeed(InSpeed); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBrush(const LCaretBrush& InBrush) noexcept { this->This()->SetCaretBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Brush(const LEditableTextBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Brush(LEditableTextBoxBrush&& InBrush) { this->This()->SetBrush(std::move(InBrush)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& OnAllowCommit(LEditableTextBoxAllowCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnAllowContentCommit.BindFunction(std::move(InCallback)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnCommit(LEditableTextBoxCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentCommitted.BindFunction(std::move(InCallback)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnChanged(LEditableTextBoxChangedDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentChanged.BindFunction(std::move(InCallback)); return this->Self(); }
};

//#
//# A simple text box that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
//# pasting, etc.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryEditableTextBox)
class WEditableTextBox : public WTextBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBox(const LCxxObjectInitializer& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WEditableTextBox)

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

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

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

    FORCEINLINE constexpr void SetPlaceholderColor(const Lal::LColor& InColor) noexcept { this->PlaceholderColor = InColor; }
    FORCEINLINE constexpr void SetCaretColor(const Lal::LColor& InColor) noexcept { this->CaretBrush.Tint = InColor; }
    FORCEINLINE constexpr void SetCaretSize(const LVector2& InSize) noexcept { this->CaretBrush.Size = InSize; }
    FORCEINLINE constexpr void SetCaretHOffset(const f32 InOffset) noexcept { this->CaretBrush.HOffset = InOffset; }
    FORCEINLINE constexpr void SetCaretBlinkerSpeed(const f32 InSpeed) noexcept { this->CaretBrush.CaretBlinkerSpeed = InSpeed; }

    FORCEINLINE constexpr void SetBrush(const LEditableTextBoxBrush& InBrush) noexcept;
    FORCEINLINE constexpr void SetBrush(LEditableTextBoxBrush&& InBrush) noexcept;

    FORCEINLINE constexpr void SetCaretBrush(const LCaretBrush& InBrush) noexcept { this->CaretBrush = InBrush; }
    FORCEINLINE constexpr LCaretBrush& GetMutableCaretBrush() noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const LCaretBrush& GetCaretBrush() const noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const Lal::LColor& GetPlaceholderColor() const noexcept { return this->PlaceholderColor; }
    FORCEINLINE constexpr const Lal::LColor& GetCaretColor() const noexcept { return this->CaretBrush.Tint; }
    FORCEINLINE constexpr const LVector2& GetCaretSize() const noexcept { return this->CaretBrush.Size; }
    FORCEINLINE constexpr f32 GetCaretHOffset() const noexcept { return this->CaretBrush.HOffset; }
    FORCEINLINE constexpr f32 GetCaretBlinkerSpeed() const noexcept { return this->CaretBrush.CaretBlinkerSpeed; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

    //# Checks whether the content can be interpreted as a floating point number or the content is not empty.
    static bool IsContentFloatingPoint(const LString& InContent) noexcept;

private:

    void OnSuperContentChanged(const LString& InNewContent);

    void MoveCaretToMouseCursor(const LViewport& Viewport);
    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    //#
    //# The placeholder text is a text that is displayed when no content is available.
    //#
    LString PlaceholderContent;
    Lal::LColor PlaceholderColor { Lal::LColor::Gray };

    LCaretBrush CaretBrush;
    i32 CaretCursor { 0 };
    f32 CaretBlinker { 0.0f };

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };
};

FORCEINLINE constexpr void WEditableTextBox::SetBrush(const LEditableTextBoxBrush& InBrush) noexcept
{
    this->PlaceholderColor = InBrush.PlaceholderColor;

    Super::SetBrush(InBrush);

    return;
}

FORCEINLINE constexpr void WEditableTextBox::SetBrush(LEditableTextBoxBrush&& InBrush) noexcept
{
    this->PlaceholderColor = std::move(InBrush.PlaceholderColor);

    Super::SetBrush(std::move(InBrush));

    return;
}

} /* ~Namespace Jafg */
