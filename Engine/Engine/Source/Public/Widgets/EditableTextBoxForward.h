// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/TextBoxForward.h"

namespace Jafg
{

class WEditableTextBox;
struct LCaretBrush;
struct LEditableTextBoxBrush;
namespace ETextCommit { enum Type : u8; }

//#
//# Delegate, which is invoked when the user tries to commit the content of the editable text box.
//# @return True, if the user is allowed to commit the content, false otherwise.
//# @note   This is useful if the content cannot be applied due to some validation, and handling the content validation
//#         directly with the #LEditableTextBoxPredicateDelegate is not desired or viable.
//#
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxAllowCommitDelegate, bool)

//#
//# Delegate, which is invoked when the user commits the content of the editable text box.
//#
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxCommitDelegate, void, const LString& InText, const ETextCommit::Type InType)

//#
//# Delegate, which is invoked when the contents of the editable text box changed in any way.
//#
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxChangedDelegate, void, const LString& InNewContent)

//#
//# A delegate that is invoked, if bounded, when the user tries to change the content of the editable text box.
//# @return True, if the content change is allowed, false otherwise. E.g.: you might only want to allow the user to
//#         type in numbers, or you want to validate the content before allowing it. If false is returned, the
//#         the content will not be changed.
//# @note   After this delegate is invoked and the new content was accepted, the #LEditableTextBoxChangedDelegate
//#         will still be invoked. Change events should be handled there. This should only be used for validation.
//#         If this validation fails, the #LEditableTextBoxChangedDelegate will not be invoked.
//#
MAKE_DELEGATE_SIGNATURE(LEditableTextBoxPredicateDelegate, bool, const LString& InNewSuggestedContent)

//#
//# A caret is a blinking line, block, or bitmap in the client area of a window. The caret typically indicates
//# the place at which text or graphics will be inserted.
//#
struct LCaretBrush final
{
    //# The caret tint.
    Lal::LColor Tint { Lal::LColor::White };

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
    Lal::LColor PlaceholderColor { Lal::LColor::Gray };

    FORCEINLINE constexpr LEditableTextBoxBrush& PlaceholderColorRet(const Lal::LColor& InColor) noexcept { this->PlaceholderColor = InColor; return *this; }
};

} /* ~Namespace Jafg */
