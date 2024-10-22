// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/Actor.h"

namespace Jafg
{

class ENGINE_API APlayerController final : public AActor
{
public:

    APlayerController() = delete;
    explicit APlayerController(LWorld* World) : AActor(World) { }

protected:

    virtual void BeginLife() override;
    virtual void Tick(const float DeltaTime) override;
    virtual void EndLife() override;
};

} /* ~Namespace Jafg. */
