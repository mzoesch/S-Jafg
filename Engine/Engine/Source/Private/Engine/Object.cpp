// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Object.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void Jafg::JObject::BeginLife()
{
    JObjectBase::BeginLife();

    for (uint8 i = 0; i < LEngine::GetMaxContexts(); ++i)
    {
        if (const LWorldContext* Context = GEngine->Contexts[i]; Context)
        {
            if (Context->ChildWorld)
            {
                if (static_cast<void*>(Context->ChildWorld) == static_cast<void*>(this->GetContext()))
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
