// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PawnComponent.h"
#include "Framework/Pawn.h"
#include "Framework/MovementTypes.h"
#include "MovementComponent.generated.h"

namespace Jafg
{

class APawn;
class APersonaController;

//#
//# What the controlling entity currently wants.
//# The intent is sampled once per physics tick into an #LMovementInput command.
//#
struct LMovementIntent final
{
    //# Local movement axes. @see MovementTypes.h for the conventions.
    LVec3F Move{ 0.0f };
    //# If set, overrides #Move with a world space direction (convenience for bots).
    std::optional<LVec3F> MoveWorldDirection;
    //# Accumulated look delta that was not yet consumed by a command.
    LVec3F PendingLook{ 0.0f };
    //# If set, the pawn turns towards this world space direction (convenience for bots).
    std::optional<LVec3F> LookAtDirection;
    //# Turn rate in radians per second that is used for #LookAtDirection.
    f32 LookAtTurnRate{ 2.0f * maths::pi_f };
    //# Level and state buttons. Edges (#EMovementButtonBits::JumpPressed) are latched until consumed.
    EMovementButtonFlags Buttons{ EMovementButtonBits::Dampeners | EMovementButtonBits::MagBoots };
};

//#
//# Base of all movement components. Movement components move the pawn they are attached to.
//#
//# Human and bot controllers both only interact with the intent API (#SetMoveInput, #AddLookInput, #Jump, ...).
//# Once per physics tick, the intent is sampled into an #LMovementInput command which is then simulated by the
//# concrete subclass (#SimulateTick). The command pipeline is designed for server authoritative multiplayer:
//#   - #EMovementNetRole::Authority simulates commands that come from the local intent or, if remotely controlled,
//#     commands that were received with #QueueRemoteInput.
//#   - #EMovementNetRole::AutonomousProxy predicts from the local intent, remembers every command and its result and
//#     reconciles once #ApplyAuthoritativeSnapshot is called.
//#   - #EMovementNetRole::SimulatedProxy only follows #ApplyAuthoritativeSnapshot.
//#
//# Subclasses implement #SimulateTick, #CaptureSnapshot, #RestoreSnapshot and #UpdatePresentation. A simulation
//# must be a deterministic function of the restored snapshot and the input commands.
//#
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API AMovementComponent : public APawnComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AMovementComponent)
    {
        this->bTick = true;
        this->bPhysicsTick = true;
    }

