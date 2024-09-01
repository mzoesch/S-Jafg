// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

/**
 * Provides core useful application statics.
 */
namespace Application
{
    EPlatformExit::Type Create();

    float GetDeltaTime();
}
