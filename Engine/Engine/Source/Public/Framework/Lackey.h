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
class ENGINE_API ALackey final : public APawn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(ALackey)
};

} /* ~Namespace Jafg */
