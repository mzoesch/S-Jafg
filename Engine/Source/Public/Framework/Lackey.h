// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Pawn.h"
#include "Lackey.generated.h"

namespace Jafg
{

//#
//# A lackey is a pawn with a physical representation and volume in the world.
//#
DECLARE_JAFG_CLASS()
class ALackey final : public APawn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ALackey)
};

} /* ~Namespace Jafg */
