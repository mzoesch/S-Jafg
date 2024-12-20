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

DECLARE_JAFG_CLASS()
class ENGINE_API WEditableTextBlock : public WWidgetBox
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBlock(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;
    virtual void Tick() override;

    virtual void UpdateDesiredSize() const override;

    // WWidgetNode implementation
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void         OnFocusReceived() override;
    virtual void         OnFocusLost() override;
    virtual LReply       OnKeyDown(LKeyEvent& InKeyEvent) override;
    // ~WWidgetNode implementation

    void OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType);

    FORCEINLINE WEditableTextBlock& SetTextColor(const LColor& InColor) { this->Color = InColor; return *this; }
    FORCEINLINE WEditableTextBlock& SetTextScale(const float InScale)   { this->Scale = InScale; return *this; }
    FORCEINLINE       LCaretBrush&  GetCaretBrush()                     { return this->CaretBrush; }
    FORCEINLINE const LCaretBrush&  GetCaretBrush() const               { return this->CaretBrush; }
    FORCEINLINE WEditableTextBlock& SetCaretBrush(const LCaretBrush& InBrush) { this->CaretBrush = InBrush; return *this; }

    template <typename TNode> FORCEINLINE TNode& operator>>(TNode*& OutNode) { OutNode = this; return *this; }
    template <typename TNode> FORCEINLINE TNode& operator>>(TNode& OutNode)  { OutNode = this; return *this; }

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
