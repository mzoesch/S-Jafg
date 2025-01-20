// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "CoreInputSubsystem.generated.h"

namespace Jafg
{

struct LInputActionValue;

//#
//# Adds core input functionality such as debugging keys, return to home, and more.
//# These keys are not intended to be overridden by other subsystems.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API JCoreInputSubsystem final : public JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreInputSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

    virtual void OnNewPawnPossessed(APawn* InOld, APawn* InNew) override;

private:

    void OnDebugScreenToggle(LInputActionValue& InValue);
};

} /* ~Namespace Jafg */
