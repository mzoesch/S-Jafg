// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/Subsystem.h"
#include "Tickable/TickableObject.h"
#include "Engine/World.h"

namespace Jafg
{

class JWorldSubsystem : public JSubsystem
{
public:

    JWorldSubsystem(Jafg::LWorld* InOuter) : JSubsystem(InOuter) { }

    // Subsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
    // ~Subsystem implementation
};

class JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
public:

    JTickableWorldSubsystem(Jafg::LWorld* InOuter) : JWorldSubsystem(InOuter) { }

protected:

    virtual void Tick(const float DeltaTime) override { }
};

} /* Namespace Jafg */
