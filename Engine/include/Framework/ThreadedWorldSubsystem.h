// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldSubsystem.h"
#include "Framework/TickedRunnable.h"
#include "Core/TaskUtility.h"
#include "ThreadedWorldSubsystem.generated.h"

namespace Jafg
{

//#
//# A world subsystem that shares the same lifetime as the world it resides in but is ticked on a separate thread.
//# The tick of this subsystem is not synchronized with the tick of the engine.
//# If you desire synchronization, use the #JSynchronizedThreadedWorldSubsystem class instead.
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class JThreadedWorldSubsystem: public JWorldSubsystem, public LTickedRunnable
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS_BODY(JThreadedWorldSubsystem)
    {
        this->SetPriorityTearDown(true);
        this->SetThreadName(this->GetHumanReadableName());
    }

    virtual void Initialize(LSubsystemCollection& Collection) override final;
    virtual void FixedTick(f32 RunnableDeltaTime) override {}
    virtual void TearDown() override;

public:

    virtual Tasks::EExit::Type Initialize() override { return LTickedRunnable::Initialize(); }
};

} /* ~Namespace Jafg */
