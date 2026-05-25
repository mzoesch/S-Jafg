// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
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
        (void)this->ActivateUserInputContext();
    }
    virtual void OnNewPersonaController(Jafg::APersonaController* New) override
    {
        Super::OnNewPersonaController(New);
        (void)this->ActivateUserInputContext();
    }

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() const noexcept;

    void OnMove(Jafg::LInputActionValue const& Value);
    void OnRotate(Jafg::LInputActionValue const& Value);

    FORCEINLINE constexpr f32 GetSensitivity() const noexcept { return this->Sensitivity; }
    FORCEINLINE void SetSensitivity(f32 NewSensitivity) noexcept { this->Sensitivity = NewSensitivity; }

private:

    //# Sensitivity in radians per pixel.
    f32 Sensitivity{0.1f};

    f32 CachedPitch{};
};

} /* ~Namespace Jgc */