public:

    virtual void OnAttach(AActor& InOwner) override;
    virtual void ParentTick(f32 Dt) override;
    virtual void ParentPhysicsTick(f32 Dt, bool bFollowup) override;

    //# ---------- Intent ----------

    //# Set the view-relative move axes (X right, Y up, Z forward). The vector is clamped to unit length.
    void SetMoveInput(LVec3F const& Axes) noexcept;
    //# Move along a world space direction instead of view-relative axes. The length (clamped to one) scales the speed.
    void SetMoveDirectionWorld(LVec3F const& Direction) noexcept;
    //#
    //# Add a look delta in radians (X yaw right, Y pitch up, Z roll right).
    //# The view already shows the delta in the next frame, even if no physics tick consumed it yet.
    //#
    void AddLookInput(LVec3F const& Delta) noexcept;
    //# Continuously turn towards a world space direction. Pass nullopt to stop.
    void SetLookAtDirection(std::optional<LVec3F> Direction, f32 TurnRate = 2.0f * maths::pi_f) noexcept;
    //# Request a jump. The request is latched until the next command consumes it.
    void Jump() noexcept { this->Intent.Buttons |= EMovementButtonBits::JumpPressed; }
    void SetJumpHeld(bool b) noexcept { this->SetButton(EMovementButtonBits::JumpHeld, b); }
    void SetCrouch(bool b) noexcept { this->SetButton(EMovementButtonBits::Crouch, b); }
    void SetSprint(bool b) noexcept { this->SetButton(EMovementButtonBits::Sprint, b); }
    void SetJetpackEngaged(bool b) noexcept { this->SetButton(EMovementButtonBits::Jetpack, b); }
    void SetDampenersEngaged(bool b) noexcept { this->SetButton(EMovementButtonBits::Dampeners, b); }
    void SetMagBootsEngaged(bool b) noexcept { this->SetButton(EMovementButtonBits::MagBoots, b); }
    void SetButton(EMovementButtonBits Bit, bool b) noexcept
    {
        if (b)
        {
            this->Intent.Buttons |= Bit;
        }
        else
        {
            this->Intent.Buttons &= ~EMovementButtonFlags{Bit};
        }
    }
    //# Reset all transient intent (axes, pending look and edges). States (jetpack, dampeners, boots) are kept.
    void ClearIntent() noexcept;
    NODISCARD FORCEINLINE constexpr LMovementIntent const& GetIntent() const noexcept { return this->Intent; }

    //# ---------- State ----------

    NODISCARD FORCEINLINE constexpr EMovementMode GetMovementMode() const noexcept { return this->Mode; }
    //#
    //# Freeze the pawn in place (e.g., while seated in a vehicle or during a cutscene). Unfreezing resumes as falling;
    //# the next tick picks the correct mode.
    //#
    void SetFrozen(bool bFrozen);
    NODISCARD FORCEINLINE constexpr bool IsFrozen() const noexcept { return this->Mode == EMovementMode::None; }
    NODISCARD virtual LVec3F GetVelocity() const noexcept PURE_VIRTUAL(return {};)
    //# The up vector of the body.
    NODISCARD virtual LVec3F GetUp() const noexcept PURE_VIRTUAL(return {};)
    //# Move the pawn instantly to a new location (and optionally rotation). Keeps the velocity.
    virtual void Teleport(LWorldVec3 const& Location, std::optional<LWorldQuat> Rotation = {}) PURE_VIRTUAL()

    //# Broadcast after the mode changed. Return true to unbind.
    TMulticastDelegate<bool(AMovementComponent& Comp, EMovementMode Previous, EMovementMode New)> OnMovementModeChanged;

    //# ---------- Netcode ----------

    NODISCARD FORCEINLINE constexpr EMovementNetRole GetNetRole() const noexcept { return this->NetRole; }
    void SetNetRole(EMovementNetRole Role) noexcept;
    //# Authority only. If true, commands are taken from #QueueRemoteInput instead of the local intent.
    NODISCARD FORCEINLINE constexpr bool IsRemotelyControlled() const noexcept { return this->bRemotelyControlled; }
    void SetRemotelyControlled(bool b) noexcept;

    //# Authority only. Enqueue a command that was received from the owning client. Old or duplicated commands are dropped.
    void QueueRemoteInput(LMovementInput const& Input) noexcept;
    //# The sequence of the last simulated command. A server acknowledges this sequence with its snapshots.
    NODISCARD FORCEINLINE constexpr u32 GetLastSimulatedSequence() const noexcept { return this->LastSimulatedSequence; }

    //#
    //# Autonomous and simulated proxies only. Apply a snapshot that was received from the server.
    //# An autonomous proxy compares the snapshot with its prediction and, if it diverged, restores the snapshot and
    //# replays all commands that the server did not yet acknowledge.
    //#
    void ApplyAuthoritativeSnapshot(LMovementSnapshot const& Snapshot);
    NODISCARD FORCEINLINE constexpr u64 GetCorrectionCount() const noexcept { return this->CorrectionCount; }

    //# Broadcast for every command that was produced from the local intent. A net layer sends these to the server.
    TMulticastDelegate<bool(LMovementInput const& Input)> OnInputCommandProduced;

    //# The complete simulated state. Its #LMovementSnapshot::Sequence is the last simulated sequence.
    NODISCARD virtual LMovementSnapshot CaptureSnapshot() const PURE_VIRTUAL(return {};)

    //# Tolerances that decide whether a prediction diverged from the authoritative state.
    f32 CorrectionPositionTolerance{ 0.01f };
    f32 CorrectionVelocityTolerance{ 0.05f };

