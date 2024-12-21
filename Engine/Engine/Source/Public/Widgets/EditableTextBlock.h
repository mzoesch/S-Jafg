// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetBox.h"
#include "Rhi/FontShaderContext.h"
#include "EditableTextBlock.generated.h"

namespace Jafg
{

/**
 * A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
 * the place at which text or graphics will be inserted.
 */
struct LCaretBrush final
{
    LColor   Color   = LColor::White;
    LVector2 Size    = LVector2(1.0f, 0.85f);
    float    HOffset = 3.0f;
};

namespace ETextCommit
{

enum Type : uint8
{
    /**
     * Text was commit via an enter key press. This does not mean a loss of focus.
     */
    OnEnter,

    /**
     * Focus was lost due to a press of the escape key.
     */
    OnCleared,

    /**
     * Focus was lost due to some other reason.
     */
    FocusLost,
};

} /* ~Namespace ETextCommit */
ENGINE_API LSimpleString LexToString(const ETextCommit::Type InType);

struct LEditableTextBrush : public LBoxBrush
{
    LColor Color = LColor::Black;
    float  Scale = 1.0f;
};

template <typename TNode>
class TWidgetFactoryEditableTextBlock : public TWidgetFactoryWidgetBox<TNode>
{
public:

    using TFactoryRetTy = typename TWidgetFactoryWidgetBox<TNode>::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& SetTextColor(const LColor& InColor)       { this->This()->SetTextColor(InColor);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetTextScale(const float InScale)         { this->This()->SetTextScale(InScale);  return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetCaretBrush(const LCaretBrush& InBrush) { this->This()->SetCaretBrush(InBrush); return this->Self(); }
};

/**
 * A simple text block that is editable by the user with all that comes with it, e.g., caret, text selection, copy,
 * pasting, etc.
 */
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

    void OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType);

    FORCEINLINE void SetTextColor(const LColor& InColor) { this->Color = InColor; }
    FORCEINLINE void SetTextScale(const float InScale)   { this->Scale = InScale; }
    FORCEINLINE auto GetCaretBrush()       ->       LCaretBrush& { return this->CaretBrush; }
    FORCEINLINE auto GetCaretBrush() const -> const LCaretBrush& { return this->CaretBrush; }
    FORCEINLINE void SetCaretBrush(const LCaretBrush& InBrush) { this->CaretBrush = InBrush; }

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
