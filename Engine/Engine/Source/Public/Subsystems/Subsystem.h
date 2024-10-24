// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Subsystems/SubsystemCollection.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JSubsystem : public JObject
{
    GENERATED_CLASS_BODY()

public:

    JSubsystem(Jafg::LWorld* InOuter) : JObject(InOuter) { }

    virtual void Initialize(LSubsystemCollection& Collection) { }
    virtual void TearDown() { }
};

} /* Namespace Jafg */
