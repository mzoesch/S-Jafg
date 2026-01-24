// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "Cli/CliHandles.h"
#include "CoreCommandSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JCoreCommandSubsystem final : public JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreCommandSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown(LClassOuter& PreviousOuter) override;

private:

    LCliCommandHandle CommandHandle_Quit;
    LCliCommandHandle CommandHandle_Say;
    LCliCommandHandle CommandHandle_CreateNewSurface;
};

} /* ~Namespace Jafg */
