// Copyright mzoesch. All rights reserved.

#include "Engine/ObjectContext.h"
#include "Engine/ObjectBase.h"

Jafg::LObjectContext::LObjectContext(EGlobalCarnifex)
{
    check( Private::GCarnifexReferrer )
    check( this->IsValid() == false )
    this->DeferredInitialize(Private::GCarnifexReferrer);
    check( this->IsValid() )

    return;
}

Jafg::LObjectContext& Jafg::LObjectContext::operator=(EGlobalCarnifex)
{
    check( Private::GCarnifexReferrer )
    check( this->IsValid() == false )
    this->DeferredInitialize(Private::GCarnifexReferrer);
    check( this->IsValid() )

    return *this;
}

void Jafg::LObjectContext::TearDownContext()
{
    check( this->Carnifex )

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

    this->Carnifex = nullptr;

    return;
}
