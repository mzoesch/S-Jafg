// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "Cli/CommandLineInterface.h"
#include "CoreCommandSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JCoreCommandSubsystem final : public JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JCoreCommandSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    LCliCommandHandle CommandHandle_Quit;
    LCliCommandHandle CommandHandle_Say;
};

} /* ~Namespace Jafg */
