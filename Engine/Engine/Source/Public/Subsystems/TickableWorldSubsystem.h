// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "WorldSubsystem.h"
#include "Tickable/TickableObject.h"

#include "TickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JTickableWorldSubsystem : public JWorldSubsystem, public LTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JTickableWorldSubsystem)

    virtual void Tick(const float DeltaTime) override { }
};

} /* ~Namespace Jafg */
