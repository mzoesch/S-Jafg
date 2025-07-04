// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/TextBoxForward.h"

namespace Jafg
{

class WEditableTextBox;
struct LCaretBrush;
struct LEditableTextBoxBrush;
namespace ETextCommit { enum Type : u8; }

MAKE_DELEGATE_SIGNATURE(LEditableTextBoxAllowCommitDelegate, bool)
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxCommitDelegate, void, const LString& InText, const ETextCommit::Type InType)
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxChangedDelegate, void, const LString& InNewContent)

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which text or graphics will be inserted.
//#
struct LCaretBrush final
{
    //# The caret tint.
    LColor Tint { LColor::White };

    //# The size in percent from the default. Default is (2px x DesiredSize.Y).
    LVector2 Size { 1.0f, 0.85f };

    //# The horizontal offset.
    f32 HOffset { 0.0f };

    //# The blinker speed of the caret in seconds.
    f32 CaretBlinkerSpeed { 0.5f };
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

struct LEditableTextBoxBrush : public LTextBoxBrush
{
    LColor PlaceholderColor { LColor::Gray };

    FORCEINLINE constexpr LEditableTextBoxBrush& PlaceholderColorRet(const LColor& InColor) noexcept { this->PlaceholderColor = InColor; return *this; }
};

} /* ~Namespace Jafg */
