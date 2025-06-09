// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Engine/TickableObject.h"
#include "TickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JTickableWorldSubsystem)

    // JWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
    // ~JWorldSubsystem implementation

    // LTickableObject implementation
    virtual void Tick(const f32 DeltaTime) override { }
    // ~LTickableObject implementation
};

} /* ~Namespace Jafg */
