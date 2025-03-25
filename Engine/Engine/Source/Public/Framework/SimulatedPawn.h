// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Pawn.h"
#include "SimulatedPawn.generated.h"

namespace Jafg
{

//#
//# A pawn that allows for moving around inside a world that is currently being simulated.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API ASimulatedPawn final : public APawn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(ASimulatedPawn)
};

} /* ~Namespace Jafg */
