// Copyright mzoesch. All rights reserved.

#include "Engine/Carnifex.h"
#include "Engine/ObjectBase.h"
#include "Stats/Stats.h"

void Jafg::LCarnifex::KillAllGarbageChildren()
{
    if (this->GarbageChildren.IsEmpty())
    {
        return;
    }

    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogCarnifex, "Found {} garbage children. Begin to kill them.", this->GarbageChildren.GetSize())

    while (this->GarbageChildren.IsEmpty() == false)
    {
        JObjectBase* Child = *this->GarbageChildren.Peek();
        checkSlow( Child )
        check( Child->IsGarbage() )

        Child->EndLife();

        this->FreeChild(Child);

        this->GarbageChildren.Pop();

        continue;
    }

    check( this->GarbageChildren.IsEmpty() )

    return;
}

void Jafg::LCarnifex::DevourGarbageChildNow(JObjectBase* Child)
{
    if (JObjectBase** GarbageChild = this->GarbageChildren.FindRef(Child); GarbageChild)
    {
        this->GarbageChildren.RemoveOnceChecked(*GarbageChild);
    }
    else
    {
        if (Child->IsGarbage() == false)
        {
            LOG_WARNING(LogCarnifex, "The provided child is not garbage - but still alive. {}", Child->GetVTableSlow()->GetSpacedClassName())

            check( Child->GetVTable() )

            Child->MarkAsGarbage();
            if (GarbageChild = this->GarbageChildren.FindRef(Child); GarbageChild)
            {
                this->GarbageChildren.RemoveOnceChecked(*GarbageChild);
            }
            else
            {
                LOG_ERROR(LogCarnifex, "The child that was not garbage is not part of this killer. Sending it to the next killer.")
                if (Child->Outer && Child->Outer->GetCarnifex())
                {
                    Child->Outer->GetCarnifex()->DevourGarbageChildNow(Child);
                }
                else
                {
                    LOG_ERROR(LogCarnifex, "The child is not hunted by any carnifex. Devouring foreign child now.")
                    Child->EndLife();
                    this->FreeChild(Child);
                }

                return;
            }
        }
        else
        {
            check( Child->GetOuter() && Child->GetOuter()->GetCarnifex() && Child->GetOuter()->GetCarnifex() == this )
        }
    }

    checkSlow( Child )
    check( Child->IsGarbage() )

    Child->EndLife();

    this->FreeChild(Child);

    return;
}

void Jafg::LCarnifex::FreeChild(JObjectBase* Child)
{
    checkSlow( Child )

    Child->~JObjectBase();
    ::free(Child);

    return;
}