protected:

    //# Integrate exactly one command. Must be deterministic. Called during physics ticks and during replays.
    virtual void SimulateTick(LMovementInput const& Input, f32 Dt) PURE_VIRTUAL()
    //# Restore the complete simulated state.
    virtual void RestoreSnapshot(LMovementSnapshot const& Snapshot) PURE_VIRTUAL()
    //# Write the simulated state (and any not yet consumed look input) to the pawn. Called every frame and after ticks.
    virtual void UpdatePresentation(f32 Dt) {}

    //# Converts a world space direction into view-relative move axes using the current state.
    NODISCARD virtual LVec3F WorldDirectionToMoveAxes(LVec3F const& Direction) const noexcept { return Direction; }
    //# The look delta that turns towards #Direction by at most #MaxAngle radians.
    NODISCARD virtual LVec3F ComputeLookDeltaTowards(LVec3F const& Direction, f32 MaxAngle) const noexcept { return LVec3F{0.0f}; }
    //# The look delta that was already applied to the view but is not yet part of the simulated state.
    NODISCARD FORCEINLINE constexpr LVec3F const& GetPendingLook() const noexcept { return this->Intent.PendingLook; }

    //# @return Whether the prediction diverged from the authoritative state.
    NODISCARD virtual bool NeedsCorrection(LMovementSnapshot const& Predicted, LMovementSnapshot const& Authoritative) const noexcept;

    //# Subclasses call this from #SimulateTick or #RestoreSnapshot.
    void SetMovementMode(EMovementMode New);
    //#
    //# Called after the mode changed. Meant for cosmetic reactions only (sounds, animations): it is not called for
    //# every intermediate change while replaying predicted moves.
    //#
    virtual void OnMovementModeChangedImpl(EMovementMode Previous, EMovementMode New) {}

    //#
    //# Whether predicted moves are currently being re-simulated after a correction. Cosmetic events (landing sounds,
    //# jump effects, ...) should not be triggered while replaying, as they already happened.
    //#
    NODISCARD FORCEINLINE constexpr bool IsReplaying() const noexcept { return this->bReplaying; }

private:

    //# Sample the intent into a new command and consume the edges and the pending look.
    NODISCARD LMovementInput ConsumeIntent(f32 Dt) noexcept;
    void SimulateRemoteInputs(f32 Dt);

    LMovementIntent Intent;

    CLASS_FIELD(EditorVisible)
    EMovementMode Mode{ EMovementMode::Falling };
    CLASS_FIELD(EditorVisible)
    EMovementNetRole NetRole{ EMovementNetRole::Authority };
    bool bRemotelyControlled{};
    bool bReplaying{};

    u32 NextSequence{ 1 };
    u32 LastSimulatedSequence{};
    //# Autonomous proxy only. The newest sequence that the server acknowledged.
    u32 LastAcknowledgedSequence{};
    u64 CorrectionCount{};

    struct LPredictedMove final
    {
        LMovementInput Input;
        f32 Dt{};
        LMovementSnapshot Result;
    };
    //# Autonomous proxy only. Commands that were not yet acknowledged by the server, oldest first.
    TArray<LPredictedMove> PredictedMoves;
    //# Authority only. Received but not yet simulated commands, oldest first.
    TArray<LMovementInput> RemoteInputs;

    //# Upper bound of remembered predictions (~2 seconds at 60Hz). Older ones are dropped.
    static constexpr std::size_t MaxPredictedMoves{ 128 };
    //# Upper bound of buffered remote commands. Exceeding commands are dropped (oldest first).
    static constexpr std::size_t MaxRemoteInputs{ 32 };
    //# If more than this many remote commands are buffered, two are simulated per tick to catch up.
    static constexpr std::size_t RemoteInputCatchUpThreshold{ 4 };
};

} /* ~Namespace Jafg */
