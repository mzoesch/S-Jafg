// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Widgets/Box.h"

namespace Jafg
{

class WEditableTextBlock;
struct LCaretBrush;
struct LEditableTextBrush;
namespace ETextCommit { enum Type : u8; }

MAKE_DELEGATE_SIGNATURE(LEditableTextBlockCommitDelegate, void, const LString&, const ETextCommit::Type)
MAKE_DELEGATE_SIGNATURE(LEditableTextBlockChangedDelegate, void, const LString& NewContent)

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which text or graphics will be inserted.
//#
struct LCaretBrush final
{
    //# The caret color.
    LColor Color { LColor::White };

    //# The size in percent from the default. Default is (2px x DesiredSize.Y).
    LVector2 Size { 1.0f, 0.85f };

    //# The horizontal offset.
    f32 HOffset { 0.0f };
};

namespace ETextCommit
{

enum Type : u8
{
    //#
    //# Text was commit via an enter key press. This does not mean a loss of focus.
    //#
    OnEnter,

    //#
    //# Focus was lost due to a press of the escape key.
    //#
    OnCleared,

    //#
    //# Focus was lost due to some other reason.
    //#
    FocusLost,
};

} /* ~Namespace ETextCommit */
ENGINE_API LString LexToString(const ETextCommit::Type InType);

struct LEditableTextBrush : public LBoxBrush
{
    LColor TextColor { LColor::Black };
    f32    TextScale { 1.0f };
    LColor PlaceholderColor { LColor::Gray };
};

} /* ~Namespace Jafg */
