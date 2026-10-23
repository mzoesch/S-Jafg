// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Framework/SupremePolicies.h"
#include "OwssSupremePolicies.generated.h"

namespace Jgc
{

//#
//# A owss supreme policies class is always created for owss.
//#
DECLARE_JAFG_CLASS()
class JGC_API AOwssSupremePolicies : public Jafg::ASupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AOwssSupremePolicies)

public:

    virtual void OnWorldPostInit() override;

    virtual void OnPersonaControllerCreated(Jafg::APersonaController& Pc) override;
    virtual TJxxUnique<Jafg::APawn> GetPawnForPersonaController(Jafg::APersonaController const& Pc) override;

    virtual LVec3F GetPreferredGravity() const noexcept override
    {
        return maths::zero_vector<LVec3F>;
    }
};

} /* ~Namespace Jgc */
