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
    LOG_TRACE(
        LogCarnifex,
        "Context [{}] found {} garbage employees. Begin to kill them.",
        this->HumanReadableName, this->Employees.GetSize()
    )

    for (JObjectBase* const& Employee : this->Employees)
    {
        checkSlow( Employee )

        if (Employee->GetVTable())
        {
            Employee->MarkAsGarbage();
            continue;
        }

        /*
         * The default content referrer.
         */
        check( Employee->bGarbage == false )
        Employee->bGarbage = true;
        delete Employee;

        continue;
    }

    this->Employees.Empty();

    return;
}
