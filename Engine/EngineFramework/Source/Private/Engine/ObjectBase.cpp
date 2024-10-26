// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectBase.h"

Jafg::Private::JObjectBase::JObjectBase(const LObjectInitializer& ObjectInitializer)
{
    // TODO Check if obj was already registered

    if (ObjectInitializer.Outer)
    {
        this->Outer = ObjectInitializer.Outer;
    }

    return;
}

void Jafg::Private::JObjectBase::BeginLife()
{
    return;
}
