// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Subsystems/SubsystemCollection.h"

namespace Jafg
{

class JSubsystem : public JObject
{
public:

    JSubsystem() = default;
    JSubsystem(Jafg::LWorld* InOuter) : JObject(InOuter) { }

    virtual void Initialize(LSubsystemCollection& Collection) { }
    virtual void TearDown() { }
};

} /* Namespace Jafg */
