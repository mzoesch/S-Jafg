// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetBox.h"
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
class TWidgetFactoryEditableTextBlock : public TWidgetFactoryWidgetBox<TNode>
{
public:

    using Super         = TWidgetFactoryWidgetBox<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    using Super::operator&;

    TFactoryRetTy& SetTextColor(const LColor& InColor)       { this->This()->SetTextColor(InColor);  return this->Self(); }
    TFactoryRetTy& SetTextScale(const float InScale)         { this->This()->SetTextScale(InScale);  return this->Self(); }
    TFactoryRetTy& SetCaretBrush(const LCaretBrush& InBrush) { this->This()->SetCaretBrush(InBrush); return this->Self(); }
    TFactoryRetTy& SetTextCommitCallback(LEditableTextBlockCommitDelegate::LFunctionSigTy&& InCallback) { this->This()->OnTextCommitted.BindFunction(std::move(InCallback)); return this->Self(); }

    TFactoryRetTy& operator&(LEditableTextBlockCommitDelegate::LFunctionSigTy&& InCallback) { return this->SetTextCommitCallback(std::move(InCallback)); }
};

//#
//# A simple text block that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
//# pasting, etc.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API WEditableTextBlock : public WWidgetBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBlock(const LObjectInitializer& ObjectInitializer);

public:

    using TWidgetFactoryTy = TWidgetFactoryEditableTextBlock<Derived>;

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

private:

    void SafelyReduceCaretCursor();
    void SafelyIncreaseCaretCursor();

    LColor Color = LColor::Black;
    float  Scale = 1.0f;

    LString            Content;
    LFontShaderContext ShaderContext;

    LCaretBrush       CaretBrush;
    int32             CaretCursor  = 0;
    float             CaretBlinker = 0.0f;
    float             CaretBlinkerSpeed = 0.5f;
    LBoxShaderContext CaretShaderContext;
};

} /* ~Namespace Jafg */
