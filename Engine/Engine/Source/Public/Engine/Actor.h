// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"

namespace Jafg
{

class LWorld;

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public JObject
{
    GENERATED_CLASS_BODY()

    friend LWorld;

public:

    // Temporary constructor until jafg build system is implemented.
    explicit AActor(LWorld* InOuter);

    virtual void BeginLife() override        { JObject::BeginLife(); }
    virtual void Tick(const float DeltaTime) { }
    virtual void EndLife()                   { }
};

} /* ~Namespace Jafg. */
