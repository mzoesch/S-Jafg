// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "WorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
class ENGINE_API JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldSubsystem)

public:

    FORCEINLINE auto GetWorld() const -> LWorld* { return reinterpret_cast<LWorld*>(this->GetOuter()); }
};

} /* ~Namespace Jafg */
