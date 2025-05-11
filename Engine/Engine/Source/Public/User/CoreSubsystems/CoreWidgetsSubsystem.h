// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"

#include "Widgets/UserWidget.h"

#include "CoreWidgetsSubsystem.generated.h"


namespace Jafg
{

class WDebugScreen;
class WCrosshair;
class WConsoleScreen;
class WPauseScreen;

DECLARE_JAFG_CLASS()
class ENGINE_API JCoreWidgetsSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreWidgetsSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    WDebugScreen* DebugScreen = nullptr;
    WCrosshair* Crosshair = nullptr;
    WConsoleScreen* ConsoleScreen = nullptr;
    WPauseScreen* PauseScreen = nullptr;
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WMyTest : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WMyTest)

public:

    virtual void Construct() override;

};

} /* ~Namespace Jafg */
