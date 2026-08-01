// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "CoreInputSubsystem.generated.h"

namespace Jafg
{

struct LInputActionValue;

//# Adds core input functionality such as debugging keys, return to home, and more.
DECLARE_JAFG_CLASS()
class JCoreInputSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JCoreInputSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
};

} /* ~Namespace Jafg */
