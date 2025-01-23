// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBlockForward.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WEditableTextBlock;

namespace EConsoleScreenState
{

enum Type : uint8
{
    Show,
    Hide,
};

} /* ~Namespace EConsoleScreenState */

DECLARE_JAFG_WIDGET()
class ENGINE_API WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void Construct() override;

    void SetConsoleFrontendState(const EConsoleScreenState::Type InState);
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

private:

    void HideConsoleScreenWithSideEffects();
    void ShowConsoleScreenWithSideEffects();

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    WEditableTextBlock* EditableTextBlock = nullptr;
};

} /* ~Namespace Jafg */
