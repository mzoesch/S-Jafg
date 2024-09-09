// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Object.h"

class AActor;

class JWorld final : public JObject
{
public:

    void Init();
    void TearDown();

    FORCEINLINE auto GetActorCount() const -> sizetype { return this->Actors.size(); }
    FORCEINLINE auto GetActors() const -> const std::vector<AActor*>& { return this->Actors; }

    template <class T = AActor>
    FORCEINLINE auto GetFirstActor() const -> T*
    {
        for (auto Actor : this->Actors)
        {
            if (auto CastedActor = dynamic_cast<T*>(Actor))
            {
                return CastedActor;
            }
        }

        return nullptr;
    }

    template <class T = AActor>
    FORCEINLINE auto GetFirstActorChecked() const -> AActor*
    {
        if (T* Actor = this->GetFirstActor<T>())
        {
            return Actor;
        }

        check( false && "Failed to find actor of specified type" )

        return nullptr;
    }

private:

    std::vector<AActor*> Actors;
};
