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

    FORCEINLINE TFactoryRetTy& PlaceholderColor(const LColor& InColor) noexcept { this->This()->SetPlaceholderColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretColor(const LColor& InColor) noexcept { this->This()->SetCaretColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretSize(const LVector2& InSize) noexcept { this->This()->SetCaretSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretHOffset(const f32 InOffset) noexcept { this->This()->SetCaretHOffset(InOffset); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBlinkerSpeed(const f32 InSpeed) noexcept { this->This()->SetCaretBlinkerSpeed(InSpeed); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBrush(const LCaretBrush& InBrush) noexcept { this->This()->SetCaretBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& OnAllowCommit(LEditableTextBoxAllowCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnAllowContentCommit.BindFunction(std::move(InCallback)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnCommit(LEditableTextBoxCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentCommitted.BindFunction(std::move(InCallback)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnChanged(LEditableTextBoxChangedDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentChanged.BindFunction(std::move(InCallback)); return this->Self(); }
};

//#
//# A simple text box that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
//# pasting, etc.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryEditableTextBox)
class ENGINE_API WEditableTextBox : public WTextBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBox(const LObjectInitializer& ObjectInitializer);

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
    virtual LReply       OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    LEditableTextBoxAllowCommitDelegate OnAllowContentCommit;
    LEditableTextBoxCommitDelegate  OnContentCommitted;
    LEditableTextBoxChangedDelegate OnContentChanged;

    FORCEINLINE void SetPlaceholderContent(const LString& InText) noexcept { this->PlaceholderContent = InText; }
    FORCEINLINE void SetPlaceholderContent(LString&& InText) noexcept { this->PlaceholderContent = std::move(InText); }
    FORCEINLINE const LString& GetPlaceHolderContent() const noexcept { return this->PlaceholderContent; }

    FORCEINLINE constexpr void SetPlaceholderColor(const LColor& InColor) noexcept { this->PlaceholderColor = InColor; }
    FORCEINLINE constexpr void SetCaretColor(const LColor& InColor) noexcept { this->CaretBrush.Tint = InColor; }
    FORCEINLINE constexpr void SetCaretSize(const LVector2& InSize) noexcept { this->CaretBrush.Size = InSize; }
    FORCEINLINE constexpr void SetCaretHOffset(const f32 InOffset) noexcept { this->CaretBrush.HOffset = InOffset; }
    FORCEINLINE constexpr void SetCaretBlinkerSpeed(const f32 InSpeed) noexcept { this->CaretBrush.CaretBlinkerSpeed = InSpeed; }

    FORCEINLINE constexpr void SetCaretBrush(const LCaretBrush& InBrush) noexcept { this->CaretBrush = InBrush; }
    FORCEINLINE constexpr LCaretBrush& GetMutableCaretBrush() noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const LCaretBrush& GetCaretBrush() const noexcept { return this->CaretBrush; }
    FORCEINLINE constexpr const LColor& GetPlaceholderColor() const noexcept { return this->PlaceholderColor; }
    FORCEINLINE constexpr const LColor& GetCaretColor() const noexcept { return this->CaretBrush.Tint; }
    FORCEINLINE constexpr const LVector2& GetCaretSize() const noexcept { return this->CaretBrush.Size; }
    FORCEINLINE constexpr f32 GetCaretHOffset() const noexcept { return this->CaretBrush.HOffset; }
    FORCEINLINE constexpr f32 GetCaretBlinkerSpeed() const noexcept { return this->CaretBrush.CaretBlinkerSpeed; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

private:

    void OnSuperContentChanged(const LString& InNewContent);

    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    //#
    //# The placeholder text is a text that is displayed when no content is available.
    //#
    LString PlaceholderContent;
    LColor PlaceholderColor { LColor::Gray };

    LCaretBrush CaretBrush;
    i32 CaretCursor { 0 };
    f32 CaretBlinker { 0.0f };

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };
};

} /* ~Namespace Jafg */
