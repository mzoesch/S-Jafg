// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Carnifex.h"
#include "Engine/ObjectBase.h"

void Jafg::LCarnifex::KillAllGarbageChildren()
{
    if (this->GarbageChildren.IsEmpty())
    {
        return;
    }

    LOG_TRACE(LogCarnifex, "Found {} garbage children. Begin to kill them.", this->GarbageChildren.GetSize())

    while (this->GarbageChildren.IsEmpty() == false)
    {
        Private::JObjectBase* Child = *this->GarbageChildren.Peek();
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

void Jafg::LCarnifex::DevourGarbageChildNow(Private::JObjectBase* Child)
{
    if (Private::JObjectBase** GarbageChild = this->GarbageChildren.FindRef(Child); GarbageChild)
    {
        this->GarbageChildren.RemoveOnceChecked(*GarbageChild);
    }

    checkSlow( Child )
    check( Child->IsGarbage() )

    Child->EndLife();

    this->FreeChild(Child);

    return;
}

void Jafg::LCarnifex::FreeChild(Private::JObjectBase* Child)
{
    checkSlow( Child )

    Child->~JObjectBase();
    ::free(Child);

    return;
}
