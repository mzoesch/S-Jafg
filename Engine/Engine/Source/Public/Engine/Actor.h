// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"

namespace Jafg
{

class LWorld;

class ENGINE_API AActor : public JObject
{
    friend LWorld;

public:

    // Temporary constructor until jafg build system is implemented.
    explicit AActor(LWorld* InOuter);

protected:

    virtual void BeginLife()                 { }
    virtual void Tick(const float DeltaTime) { }
    virtual void EndLife()                   { }
};

} /* ~Namespace Jafg. */
