// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

/**
 * Provides core useful application statics.
 */
namespace Application
{
    EPlatformExit::Type Create();

    void  SetDeltaTime(const double DeltaTime);
    double GetDeltaTime();
    float GetDeltaTimeAsFloat();
    void UpdateFrameCount();
    uint64 GetFrameCount();
}
