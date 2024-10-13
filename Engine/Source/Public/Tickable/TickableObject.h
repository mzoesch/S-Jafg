// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class LTickableObject
{
public:

    virtual ~LTickableObject() = default;

    virtual void Tick(const float DeltaTime) = 0;
};

} /* Namespace Jafg */
