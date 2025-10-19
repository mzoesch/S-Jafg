// Copyright mzoesch. All rights reserved.

#include "Engine/WorldObject.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void Jafg::JWorldObject::BeginLife()
{
    Super::BeginLife();

    check( this->CastedOuter == nullptr )
    for (Private::LWorldTrack const& Track : GEngine->GetTracks())
    {
        check( Track.ChildWorld.get() )
        if (Track.ChildWorld.get() == this->GetOuter())
        {
            this->CastedOuter = Track.ChildWorld.get();
        }

        continue;
    }

    return;
}
