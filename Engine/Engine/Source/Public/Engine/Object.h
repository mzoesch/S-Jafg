// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine.h"

namespace Jafg
{

class ENGINE_API JObject
{
protected:

    JObject() = default;
    JObject(LWorld* Outer) : Outer(Outer) { }
    PROHIBIT_REALLOC_OF_ANY_FROM(JObject)
    virtual ~JObject() = default;

public:

    FORCEINLINE virtual auto HasContext() const -> bool { return this->Outer != nullptr; }
    FORCEINLINE virtual auto GetWorld() const -> LWorld* { return this->Outer; }

protected:

    LWorld* Outer = nullptr;
};

JObject& NewJObject();
JObject& NewJObject(LWorld* Outer);

} /* ~Namespace Jafg */
