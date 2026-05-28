// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Subsystems/FrontendSubsystem.h"
#include "JgcInputSubsystem.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JgcInputSubsystem final : public Jafg::JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JgcInputSubsystem)

    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
};

} /* ~Namespace Jgc */
