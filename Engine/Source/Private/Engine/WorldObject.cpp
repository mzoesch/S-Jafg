// Copyright mzoesch. All rights reserved.

#include "Engine/WorldObject.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

#if JAFG_DO_CHECKS
void Jafg::AWorldObject::BeginLife()
{
    Super::BeginLife();

    check(this->GetOuter().IsWorld())
    checkCode
    (
        bool bFound{};
        for (auto const& Track : GEngine->GetTracks())
        {
            check(Track.ChildWorld.get())
            if (Track.ChildWorld.get() == &this->GetOuter())
            {
                check(bFound == false)
                bFound = true;
            }
            continue;
        }
        check(bFound)
    )

    return;
}
#endif /* JAFG_DO_CHECKS */
