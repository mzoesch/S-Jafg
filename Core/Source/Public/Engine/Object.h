// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine.h"

class JObject
{
protected:

    virtual ~JObject() = default;

public:

    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE         auto GetEngine(void) const -> JEngine* { return GEngine; }
    FORCEINLINE virtual auto GetWorld(void) const -> JWorld* { return this->GetEngine()->GetFirstRunningWorld(); }
};
