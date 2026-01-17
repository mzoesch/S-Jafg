// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "DebugCameraComponent.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JGC_API JDebugCameraComponent final : public Jafg::JActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JDebugCameraComponent)

public:

    virtual void OnAttach(Jafg::AActor* InOwner) override;

    void OnMove(Jafg::LInputActionValue const& Value);
    void OnRotate(Jafg::LInputActionValue const& Value);

private:

    f32 Sensitivity{0.1f};
};

} /* ~Namespace Jgc */
