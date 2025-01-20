// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Async/TickedRunnable.h"
#include "Async/TaskUtility.h"
#include "ThreadedWorldSubsystem.generated.h"

namespace Jafg
{

//#
//# A world subsystem that shares the same lifetime as the world it resides in but is ticked on a separate thread.
//# The tick of this subsystem is not synchronized with the tick of the engine.
//# If you desire synchronization, use the #JSynchronizedThreadedWorldSubsystem class instead.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JThreadedWorldSubsystem : public JWorldSubsystem, public LTickedRunnable
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JThreadedWorldSubsystem)

public:

    //# Use this method for initialization instead of the normal subsystems one.
    virtual void OnInitialize(LSubsystemCollection& Collection) { }
    virtual void Initialize(LSubsystemCollection& Collection) override final;
    virtual void TearDown() override final;

    //# Called on the master thread, *after* the thread has been terminated.
    virtual void OnTearDown() { }

    // LTickedRunnable implementation
    //# Do not use this here. Initialization is done on the aggregating thread. For initialization use #OnInitialize.
    virtual auto Initialize() -> ETaskExit::Type override final { return LTickedRunnable::Initialize(); }
    //# Override this method to gain access to your tick functionality.
    virtual void FixedTick(const float RunnableDeltaTime) override { }
    //# Use this instead of the normal subsystems #TearDown method. Called on separate thread.
    //# If you need to tear down engine things, use #OnTearDown instead as it is called on the engine thread.
    virtual void Exit() override { LTickedRunnable::Exit(); }
    // ~LTickedRunnable implementation

    FORCEINLINE auto GetThisSubsystemsThreadName(void) const -> ENamedThreads::Type { return this->ThisSubsystemsThreadName; }

private:

    ENamedThreads::Type ThisSubsystemsThreadName = ENamedThreads::Custom;
};

} /* ~Namespace Jafg */
