// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class RHI_API LViewport final
{
public:

    LViewport() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LViewport)
    ~LViewport() = default;

    void Initialize();
    void TearDown();
};

} /* ~Namespace Jafg. */
