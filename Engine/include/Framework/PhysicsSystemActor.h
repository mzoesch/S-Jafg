// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "PhysicsSystemActor.generated.h"

namespace Jafg
{

//#
//# An actor that serves as a physics subsystem of a physics system owned by a world.
//# All subcomponents or attached actors leave their physics system and enter this physics simulation system.
//#
//# APhysicsSystems support recursive attachemt states.
//#
//# TODO: Implement
//#
DECLARE_JAFG_CLASS()
class ENGINE_API APhysicsSystem : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(APhysicsSystem)
};

} /* ~Namespace Jafg */
