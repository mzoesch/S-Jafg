// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectBase.h"

Jafg::LObjectContext::LObjectContext()
{
    this->Carnifex = Private::GCarnifexReferrer;
    return;
}

void Jafg::LObjectContext::TearDownContext()
{
    LOG_TRACE
    (
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
        Employee->OnDefaultGarbage();
        delete Employee;

        continue;
    }

    this->Employees.Empty();

    return;
}
