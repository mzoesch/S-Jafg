// Copyright mzoesch. All rights reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "DebugCameraComponent.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JGC_API ADebugCameraComponent final : public Jafg::APawnComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ADebugCameraComponent)

public:

    virtual void OnAttach(Jafg::AActor& InOwner) override
    {
        Super::OnAttach(InOwner);
        this->ActivateUserInputContext();
    }
    virtual void OnNewPersonaController(Jafg::APersonaController* New)
    {
        Super::OnNewPersonaController(New);
        this->ActivateUserInputContext();
    }

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() const noexcept;

    void OnMove(Jafg::LInputActionValue const& Value);
    void OnRotate(Jafg::LInputActionValue const& Value);

private:

    f32 Sensitivity{0.1f};
};

} /* ~Namespace Jgc */
