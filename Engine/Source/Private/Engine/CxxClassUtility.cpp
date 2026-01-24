// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClassUtlity.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"

bool Jafg::IsValidFast(LClassOuter const* Outer, JCxxClass const* Obj)
{
    STAT_CYCLE_FUNCTION()

    check( Outer )

    if (Obj == nullptr)
    {
        return false;
    }

    if (Outer->IsHiredHere(Obj) == false)
    {
        return false;
    }

    return Obj->IsGarbage() == false;
}

bool Jafg::IsValidSlow(LClassOuter const* Outer, JCxxClass const* Obj)
{
    STAT_CYCLE_FUNCTION()

    if (Outer == nullptr || Obj == nullptr)
    {
        return false;
    }

    if (GEngine == nullptr)
    {
        LOG_WARNING(LogObjectInternal, "Engine is invalid.")
        return false;
    }

    if (GEngine->IsClassOuterKnown(Outer) == false)
    {
        return false;
    }

    if (Outer->IsHiredHere(Obj) == false)
    {
        return false;
    }

    return Obj->IsGarbage() == false;
}

void Jafg::PullConfigForCxxObject(LCxxClass* Obj)
{
    // TODO: Config
}

void Jafg::PushConfigFromCxxObject(LCxxClass const& Obj)
{
    // TODO: Config
}

Jafg::JCxxClass* Jafg::Private::LClassOuterMiscellaneousAccessor::NewDeferredObjectImpl(LClassOuter* Outer, LCxxClass const& Class)
{
    check( Tasks::IsOnMasterThread() )
    check( Outer )

    if (Class.IsAbstract())
    {
        panicMsgf( "Tried to instantiate abstract class [{}].", Class.GetFullyQualifiedName() );
    }

    // This is very dirty. In C26 we can do this maybe much better with the new reflection features.
    // and then explicitly copy this with LCxxCDRCopy, or something like this in the Ctor.
    TUnique<JCxxClass> Obj { LCxxRecordMiscellaneousAccessor::MallocClass(*Class.GetCDR()) };

    LCxxRecordMiscellaneousAccessor::ChangeOuter(Obj.get(), Outer);
    Outer->Employees.emplace_back(std::move(Obj));
    return Outer->Employees.back().get();
}
