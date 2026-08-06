// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Framework/Pawn.h"
#include "PawnComponent.generated.h"

namespace Jafg
{

class APawn;
class APersonaController;

DECLARE_JAFG_CLASS()
class ENGINE_API APawnComponent : public AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(APawnComponent)

public:

#if JAFG_DO_CHECKS
    virtual void OnAttach(AActor& InOwner) override;
#endif /* JAFG_DO_CHECKS */

    FORCEINLINE constexpr bool IsOwningPawnValid() const noexcept { return this->IsOwningActorValid(); }
    FORCEINLINE CONSTEXPR_CHECK APawn& GetOwningPawn() noexcept { return *StaticCast<APawn>(&this->GetOwningActor()); }
    FORCEINLINE CONSTEXPR_CHECK APawn const& GetOwningPawn() const noexcept { return *StaticCast<APawn>(&this->GetOwningActor()); }

    //#
    //# Called when the PC changed from the pawn owning this component.
    //# @note If #New is null, then the pawn is not possessed.
    //#
    virtual void OnNewPersonaController(APersonaController* New) {}
};

} /* ~Namespace Jafg */
