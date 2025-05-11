// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Box.h"
#include "Rhi/FontShaderContext.h"
#include "Widgets/EditableTextBlockForward.h"
#include "EditableTextBlock.generated.h"

namespace Jafg
{

struct LEditableTextBrush : public LBoxBrush
{
    LColor Color = LColor::Black;
    float  Scale = 1.0f;
};

template <typename TNode>
class TWidgetFactoryEditableTextBlock : public TWidgetFactoryBox<TNode>
{
public:

    using Super         = TWidgetFactoryBox<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& TextColor(const LColor& InColor)       { this->This()->SetTextColor(InColor);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextScale(const float InScale)         { this->This()->SetTextScale(InScale);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& CaretBrush(const LCaretBrush& InBrush) { this->This()->SetCaretBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnCommit(LEditableTextBlockCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnTextCommitted.BindFunction(std::move(InCallback)); return this->Self(); }
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
    virtual void Tick() override;

    virtual void UpdateDesiredSize() const override;

    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void         OnFocusReceived() override;
    virtual void         OnFocusLost() override;
    virtual LReply       OnKeyDown(LKeyEvent& InKeyEvent) override;

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    FORCEINLINE void SetTextColor(const LColor& InColor) { this->Color = InColor; }
    FORCEINLINE void SetTextScale(const float InScale)   { this->Scale = InScale; }
    FORCEINLINE auto GetCaretBrush()       ->       LCaretBrush& { return this->CaretBrush; }
    FORCEINLINE auto GetCaretBrush() const -> const LCaretBrush& { return this->CaretBrush; }
    FORCEINLINE void SetCaretBrush(const LCaretBrush& InBrush) { this->CaretBrush = InBrush; }

    LEditableTextBlockCommitDelegate OnTextCommitted;
    void SetText(const LString& InText);
    void SetText(     LString&& InText);
    void ClearText();
    FORCEINLINE auto GetText() const -> const LString& { return this->Content; }

    //# @return The actual caret that was set.
    i32 SetCaretCursor(const i32 InCaretCursor);
    i32 SetCaretCursorToBegin();
    i32 SetCaretCursorToEnd();

private:

    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    LColor Color = LColor::Black;
    f32    Scale = 1.0f;

    LString Content;
    LFontShaderContext ShaderContext;

    LCaretBrush CaretBrush;
    i32 CaretCursor  = 0;
    f32 CaretBlinker = 0.0f;
    f32 CaretBlinkerSpeed = 0.5f;
    LBoxShaderContext CaretShaderContext;
};

} /* ~Namespace Jafg */
