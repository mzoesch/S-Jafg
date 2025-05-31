// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Box.h"
#include "Rhi/FontShaderContext.h"
#include "Widgets/EditableTextBlockForward.h"
#include "EditableTextBlock.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryEditableTextBlock : public TWidgetFactoryBox<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryBox)

    FORCEINLINE TFactoryRetTy& TextTint(const LColor& InColor)         { this->This()->SetTextTint(InColor);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextScale(const float InScale)          { this->This()->SetTextScale(InScale);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& PlaceholderText(const LString& InText)  { this->This()->SetPlaceholderText(InText); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PlaceholderText(     LString&& InText)  { this->This()->SetPlaceholderText(std::move(InText)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PlaceholderColor(const LColor& InColor) { this->This()->SetPlaceholderColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretColor(const LColor& InColor)       { this->This()->SetCaretColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretSize(const LVector2& InSize)       { this->This()->SetCaretSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretHOffset(const f32 InOffset)        { this->This()->SetCaretHOffset(InOffset); return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBrush(const LCaretBrush& InBrush)  { this->This()->SetCaretBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnCommit(LEditableTextBlockCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentCommitted.BindFunction(std::move(InCallback)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnChanged(LEditableTextBlockChangedDelegate::LFunctionSigTy&& InCallback) { this->This()->OnContentChanged.BindFunction(std::move(InCallback)); return this->Self(); }
};

//#
//# A simple text block that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
//# pasting, etc.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryEditableTextBlock)
class ENGINE_API WEditableTextBlock : public WBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBlock(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    //# This only ticks the user interface.
    void UserInterfaceTick(const LViewport& InViewport);

    virtual void UpdateDesiredSize() const override;

    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void         OnFocusReceived() override;
    virtual void         OnFocusLost() override;
    virtual LReply       OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    FORCEINLINE LCaretBrush& GetMutableCaretBrush() { return this->CaretBrush; }
    FORCEINLINE const LCaretBrush& GetCaretBrush() const { return this->CaretBrush; }
    FORCEINLINE void SetCaretBrush(const LCaretBrush& InBrush) { this->CaretBrush = InBrush; }

    FORCEINLINE void SetTextTint(const LColor& InColor) { this->TextTint = InColor; }
    FORCEINLINE void SetTextScale(const f32 InScale)   { this->TextScale = InScale; }
    FORCEINLINE void SetPlaceholderColor(const LColor& InColor) { this->PlaceholderColor = InColor; }
    FORCEINLINE void SetPlaceholderText(const LString& InText) { this->Placeholder = InText; }
    FORCEINLINE void SetPlaceholderText(     LString&& InText) { this->Placeholder = std::move(InText); }
    FORCEINLINE void SetCaretColor(const LColor& InColor) { this->CaretBrush.Color = InColor; }
    FORCEINLINE void SetCaretSize(const LVector2& InSize) { this->CaretBrush.Size = InSize; }
    FORCEINLINE void SetCaretHOffset(const f32 InOffset) { this->CaretBrush.HOffset = InOffset; }

    LEditableTextBlockCommitDelegate  OnContentCommitted;
    LEditableTextBlockChangedDelegate OnContentChanged;
    void SetContent(const LString& InContent);
    void SetContent(     LString&& InContent);
    void ClearContent();
    FORCEINLINE const LString& GetContent() const { return this->Content; }

    FORCEINLINE const LString& GetPlaceholder() const { return this->Placeholder; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

private:

    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    LColor TextTint { LColor::Black };
    f32    TextScale { 1.0f };
    LColor PlaceholderColor { LColor::Gray };

    //#
    //# The actual text content currently present in the editable text block.
    //#
    LString Content;

    //#
    //# The placeholder text is a text that is displayed when not content is available.
    //#
    LString Placeholder;

    LFontShaderContext ShaderContext;

    LCaretBrush CaretBrush;
    i32 CaretCursor  { 0 };
    f32 CaretBlinker { 0.0f };
    f32 CaretBlinkerSpeed { 0.5f };

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };
};

} /* ~Namespace Jafg */
