// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Engine.h"
#include "Object.generated.h"

namespace Jafg
{

class LWorld;

/**
 * An object that usually uses a world as its outer.
 */
DECLARE_JAFG_CLASS()
class ENGINE_API JObject : public ::Jafg::Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JObject)

public:

    FORCEINLINE virtual auto GetWorld() const -> LWorld* { return this->CastedOuter; }

    virtual void BeginLife() override;

private:

    LWorld* CastedOuter = nullptr;
};

} /* ~Namespace Jafg */
