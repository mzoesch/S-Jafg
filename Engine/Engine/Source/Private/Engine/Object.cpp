// Copyright mzoesch. All rights reserved.

#include "Engine/WorldObject.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

Jafg::LWorld* Jafg::JWorldObject::GetOrCalculateCastedOuter()
{
    if (this->CastedOuter)
    {
        return this->CastedOuter;
    }

    for (Private::LWorldTrack const& Track : GEngine->GetTracks())
    {
        check( Track.ChildWorld.get() )
        if (Track.ChildWorld.get() == this->GetOuter())
        {
            this->CastedOuter = Track.ChildWorld.get();
            return this->CastedOuter;
        }

        continue;
    }

    return nullptr;
}

void Jafg::JWorldObject::BeginLife()
{
    JCxxClass::BeginLife();
    (void)this->GetOrCalculateCastedOuter();
    return;
}
