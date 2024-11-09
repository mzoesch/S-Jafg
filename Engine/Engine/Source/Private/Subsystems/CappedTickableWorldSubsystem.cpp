// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Subsystems/CappedTickableWorldSubsystem.h"

void Jafg::JCappedTickableWorldSubsystem::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    this->LastTickTime += DeltaTime;

    if (this->LastTickTime > this->TickInterval)
    {
        this->FixedTick(DeltaTime, this->LastTickTime);
        this->LastTickTime = 0.0f;
    }

    return;
}
