// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Actor.generated.h"

namespace Jafg
{

class LWorld;

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public JObject
{
    GENERATED_CLASS_BODY()

    friend LWorld;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AActor)

public:

    virtual void BeginLife() override        { JObject::BeginLife(); }
    virtual void Tick(const float DeltaTime) { }
    virtual void EndLife()                   { }
};

} /* ~Namespace Jafg. */
