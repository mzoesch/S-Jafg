// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/World.h"
#include "WorldObject.generated.h"

namespace Jafg
{

class LWorld;

//#
//# An object that usually uses a world as its outer.
//# Additionally, defines utility functions to help with world and engine access.
//# If you want a simpler base class, use JObjectBase.
//#
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API JWorldObject : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldObject)

public:

    virtual void BeginLife() override;

    FORCEINLINE LWorld* GetWorld() noexcept { auto* Out{ this->GetOuter() }; if (Out) { return Out->AsWorld(); } return nullptr; }
    FORCEINLINE LWorld* GetWorldChecked() noexceptcheck { return this->GetOuterChecked()->AsWorld(); }
    FORCEINLINE LWorld* GetWorldAsserted() noexceptcheck { return this->GetOuterAsserted()->AsWorld(); }
    FORCEINLINE LWorld const* GetWorld() const noexcept { auto* Out{ this->GetOuter() }; if (Out) { return Out->AsWorld(); } return nullptr; }
    FORCEINLINE LWorld const* GetWorldChecked() const noexceptcheck { return this->GetOuterChecked()->AsWorld(); }
    FORCEINLINE LWorld const* GetWorldAsserted() const noexceptcheck { return this->GetOuterAsserted()->AsWorld(); }
};

} /* ~Namespace Jafg */
