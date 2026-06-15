// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Framework/PersonaController.h"
#include "PersonaControllerComponent.generated.h"

namespace Jafg
{

class APawn;
class APersonaController;

DECLARE_JAFG_CLASS()
class ENGINE_API APersonaControllerComponent : public AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(APersonaControllerComponent)

public:

#if JAFG_DO_CHECKS
    virtual void OnAttach(AActor& InOwner) override;
#endif /* JAFG_DO_CHECKS */

    FORCEINLINE constexpr bool IsOwningPersonaControllerValid() const noexcept { return this->IsOwningActorValid(); }
    FORCEINLINE constexpr APersonaController& GetOwningPersonaController() const noexcept { return *StaticCast<APersonaController>(&this->GetOwningActor()); }
};

} /* ~Namespace Jafg */
