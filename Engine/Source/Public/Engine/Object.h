// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine.h"

namespace Jafg
{

class ENGINE_API JObject
{
protected:

    virtual ~JObject() = default;

public:

    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE         auto GetEngine(void) const -> LEngine* { return GEngine; }
    FORCEINLINE virtual auto GetWorld(void) const -> JWorld* { return this->GetEngine()->GetFirstRunningWorld(); }
};

} /* Namespace Jafg */
