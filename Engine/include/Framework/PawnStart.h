// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "PawnStart.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API APawnStart : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_EXTERNAL_BODY(APawnStart)

public:

    virtual bool CanAcceptPawn(APawn const& Pawn) const
    {
        //# TODO: To some collision checking as default maybe?
        return true;
    }

    TArray<LString> Tags;
};

} /* ~Namespace Jafg */
