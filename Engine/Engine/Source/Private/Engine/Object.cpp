// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Engine/World.h"

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    if (LWorld* InnerCastedOuter = dynamic_cast<LWorld*>(this->GetContext()); InnerCastedOuter)
    {
        this->CastedOuter = InnerCastedOuter;
    }

    return;
}
