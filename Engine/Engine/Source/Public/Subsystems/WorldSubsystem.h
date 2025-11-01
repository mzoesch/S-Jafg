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

    static bool IsOuterFrontend(LClassOuter const* Outer) noexcept;
    static bool IsOuterWorld(LClassOuter const* Outer) noexcept;

    FORCEINLINE LWorld* GetWorld() noexcept { auto* Out{ this->GetOuter() }; if (Out) { return Out->AsWorld(); } return nullptr; }
    FORCEINLINE LWorld* GetWorldChecked() noexceptcheck { return this->GetOuterChecked()->AsWorld(); }
    FORCEINLINE LWorld* GetWorldAsserted() noexceptcheck { return this->GetOuterAsserted()->AsWorld(); }
    FORCEINLINE LWorld const* GetWorld() const noexcept { auto* Out{ this->GetOuter() }; if (Out) { return Out->AsWorld(); } return nullptr; }
    FORCEINLINE LWorld const* GetWorldChecked() const noexceptcheck { return this->GetOuterChecked()->AsWorld(); }
    FORCEINLINE LWorld const* GetWorldAsserted() const noexceptcheck { return this->GetOuterAsserted()->AsWorld(); }
};

} /* ~Namespace Jafg */
