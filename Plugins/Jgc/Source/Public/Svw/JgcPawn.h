// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Pawn.h"
#include "JgcPawn.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class AJgcPawn : public Jafg::APawn
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AJgcPawn)

public:

    virtual void BeginLifeCDR() override;

    virtual void SetOwningController(Jafg::APersonaController* InNew) override;

    static void OnOngoingNaiveMove(Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value);
};

} /* ~Namespace Jgc */
