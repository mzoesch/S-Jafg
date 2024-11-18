// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "World.h"
#include "Object.generated.h"

namespace Jafg
{

class LWorld;
class LApplicationInstance;

/**
 * An object that usually uses a world as its outer.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JObject : public ::Jafg::Private::JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JObject)

public:

    auto GetEngine() const -> LEngine*;
    auto GetApplicationInstanceSubsystem() const -> LApplicationInstance*;

    FORCEINLINE auto GetWorld() const -> LWorld* { return this->CastedOuter; }

    virtual void BeginLife() override;

private:

    LWorld* CastedOuter = nullptr;
};

} /* ~Namespace Jafg */
