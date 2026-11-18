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

    static void BeginClassLife(Jafg::LBeginClassLifeInfo const& Info);
    static void EndClassLife(Jafg::LEndClassLifeInfo const& Info);

    virtual void OnWorldPostInit() override;

    virtual void OnPersonaControllerCreated(Jafg::APersonaController& Pc) override;
    virtual TJxxUnique<Jafg::AActor> SpawnAvatarForPersonaController(Jafg::APersonaController const& Pc) override;

    virtual LVec3F GetPreferredGravity() const noexcept override
    {
        return maths::zero_vector<LVec3F>;
    }

    //# Space: There is no air.
    virtual f32 GetPreferredAirDensity() const noexcept override
    {
        return 0.0f;
    }
};

} /* ~Namespace Jgc */
