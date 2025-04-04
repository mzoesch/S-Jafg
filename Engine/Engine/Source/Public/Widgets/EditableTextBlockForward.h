// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class WEditableTextBlock;
struct LCaretBrush;
struct LEditableTextBrush;
namespace ETextCommit { enum Type : u8; }

MAKE_DELEGATE_SIGNATURE(LEditableTextBlockCommitDelegate, void, const LString&, const ETextCommit::Type)

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which text or graphics will be inserted.
//#
struct LCaretBrush final
{
    LColor   Color   = LColor::White;
    LVector2 Size    = LVector2(1.0f, 0.85f);
    float    HOffset = 3.0f;
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

} /* ~Namespace Jafg */
