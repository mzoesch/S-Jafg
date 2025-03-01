// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "FrontendSubsystem.generated.h"

namespace Jafg
{

class LFrontend;

//#
//# A subsystem that shares its lifetime with the local ego frontend.
//# Important delegates will be broadcasted to all children of this subsystem.
//# Widget construction and destruction should be done here.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JFrontendSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

    friend LFrontend;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JFrontendSubsystem)

    // JSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    // ~JSubsystem implementation

    virtual void Tick(const float DeltaTime) { }

    FORCEINLINE bool ShouldTick() const { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(const bool bShouldTick) { this->bShouldTick = bShouldTick; }

    //# Always valid.
    FORCEINLINE       LFrontend* GetFrontend() { return this->OwningFrontend; }
    FORCEINLINE const LFrontend* GetFrontend() const { return this->OwningFrontend; }

private:

    bool bShouldTick = false;
    LFrontend* OwningFrontend = nullptr;
};

} /* ~Namespace Jafg. */
