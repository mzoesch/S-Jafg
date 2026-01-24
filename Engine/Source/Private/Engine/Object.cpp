// Copyright mzoesch. All rights reserved.

#include "Engine/WorldObject.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void Jafg::JWorldObject::BeginLife()
{
    Super::BeginLife();

    check( this->GetOuter()->IsWorld() )
    checkCode
    (
        bool bFound{ false };

        for (auto const& Track : GEngine->GetTracks())
        {
            check( Track.ChildWorld.get() )
            if (Track.ChildWorld.get() == this->GetOuter())
            {
                check( bFound == false )
                bFound = true;
            }

            continue;
        }

        check( bFound )
    )

    return;
}
