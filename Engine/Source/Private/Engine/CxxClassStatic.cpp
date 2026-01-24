// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClass.h"

bool Jafg::LCxxClass::DerivesFrom(LCxxClass const& Parent) const noexcept
{
    LCxxClass const* Cursor { this };
    while (Cursor != nullptr)
    {
        if (Cursor == &Parent)
        {
            return true;
        }

        Cursor = Cursor->Parent;
        continue;
    }

    return false;
}
