// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Subsystems/SubsystemCollection.h"

class JSubsystem : public JObject
{
public:

    JSubsystem() = default;

    virtual void Initialize(LSubsystemCollection& Collection) { }
    virtual void TearDown() { }
};
