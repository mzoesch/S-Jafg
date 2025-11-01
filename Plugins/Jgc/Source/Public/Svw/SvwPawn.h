// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Pawn.h"
#include "SvwPawn.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class ASvwPawn : public Jafg::APawn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(ASvwPawn)

public:

    virtual void SetOwningController(Jafg::APersonaController* InNew) override;
};

} /* ~Namespace Jgc */
