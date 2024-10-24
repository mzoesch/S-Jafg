// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Engine/World.h"

Jafg::JObject::JObject(LWorld* InOuter) : JObjectBase(InOuter)
{
    return;
}

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    if (LWorld* InnerCastedOuter = dynamic_cast<LWorld*>(this->GetContext()))
    {
        this->CastedOuter = InnerCastedOuter;
    }

    return;
}
