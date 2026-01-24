// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "WorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldSubsystem)

public:

    FORCEINLINE LWorld* GetWorld() noexcept { check( this->GetOuter() ) return this->GetOuter()->AsWorld(); }
    FORCEINLINE LWorld const* GetWorld() const noexcept { check( this->GetOuter() ) return this->GetOuter()->AsWorld(); }
};

} /* ~Namespace Jafg */
