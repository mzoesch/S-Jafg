// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectClass.h"

bool Jafg::LObjectClass::DerivesFrom(const LObjectClass* InParent) const
{
    const LObjectClass* Current = this;
    while (Current != nullptr)
    {
        if (Current == InParent)
        {
            return true;
        }

        Current = Current->Parent;

        continue;
    }

    return false;
}
