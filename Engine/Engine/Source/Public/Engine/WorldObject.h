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
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldObject)

public:

    FORCEINLINE auto IsWorldValid() const -> bool { return this->CastedOuter != nullptr; }
                LWorld* GetOrCalculateCastedOuter();
    FORCEINLINE LWorld* GetOrCalculateCastedOuterChecked() { LWorld* Out = this->GetOrCalculateCastedOuter(); check( Out) return Out; }
    FORCEINLINE LWorld* GetOrCalculateCastedOuterAsserted() { LWorld* Out = this->GetOrCalculateCastedOuter(); jassert( Out ) return Out; }
    FORCEINLINE auto GetWorld() const -> LWorld* { return this->CastedOuter; }
    FORCEINLINE auto GetWorldChecked() const -> LWorld* { check( this->GetWorld() ) return this->GetWorld(); }
    FORCEINLINE auto GetWorldAsserted() const -> LWorld* { jassert( this->GetWorld() ) return this->GetWorld(); }

    virtual void BeginLife() override;

private:

    LWorld* CastedOuter { nullptr };
};

} /* ~Namespace Jafg */
