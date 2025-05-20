// Copyright mzoesch. All rights reserved.

#include "Engine/Object.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

Jafg::LEngine* Jafg::JObject::GetEngine() const
{
    return GEngine;
}

Jafg::LCommandLineInterface* Jafg::JObject::GetCommandLineInterface() const
{
    return GEngine ? GEngine->GetCommandLineInterface() : nullptr;
}

Jafg::LWorld* Jafg::JObject::GetOrCalculateCastedOuter()
{
    if (this->CastedOuter)
    {
        return this->CastedOuter;
    }

    for (const Private::LWorldContext& Context : GEngine->GetContexts())
    {
        check( Context.ChildWorld )
        if (Context.ChildWorld == this->GetOuter())
        {
            this->CastedOuter = Context.ChildWorld;
            return this->CastedOuter;
        }
    }

    return nullptr;
}

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    for (const Private::LWorldContext& Context : GEngine->GetContexts())
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
