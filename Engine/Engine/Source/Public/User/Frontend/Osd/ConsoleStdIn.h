// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "ConsoleStdIn.generated.h"

namespace Jafg
{

class WEditableTextBox;
class WScrollRegion;
class WVRegion;

DECLARE_JAFG_WIDGET()
class WConsoleStdIn : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleStdIn)

public:

    virtual void Construct() override;

private:

    WEditableTextBox* EditableTextBox;
};

} /* ~Namespace Jafg */
