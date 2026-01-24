// Copyright mzoesch. All rights reserved.

#include "Subsystems/FixedTickableWorldSubsystem.h"

void Jafg::JFixedTickableWorldSubsystem::Tick(const float DeltaTime)
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
