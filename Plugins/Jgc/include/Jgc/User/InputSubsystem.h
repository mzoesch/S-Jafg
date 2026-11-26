// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Framework/FrontendSubsystem.h"
#include "InputSubsystem.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JInputSubsystem final : public Jafg::JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JInputSubsystem)

    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
};

} /* ~Namespace Jgc */
