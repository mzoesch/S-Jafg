// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "PlayerController.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API APlayerController final : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APlayerController)

public:

    virtual void BeginLife() override;
    virtual void Tick(const float DeltaTime) override;
    virtual void EndLife() override;
};

} /* ~Namespace Jafg. */
