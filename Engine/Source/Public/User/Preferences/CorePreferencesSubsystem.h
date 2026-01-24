// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "CorePreferencesSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JCorePreferencesSubsystem final : public JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCorePreferencesSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
};

} /* ~Namespace Jafg */
