// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WEditableTextBlock;

DECLARE_JAFG_CLASS()
class ENGINE_API WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void Construct() override;

    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

private:

    WEditableTextBlock* EditableTextBlock = nullptr;
};

} /* ~Namespace Jafg */
