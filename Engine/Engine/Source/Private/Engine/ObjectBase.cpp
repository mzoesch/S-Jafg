// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/ObjectBase.h"
#include "Engine/Carnifex.h"

Jafg::JObjectBase::JObjectBase(const LObjectInitializer& ObjectInitializer)
{
    // TODO Check if obj was already registered

    check( ObjectInitializer.Outer )
    this->Outer = ObjectInitializer.Outer;

    check( this->Outer->Employees.Contains(this) == false )
    this->Outer->Employees.Add(this);

    return;
}

Jafg::JObjectBase::~JObjectBase()
{
    /*
     * If this check triggers, you might have done one of the following things that are forbidden:
     *   - Manually deleted an object via ~delete or ~delete[].
     *   - Used a smart pointer not from the jafg library, for example, a std::shared_ptr (Which are not
     *     compatible).
     *
     * Create a new object with NewObject<T>. Delete them by either calling #MarkAsGarbage to get them discarded
     * at the next engine butcher cycle or by calling #KillYourSelfNow to get them discarded immediately (comparable
     * with a call to the delete operator).
     */
    check( this->bGarbage )
}

void Jafg::JObjectBase::MarkAsGarbage()
{
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
    check( this->Outer->Carnifex )
    check( this->Outer->GetCarnifex() )
    this->Outer->GetCarnifex()->DevourGarbageChildNow(this);

    return;
}

void Jafg::JObjectBase::OnDefaultGarbage()
{
    check( this->IsDefault() )
    if (this->GetVTableSlow()->IsConfig())
    {
        PushConfigFromObject(this);
    }

    return;
}

void Jafg::JObjectBase::MarkAsGarbage(const bool bAddToCarnifex)
{
    checkSlow( this->bGarbage == false )
    this->bGarbage = true;

    check( this->Outer )

#if IN_SHIPPING
    this->Outer->Employees.RemoveOnce(this);
#else /* IN_SHIPPING */
    const bool bWasRemoved = this->Outer->Employees.RemoveOnce(this);
    check( bWasRemoved )
#endif /* !IN_SHIPPING */

    if (bAddToCarnifex)
    {
        check( this->Outer->Carnifex )
        check( this->Outer->GetCarnifex() )
        this->Outer->GetCarnifex()->AddGarbageChild(this);
    }

    this->OnGarbage();

    return;
}
