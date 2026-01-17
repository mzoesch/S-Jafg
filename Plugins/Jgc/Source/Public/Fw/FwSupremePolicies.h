// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SupremePolicies.h"
#include "FwSupremePolicies.generated.h"

namespace Jafg
{
class AActor;

} /* ~Namespace Jafg */

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JGC_API JFwSupremePolicies : public Jafg::JSupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JFwSupremePolicies)

public:

    virtual void OnWorldPreInit() override;
    virtual Jafg::APawn* SpawnDeferredPawnForPersonaController(Jafg::APersonaController& Pc) override;

    Jafg::AActor* RotatingActor{};
};

} /* ~Namespace Jgc */
