// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "HudSubsystem.generated.h"

namespace Jafg
{

class LHud;

//#
//# A subsystem that shares its lifetime with the local ego hud.
//# Important delegates will be broadcasted to all children of this subsystem.
//# Widget construction and destruction should be done here.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JHudSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

    friend LHud;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JHudSubsystem)

    // JSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    // ~JSubsystem implementation

    virtual void Tick(const float DeltaTime) { }

    FORCEINLINE bool ShouldTick() const { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(const bool bShouldTick) { this->bShouldTick = bShouldTick; }

    FORCEINLINE       LHud* GetHud() { return this->OwningHud; }
    FORCEINLINE const LHud* GetHud() const { return this->OwningHud; }

private:

    bool  bShouldTick = false;
    LHud* OwningHud   = nullptr;
};

} /* ~Namespace Jafg. */
