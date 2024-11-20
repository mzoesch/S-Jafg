// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "ThreadedWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JThreadedWorldSubsystem : public JWorldSubsystem, public LRunn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JThreadedWorldSubsystem)
};

} /* ~Namespace Jafg */
