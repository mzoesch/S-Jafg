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
class JWorldObject : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldObject)

public:

    virtual void BeginLife() override;

    FORCEINLINE auto IsWorldValid() const -> bool { return this->CastedOuter != nullptr; }

    FORCEINLINE LWorld* GetWorld() noexcept { return this->CastedOuter; }
    FORCEINLINE LWorld* GetWorldChecked() noexceptcheck { check( this->GetWorld() ) return this->GetWorld(); }
    FORCEINLINE LWorld* GetWorldAsserted() noexceptcheck { jassert( this->GetWorld() ) return this->GetWorld(); }
    FORCEINLINE LWorld const* GetWorld() const noexcept { return this->CastedOuter; }
    FORCEINLINE LWorld const* GetWorldChecked() const noexceptcheck { check( this->GetWorld() ) return this->GetWorld(); }
    FORCEINLINE LWorld const* GetWorldAsserted() const noexceptcheck { jassert( this->GetWorld() ) return this->GetWorld(); }

private:

    LWorld* CastedOuter{ nullptr };
};

} /* ~Namespace Jafg */
