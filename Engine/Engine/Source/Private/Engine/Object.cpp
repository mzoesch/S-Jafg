// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Object.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

Jafg::LEngine* Jafg::JObject::GetEngine() const
{
    return GEngine;
}

Jafg::LApplicationInstance* Jafg::JObject::GetApplicationInstanceSubsystem() const
{
    return GEngine->GetApplicationInstance();
}

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    for (uint8 i = 0; i < LEngine::GetMaxContexts(); ++i)
    {
        if (const LWorldContext* Context = GEngine->Contexts[i]; Context)
        {
            if (Context->ChildWorld)
            {
                if (static_cast<void*>(Context->ChildWorld) == static_cast<void*>(this->GetOuter()))
                {
                    this->CastedOuter = Context->ChildWorld;
                    return;
                }
            }
        }

        continue;
    }

    return;
}
