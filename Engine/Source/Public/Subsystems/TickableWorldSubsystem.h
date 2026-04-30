// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Engine/TickableObject.h"
#include "TickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTickableWorldSubsystem)

    // JWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
    // ~JWorldSubsystem implementation

    // LTickableObject implementation
    virtual void Tick(f32 Dt) override {}
    // ~LTickableObject implementation
};

} /* ~Namespace Jafg */
