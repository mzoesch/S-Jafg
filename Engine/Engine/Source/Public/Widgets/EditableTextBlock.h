// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
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

DECLARE_JAFG_CLASS()
class ENGINE_API WEditableTextBlock : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBlock(const LObjectInitializer& ObjectInitializer);

public:

    // WWidgetNode implementation
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual void OnFocusReceived() override;
    virtual void OnFocusLost() override;
    // ~WWidgetNode implementation

    WIDGET_REGION_SUBCLASS_WSDSML_DECLARATIONS(WEditableTextBlock)

    void OnTextCommit(const LSimpleString& InText, const ETextCommit::Type InCommitType);
};

} /* ~Namespace Jafg */
