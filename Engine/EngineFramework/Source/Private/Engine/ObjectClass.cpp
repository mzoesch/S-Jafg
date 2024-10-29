// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"

const Jafg::LSimpleString& Jafg::LObjectClass::GetSpacedClassName() const
{
    return Private::GObjectRegistry->GetPanickedPackageByStaticClass(this)->SpacedClassName;
}

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
