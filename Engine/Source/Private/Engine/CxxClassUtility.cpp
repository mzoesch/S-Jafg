// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClassUtlity.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"

bool Jafg::IsValidFast(LClassOuter const* Outer, JCxxClass const* Obj)
{
    STAT_CYCLE_FUNCTION()

    check(Outer)

    if (Obj == nullptr)
    {
        return false;
    }

    if (Outer->IsHiredHere(Obj) == false)
    {
        return false;
    }

    return Obj->_IsGarbage() == false;
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

    return Obj->_IsGarbage() == false;
}
