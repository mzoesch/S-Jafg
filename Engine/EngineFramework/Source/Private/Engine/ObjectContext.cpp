// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectBase.h"

Jafg::Private::LObjectContext::LObjectContext()
{
    this->Carnifex = GCarnifexReferrer;
    return;
}

void Jafg::Private::LObjectContext::TearDownContext()
{
    for (JObjectBase* const& Employee : this->Employees)
    {
        checkSlow( Employee )
        Employee->MarkAsGarbage();
        continue;
    }

    this->Employees.Empty();

    return;
}
