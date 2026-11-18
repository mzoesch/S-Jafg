// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PawnComponent.h"
#include "JgcCharacterInputComponent.generated.h"

namespace Jafg
{

class AMovementComponent;
struct LInputActionValue;

} /* ~Namespace Jafg */

namespace Jgc
{

//#
//# Translates the input of a local human into the intent of the movement component of the owning pawn.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcCharacterInputComponent: public Jafg::APawnComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcCharacterInputComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnAttach(Jafg::AActor& InOwner) override;
    virtual void OnNewPersonaController(Jafg::APersonaController* New) override;
    virtual void ParentTick(f32 Dt) override;

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() noexcept;

    void OnMove(Jafg::LInputActionValue const& Value) noexcept;
    void OnLook(Jafg::LInputActionValue const& Value) noexcept;
    void OnRoll(Jafg::LInputActionValue const& Value) noexcept;
    void OnJumpPressed() noexcept;
    void OnJumpHeld() noexcept { this->bFrameJumpHeld = true; }
    void OnCrouchHeld() noexcept { this->bFrameCrouch = true; }
    void OnSprintHeld() noexcept { this->bFrameSprint = true; }
    void ToggleJetpack() noexcept;
    void ToggleDampeners() noexcept;
    void ToggleMagBoots() noexcept;

    NODISCARD Jafg::AMovementComponent* GetMovementComponent() noexcept;
    NODISCARD Jafg::AMovementComponent const* GetMovementComponent() const noexcept;

    NODISCARD FORCEINLINE constexpr bool IsJetpackEngaged() const noexcept { return this->bJetpack; }
    NODISCARD FORCEINLINE constexpr bool AreDampenersEngaged() const noexcept { return this->bDampeners; }
    NODISCARD FORCEINLINE constexpr bool AreMagBootsEngaged() const noexcept { return this->bMagBoots; }

    //# Sensitivity in radians per pixel.
    CLASS_FIELD(EditorEditable)
    f32 LookSensitivity{ 0.0015f };
    //# In radians per second.
    CLASS_FIELD(EditorEditable)
    f32 RollRate{ maths::half_pi_f };
    CLASS_FIELD(EditorEditable)
    bool bInvertPitch{};

private:

    //# Collected while the input is dispatched. Committed and reset once per frame.
    LVec3F FrameMove{ 0.0f };
    bool bFrameJumpHeld{};
    bool bFrameCrouch{};
    bool bFrameSprint{};

    bool bJetpack{};
    bool bDampeners{ true };
    bool bMagBoots{ true };
};

} /* ~Namespace Jgc */
