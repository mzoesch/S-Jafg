// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Widgets/UserWidget.h"
#include "CoreWorldWidgetsSubsystem.generated.h"

namespace Jafg
{

class WDebugScreen;
class WDebugMenu;
class WCrosshair;
class WConsoleScreen;
class WPauseScreen;

DECLARE_JAFG_CLASS()
class JCoreWorldWidgetsSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreWorldWidgetsSubsystem)

    virtual bool ShouldCreateSubsystem(LClassOuter const* Outer) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    FORCEINLINE bool IsDebugScreenValid() const { return this->DebugScreen != nullptr; }
    FORCEINLINE bool IsCrosshairValid() const { return this->Crosshair != nullptr; }
    FORCEINLINE bool IsConsoleScreenValid() const { return this->ConsoleScreen != nullptr; }
    FORCEINLINE bool IsPauseScreenValid() const { return this->PauseScreen != nullptr; }

    FORCEINLINE WDebugScreen* GetDebugScreen() { return this->DebugScreen; }
    FORCEINLINE const WDebugScreen* GetDebugScreen() const { return this->DebugScreen; }

    FORCEINLINE WDebugMenu* GetDebugMenu() { return this->DebugMenu; }
    FORCEINLINE const WDebugMenu* GetDebugMenu() const { return this->DebugMenu; }

    FORCEINLINE const WCrosshair* GetCrosshair() const { return this->Crosshair; }
    FORCEINLINE WCrosshair* GetCrosshair() { return this->Crosshair; }

    FORCEINLINE WConsoleScreen* GetConsoleScreen() { return this->ConsoleScreen; }
    FORCEINLINE const WConsoleScreen* GetConsoleScreen() const { return this->ConsoleScreen; }

    FORCEINLINE WPauseScreen* GetPauseScreen() { return this->PauseScreen; }
    FORCEINLINE const WPauseScreen* GetPauseScreen() const { return this->PauseScreen; }

private:

    CDR_NULL_PTR(WDebugScreen*) DebugScreen{ nullptr };
    CDR_NULL_PTR(WDebugMenu*) DebugMenu{ nullptr };
    CDR_NULL_PTR(WCrosshair*) Crosshair{ nullptr };
    CDR_NULL_PTR(WConsoleScreen*) ConsoleScreen{ nullptr };
    CDR_NULL_PTR(WPauseScreen*) PauseScreen{ nullptr };
};

} /* ~Namespace Jafg */
