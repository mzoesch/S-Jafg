// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "WorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldSubsystem)

public:

    ENGINE_API static bool IsOuterFrontend(const LObjectContext* InOuter) noexcept;
    ENGINE_API static bool IsOuterWorld(const LObjectContext* InOuter) noexcept;

    FORCEINLINE LWorld* GetWorld() { check( this->GetOuter()->IsWorld() )  return reinterpret_cast<LWorld*>(this->GetOuter()); }
    FORCEINLINE const LWorld* GetWorld() const { check( this->GetOuter()->IsWorld() ) return reinterpret_cast<LWorld*>(this->GetOuter()); }
};

} /* ~Namespace Jafg */
