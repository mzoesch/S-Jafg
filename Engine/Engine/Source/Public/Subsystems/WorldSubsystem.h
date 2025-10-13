// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "WorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class JWorldSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JWorldSubsystem)

public:

    ENGINE_API static bool IsOuterFrontend(LClassOuter const* Outer) noexcept;
    ENGINE_API static bool IsOuterWorld(LClassOuter const* Outer) noexcept;

    FORCEINLINE LWorld* GetWorld() { check( this->GetOuter()->IsWorld() )  return reinterpret_cast<LWorld*>(this->GetOuter()); }
    FORCEINLINE const LWorld* GetWorld() const { check( this->GetOuter()->IsWorld() ) return reinterpret_cast<LWorld const*>(this->GetOuter()); }
};

} /* ~Namespace Jafg */
