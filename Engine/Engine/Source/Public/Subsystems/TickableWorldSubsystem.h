// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "WorldSubsystem.h"
#include "Tickable/TickableObject.h"
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
    virtual void Tick(const float DeltaTime) override { }
    // ~LTickableObject implementation
};

} /* ~Namespace Jafg */
