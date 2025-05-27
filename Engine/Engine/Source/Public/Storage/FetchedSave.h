// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/Path.h"

namespace Jafg
{

struct LFetchedSave
{
    LPath Path;
    LString DisplayName;

    FORCEINLINE bool IsValid() const noexcept { return this->Path.IsEmpty() == false; }
};

} /* ~Namespace Jafg */
