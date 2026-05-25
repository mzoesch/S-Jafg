// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Subsystems/Subsystem.h"
#include "FrontendSubsystem.generated.h"

namespace Jafg
{

//#
//# A subsystem that shares its lifetime with the local ego frontend.
//# Important delegates will be broadcasted to all children of this subsystem.
//# Widget construction and destruction should be done here.
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class JFrontendSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY(ENGINE_API)

    friend LFrontendBase;
    friend LFrontend;

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JFrontendSubsystem)

    virtual void Tick() {}

    FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(const bool bShouldTick) noexcept { this->bShouldTick = bShouldTick; }

    ENGINE_API LFrontend& GetFrontend() noexcept;
    ENGINE_API LFrontend const& GetFrontend() const noexcept;

private:

    bool bShouldTick{};
};

} /* ~Namespace Jafg */
