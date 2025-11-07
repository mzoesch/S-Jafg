// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/LocalEgoSubsystem.h"
#include "JgcInputSubsystem.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JgcInputSubsystem final : public Jafg::JLocalEgoSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JgcInputSubsystem)

    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
};

} /* ~Namespace Jgc */
