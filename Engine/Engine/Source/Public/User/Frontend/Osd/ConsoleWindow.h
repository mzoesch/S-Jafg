// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/FloatingWindow.h"
#include "ConsoleWindow.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class WConsoleWindow final : public WFloatingWindow
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleWindow)

    static TOptional<LVector2> LastWindowLocation;
    static TOptional<LVector2> LastWindowSize;

    virtual void Construct() override;
    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override;
};

} /* ~Namespace Jafg */
