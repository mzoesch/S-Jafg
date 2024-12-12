// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetBox.h"
#include "Rhi/FontShaderContext.h"
#include "EditableTextBlock.generated.h"

namespace Jafg
{

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

private:

    LColor Color = LColor::Black;
    float  Scale = 1.0f;

    LSimpleString      Content;
    LFontShaderContext ShaderContext;
};

} /* ~Namespace Jafg */
