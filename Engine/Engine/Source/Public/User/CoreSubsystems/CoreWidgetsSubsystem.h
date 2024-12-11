// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/HudSubsystem.h"
#include "CoreWidgetsSubsystem.generated.h"

namespace Jafg
{

class WDebugScreen;
class WCrosshair;
class WChatScreen;
class WPauseMenu;

DECLARE_JAFG_CLASS()
class ENGINE_API JCoreWidgetsSubsystem final : public JHudSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreWidgetsSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    WDebugScreen* DebugScreen = nullptr;
    WCrosshair* Crosshair = nullptr;
    WChatScreen* ChatScreen = nullptr;
    WPauseMenu* PauseMenu = nullptr;
};

} /* ~Namespace Jafg */
