// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "CoreCommandSubsystem.generated.h"
#include "Cli/CommandLineInterface.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JCoreCommandSubsystem final : public JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreCommandSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    LCliCommandHandle CommandHandle_Say;
    LCliCommandHandle CommandHandle_CreateNewSurface;
};

} /* ~Namespace Jafg */
