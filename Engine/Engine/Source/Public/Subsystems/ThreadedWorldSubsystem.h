// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Async/TickedRunnable.h"
#include "ThreadedWorldSubsystem.generated.h"

namespace Jafg
{

/**
 * A world subsystem that shares the same lifetime as the world it resides in but is ticked on a separate thread.
 * The tick of this subsystem is not synchronized with the tick of the engine.
 * If you desire synchronization, use the #JSynchronizedThreadedWorldSubsystem class instead.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JThreadedWorldSubsystem : public JWorldSubsystem, public LTickedRunnable
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JThreadedWorldSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float RunnableDeltaTime) override { }
};

} /* ~Namespace Jafg */
