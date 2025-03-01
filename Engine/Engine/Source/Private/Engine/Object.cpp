// Copyright mzoesch. All rights reserved.

#include "Engine/Object.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

Jafg::LEngine* Jafg::JObject::GetEngine() const
{
    return GEngine;
}

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    for (const LWorldContext& Context : GEngine->GetContexts())
    {
        check( Context.ChildWorld )
        if (Context.ChildWorld == this->GetOuter())
        {
            this->CastedOuter = Context.ChildWorld;
            return;
        }
    }

    return;
}
