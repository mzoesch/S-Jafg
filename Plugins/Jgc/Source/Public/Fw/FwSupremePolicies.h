// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Framework/SupremePolicies.h"
#include "FwSupremePolicies.generated.h"

namespace Jafg
{
class AActor;

} /* ~Namespace Jafg */

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JGC_API AFwSupremePolicies : public Jafg::ASupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AFwSupremePolicies)

public:

    virtual void OnWorldPreInit() override;
    virtual void OnPersonaControllerCreated(Jafg::APersonaController& Pc) override;
    virtual Jafg::TJxxUnique<Jafg::APawn> GetPawnForPersonaController(Jafg::APersonaController const& Pc) override;
};

} /* ~Namespace Jgc */
