// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Actor.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public JObject
{
    GENERATED_CLASS_BODY()

    friend LWorld;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AActor)

public:

    virtual void BeginLife() override        { Super::BeginLife(); }
    virtual void Tick(const float DeltaTime) { }
    virtual void EndLife()                   { Super::EndLife(); }
};

} /* ~Namespace Jafg. */

/*
 * Keep this include as every action with any actor will need a ton of function from this include.
 */
#include "ActorUtility.h"
