// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "EngineSubsystem.generated.h"

namespace Jafg
{

//#
//# A subsystem that shares its lifetime with the engine.
//# @see Engine/Engine.h
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JEngineSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JEngineSubsystem)
};

} /* ~Namespace Jafg */
