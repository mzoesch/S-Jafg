// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/Subsystem.h"
#include "Tickable/TickableObject.h"
#include "Engine/World.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

public:

    JWorldSubsystem(Jafg::LWorld* InOuter) : JSubsystem(InOuter) { }

    // Subsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
    // ~Subsystem implementation
};

DECLARE_JAFG_CLASS()
class JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
    GENERATED_CLASS_BODY()

public:

    JTickableWorldSubsystem(Jafg::LWorld* InOuter) : JWorldSubsystem(InOuter) { }

protected:

    virtual void Tick(const float DeltaTime) override { }
};

} /* Namespace Jafg */
