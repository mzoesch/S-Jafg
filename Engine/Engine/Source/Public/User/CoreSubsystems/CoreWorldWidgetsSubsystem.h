// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Widgets/UserWidget.h"
#include "CoreWorldWidgetsSubsystem.generated.h"

namespace Jafg
{

class WDebugScreen;
class WCrosshair;
class WConsoleScreen;
class WPauseScreen;

DECLARE_JAFG_CLASS()
class ENGINE_API JCoreWorldWidgetsSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreWorldWidgetsSubsystem)

    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

public:

    FORCEINLINE bool IsDebugScreenValid() const { return this->DebugScreen != nullptr; }
    FORCEINLINE bool IsCrosshairValid() const { return this->Crosshair != nullptr; }
    FORCEINLINE bool IsConsoleScreenValid() const { return this->ConsoleScreen != nullptr; }
    FORCEINLINE bool IsPauseScreenValid() const { return this->PauseScreen != nullptr; }

    FORCEINLINE WDebugScreen* GetDebugScreen() { return this->DebugScreen; }
    FORCEINLINE const WDebugScreen* GetDebugScreen() const { return this->DebugScreen; }

    FORCEINLINE const WCrosshair* GetCrosshair() const { return this->Crosshair; }
    FORCEINLINE WCrosshair* GetCrosshair() { return this->Crosshair; }

    FORCEINLINE WConsoleScreen* GetConsoleScreen() { return this->ConsoleScreen; }
    FORCEINLINE const WConsoleScreen* GetConsoleScreen() const { return this->ConsoleScreen; }

    FORCEINLINE WPauseScreen* GetPauseScreen() { return this->PauseScreen; }
    FORCEINLINE const WPauseScreen* GetPauseScreen() const { return this->PauseScreen; }

private:

    WDebugScreen* DebugScreen { nullptr };
    WCrosshair* Crosshair { nullptr };
    WConsoleScreen* ConsoleScreen { nullptr };
    WPauseScreen* PauseScreen { nullptr };
};

} /* ~Namespace Jafg */
