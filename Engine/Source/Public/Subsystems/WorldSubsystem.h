// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "WorldSubsystem.generated.h"

namespace Jafg
{

//# A subsystem that are spawned in a LWorld.
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JWorldSubsystem)

public:

    FORCEINLINE LWorld& GetWorld() noexcept { return this->GetOuter().AsWorld(); }
    FORCEINLINE LWorld const& GetWorld() const noexcept { return this->GetOuter().AsWorld(); }
};

} /* ~Namespace Jafg */
