// Copyright mzoesch. All rights reserved.

#include "Engine/Carnifex.h"
#include "Engine/CxxClass.h"
#include "Stats/Stats.h"

void Jafg::LCarnifex::KillAllGarbageChildren()
{
    if (this->GarbageChildren.empty())
    {
        return;
    }

    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogCarnifex, "Found {} garbage children. Begin to kill them.", this->GarbageChildren.size())

    while (this->GarbageChildren.empty() == false)
    {
        auto& Child{ this->GarbageChildren.back() };
        check( Child->IsGarbage() )

        Child->EndLife();
        this->GarbageChildren.pop_back();

        continue;
    }

    check( this->GarbageChildren.empty() )

    return;
}

void Jafg::LCarnifex::DevourGarbageChildNow(TUnique<JCxxClass> Child)
{
    check( Child.get() )

    if (auto It{ algo::find(this->GarbageChildren, Child.get(), algo::unique_raw{})}; It != this->GarbageChildren.end())
    {
        checkSlow( It->get() == Child.get() )
        check( It->get()->IsGarbage() )
        this->GarbageChildren.erase(It);
    }
    else
    {
        if (Child->IsGarbage() == false)
        {
            LOG_WARNING(LogCarnifex,
                "The provided child is not garbage - but still alive. {}",
                Child->GetVirtualTable().GetFullyQualifiedName()
                )

            Child->MarkAsGarbage(JCxxClass::EMarkAsGarbageBehavior::Ignore, ECxxRecordTearDownReason::Default);
        }
        check( algo::contains(this->GarbageChildren, Child.get(), algo::unique_raw{}) == false )
    }

    checkSlow( Child.get() )
    check( Child->IsGarbage() )
    check( Child->Outer == nullptr && Child->Outer->IsHiredHere(Child.get()) == false )
    check( Child->HasEndedLife() == false )
    Child->EndLife();

    return;
}
