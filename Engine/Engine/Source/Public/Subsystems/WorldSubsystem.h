// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/Subsystem.h"
#include "Tickable/TickableObject.h"

namespace Jafg
{

class JWorldSubsystem : public JSubsystem
{
public:

    JWorldSubsystem() = default;

    // Subsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
    // ~Subsystem implementation
};

class JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
    virtual void Tick(const float DeltaTime) override { }
};

} /* Namespace Jafg */
