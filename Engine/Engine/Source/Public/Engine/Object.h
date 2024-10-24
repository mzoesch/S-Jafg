// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectBase.h"
#include "Engine.h"

namespace Jafg
{

class LWorld;

class ENGINE_API JObject : public ::Jafg::Private::JObjectBase
{
protected:

    explicit JObject(LWorld* InOuter);

public:

    FORCEINLINE virtual auto GetWorld() const -> LWorld* { return this->CastedOuter; }

    virtual void BeginLife() override;

private:

    LWorld* CastedOuter = nullptr;
};

} /* ~Namespace Jafg */
