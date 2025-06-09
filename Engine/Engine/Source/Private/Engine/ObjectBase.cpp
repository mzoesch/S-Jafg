// Copyright mzoesch. All rights reserved.

#include "Engine/ObjectBase.h"
#include "Engine/Carnifex.h"
#include "Engine/Engine.h"

Jafg::LObjectInitializer Jafg::GetDefaultObjectInitializer()
{
    if (GEngine)
    {
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
        return LObjectInitializer{GEngine->GetCurrentForeignContext()};
#else /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
        jassertNoEntry()
        abort()
#endif /* !PLATFORM_SUPPORTS_SHARED_LIBRARIES */
    }

    return LObjectInitializer{GOmniVitaContext};
}

Jafg::JObjectBase::JObjectBase(const LObjectInitializer& ObjectInitializer)
{
    // TODO Check if obj was already registered

    check( ObjectInitializer.Outer )
    this->Outer = ObjectInitializer.Outer;

    check( this->Outer->Employees.Contains(this) == false )
    this->Outer->Employees.Add(this);

    return;
}

void Jafg::JObjectBase::MarkAsGarbage()
{
    check( Tasks::IsOnMasterThread() )

    /*
     * Might be called multiple times when tearing down complex subsystems.
     * Therefore, we just guard this method.
     */
    if (this->bGarbage)
    {
        return;
    }

    this->MarkAsGarbage(true);

    return;
}

void Jafg::JObjectBase::KillYourSelfNow(const bool bMayBeGarbage /* = false */)
{
#if DO_CHECKS
    if (bMayBeGarbage == false)
    {
        check( this->bGarbage == false )
    }
#endif /* DO_CHECKS */

    if (this->bGarbage == false)
    {
        this->MarkAsGarbage(false);
    }

    check( this->Outer )
    check( this->Outer->IsCarnifexValid() )
    this->Outer->GetCarnifex()->DevourGarbageChildNow(this);

    return;
}

void Jafg::JObjectBase::OnDefaultGarbageInternal()
{
    check( this->IsDefault() )
    check( this->IsGarbage() )

    this->OnGarbageDefault();

    if (this->GetVTableSlow()->IsConfig())
    {
        PushConfigFromObject(this);
    }

    return;
}

void Jafg::JObjectBase::MarkAsGarbage(const bool bAddToCarnifex)
{
    check( this->bGarbage == false )
    this->bGarbage = true;

    check( this->Outer )

    this->Outer->Employees.RemoveOnceChecked(this);

    if (bAddToCarnifex)
    {
        check( this->Outer->IsCarnifexValid() )
        this->Outer->GetCarnifex()->AddGarbageChild(this);
    }

    this->OnGarbage();

    return;
}
