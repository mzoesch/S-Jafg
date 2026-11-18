// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "JgcCharacterInputComponent.generated.h"

namespace Jafg
{

class AMovementComponent;
class LLocalLackey;
struct LInputActionValue;

} /* ~Namespace Jafg */

namespace Jgc
{

class AInteractableComponent;
class WStatusHud;

struct LInteractionTarget final
{
    AInteractableComponent* Interactable;
    //# Where the cursor points at the interactable. In world space.
    LWorldVec3 Location;
};

//#
//# Translates the input of a local human into the intent of the movement component of the owning actor. Active while
//# a local controller possesses the avatar of the actor.
//#
//# While the cursor action is held, the mouse no longer looks around but moves a cursor that is kept inside the
//# view. Clicking an interactable (e.g., a vehicle) under the cursor interacts with it.
//#
DECLARE_JAFG_CLASS()
class JGC_API AJgcCharacterInputComponent: public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AJgcCharacterInputComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnAttach(Jafg::AActor& InOwner) override;
    virtual void OnGarbage(Jafg::EJxxRecordTearDownReason Reason) override;
    virtual void OnPossessionChanged(Jafg::APersonaController* Previous, Jafg::APersonaController* New) override;
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
    void OnCursorHeld() noexcept { this->bFrameCursorHeld = true; }
    //# @return Whether an interaction happened. Interactions may change the active input contexts.
    bool OnInteractPressed() noexcept;

    NODISCARD Jafg::AMovementComponent* GetMovementComponent() noexcept;
    NODISCARD Jafg::AMovementComponent const* GetMovementComponent() const noexcept;

    NODISCARD FORCEINLINE constexpr bool IsJetpackEngaged() const noexcept { return this->bJetpack; }
    NODISCARD FORCEINLINE constexpr bool AreDampenersEngaged() const noexcept { return this->bDampeners; }
    NODISCARD FORCEINLINE constexpr bool AreMagBootsEngaged() const noexcept { return this->bMagBoots; }
    //# Whether the cursor is shown instead of looking around.
    NODISCARD FORCEINLINE constexpr bool IsInCursorMode() const noexcept { return this->bCursorMode; }

    //# The interactable under the cursor. Empty outside of cursor mode or if nothing interactable is there.
    NODISCARD std::optional<LInteractionTarget> TraceInteractable() noexcept;

    //# Sensitivity in radians per pixel.
    CLASS_FIELD(EditorEditable)
    f32 LookSensitivity{ 0.0015f };
    //# In radians per second.
    CLASS_FIELD(EditorEditable)
    f32 RollRate{ maths::half_pi_f };
    CLASS_FIELD(EditorEditable)
    bool bInvertPitch{};
    //# How far the cursor traces for interactables. Each interactable limits this further. In meters.
    CLASS_FIELD(EditorEditable)
    f32 InteractionTraceLength{ 100.0f };

private:

    //# The lackey of the controller, if it is a local one.
    NODISCARD static Jafg::LLocalLackey* GetLocalLackey(Jafg::APersonaController* Ctrl) noexcept;
    //# @param Lackey The lackey whose input is changed. Null to only change the state.
    void SetCursorMode(bool bEnabled, Jafg::LLocalLackey* Lackey) noexcept;
    //# Feedback for the interactable under the cursor.
    void UpdateHover() noexcept;
    void SetHoversInteractable(bool bHovers, Jafg::LLocalLackey* Lackey) noexcept;
    //# Shows the HUD in the viewport of #Lackey. Null removes it.
    void ShowHud(Jafg::LLocalLackey* Lackey) noexcept;
    void UpdateHud() noexcept;
    void ResetFrameInput() noexcept;

    //# Collected while the input is dispatched. Committed and reset once per frame.
    LVec3F FrameMove{ 0.0f };
    bool bFrameJumpHeld{};
    bool bFrameCrouch{};
    bool bFrameSprint{};
    bool bFrameCursorHeld{};

    bool bCursorMode{};
    bool bHoversInteractable{};

    //# Shows the jetpack, the dampeners and the magnetic boots. Lives in a viewport, not in the world (see #WStatusHud).
    WStatusHud* Hud{};
    std::size_t HudJetpackRow{};
    std::size_t HudDampenersRow{};
    std::size_t HudMagBootsRow{};

    bool bJetpack{};
    bool bDampeners{ true };
    bool bMagBoots{ true };
};

} /* ~Namespace Jgc */
