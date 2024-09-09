// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/World.h"
#include "Engine/Framework/Camera.h"

void JWorld::Init()
{
    check( this->Actors.empty() )

    this->Actors.emplace_back( new ACamera() );

    return;
}

void JWorld::TearDown()
{
    for (const AActor* Actor : this->Actors)
    {
        delete Actor;
    }

    this->Actors.clear();

    return;
}
