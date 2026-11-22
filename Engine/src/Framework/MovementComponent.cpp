// Copyright mzoesch. All rights reserved.

#include "Framework/MovementComponent.h"
#include "Framework/Actor.h"
#include "Framework/RigidComponent.h"
#include "Engine/World.h"

namespace
{

NODISCARD bool IsFinite(LVec3F const& V) noexcept
{
    return std::isfinite(V.x) && std::isfinite(V.y) && std::isfinite(V.z);
}

//# Never trust commands of remote clients.
NODISCARD Jafg::LMovementInput SanitizeRemoteInput(Jafg::LMovementInput Input) noexcept
{
    if (!IsFinite(Input.Move))
    {
        Input.Move = LVec3F{0.0f};
    }
    Input.Move = maths::clamp(Input.Move, LVec3F{-1.0f}, LVec3F{1.0f});
    Input.Move = Jafg::Movement::ClampMagnitude(Input.Move, 1.0f);

    if (!IsFinite(Input.Look))
    {
        Input.Look = LVec3F{0.0f};
    }
    /* No one turns faster than half a revolution per tick. */
    Input.Look = maths::clamp(Input.Look, LVec3F{-maths::pi_f}, LVec3F{maths::pi_f});

    return Input;
}

} /* ~Namespace <Anonymous> */

void Jafg::AMovementComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    if (!InOwner.CanEverPhysicsTick())
    {
        LOG_FATAL(LogMovement, "[{}]: Owner [{}] can never physics tick. Movement is simulated in physics ticks."
            , this->GetNameAsString(), InOwner.GetNameAsString()
            )
    }
    if (!InOwner.HasRootComponent())
    {
        LOG_FATAL(LogMovement, "[{}]: Owner [{}] has no root component to move."
            , this->GetNameAsString(), InOwner.GetNameAsString()
            )
    }
    if (InOwner.GetRootComponent().IsA<ARigidComponent>())
    {
        LOG_FATAL(LogMovement, "[{}]: The root component of [{}] is a [{}]. Movement components move their root kinematically and would fight the rigid body."
            , this->GetNameAsString(), InOwner.GetNameAsString(), algo::type_name<ARigidComponent>()
            )
    }

    std::size_t MovementComponents{};
    for (auto const& Comp: InOwner.GetComponents())
    {
        if (Comp->IsA<AMovementComponent>())
        {
            ++MovementComponents;
        }
    }
    if (MovementComponents != 1)
    {
        LOG_FATAL(LogMovement, "[{}]: Owner [{}] has [{}] movement components. Only one may move an actor."
            , this->GetNameAsString(), InOwner.GetNameAsString(), MovementComponents
            )
    }

    this->GetWorld().RegisterPresentableObject(this);
    this->bPresentable = true;
}

void Jafg::AMovementComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    if (this->bPresentable)
    {
        this->GetWorld().UnregisterPresentableObject(this);
        this->bPresentable = false;
    }
    Super::OnGarbage(Reason);
}

void Jafg::AMovementComponent::Present(f32 PhysicsAlpha)
{
    this->PresentationAlpha = PhysicsAlpha;
    this->UpdatePresentation(0.0f);
}

void Jafg::AMovementComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);
    this->UpdatePresentation(Dt);
}

void Jafg::AMovementComponent::ParentPhysicsTick(f32 Dt, bool bFollowup)
{
    Super::ParentPhysicsTick(Dt, bFollowup);

    switch (this->NetRole)
    {
    case EMovementNetRole::Authority:
    {
        if (this->bRemotelyControlled)
        {
            this->SimulateRemoteInputs(Dt);
        }
        else
        {
            LMovementInput const Input{this->ConsumeIntent(Dt)};
            this->SimulateTick(Input, Dt);
            this->LastSimulatedSequence = Input.Sequence;
            this->OnInputCommandProduced.Broadcast(Input);
        }
        break;
    }
    case EMovementNetRole::AutonomousProxy:
    {
        LMovementInput const Input{this->ConsumeIntent(Dt)};
        this->SimulateTick(Input, Dt);
        this->LastSimulatedSequence = Input.Sequence;

        if (this->PredictedMoves.size() >= MaxPredictedMoves)
        {
            LOG_WARNING(LogMovement, "[{}]: Server did not acknowledge [{}] predicted moves. Dropping the oldest."
                , this->GetNameAsString(), this->PredictedMoves.size()
                )
            this->PredictedMoves.erase(this->PredictedMoves.begin());
        }
        this->PredictedMoves.push_back(LPredictedMove{.Input = Input, .Dt = Dt, .Result = this->CaptureSnapshot()});

        this->OnInputCommandProduced.Broadcast(Input);
        break;
    }
    case EMovementNetRole::SimulatedProxy:
    {
        /* Driven by snapshots only. */
        break;
    }
    }

    this->UpdatePresentation(0.0f);
}

void Jafg::AMovementComponent::SetMoveInput(LVec3F const& Axes) noexcept
{
    this->Intent.Move = IsFinite(Axes) ? Movement::ClampMagnitude(Axes, 1.0f) : LVec3F{0.0f};
    this->Intent.MoveWorldDirection.reset();
}

void Jafg::AMovementComponent::SetMoveDirectionWorld(LVec3F const& Direction) noexcept
{
    this->Intent.MoveWorldDirection = IsFinite(Direction) ? Movement::ClampMagnitude(Direction, 1.0f) : LVec3F{0.0f};
}

void Jafg::AMovementComponent::AddLookInput(LVec3F const& Delta) noexcept
{
    if (IsFinite(Delta))
    {
        this->Intent.PendingLook += Delta;
    }
}

void Jafg::AMovementComponent::SetLookAtDirection(std::optional<LVec3F> Direction, f32 TurnRate /* = 2.0f * maths::pi_f */) noexcept
{
    if (Direction && (!IsFinite(*Direction) || maths::squared_magnitude(*Direction) < maths::small_number_f))
    {
        Direction.reset();
    }
    this->Intent.LookAtDirection = Direction ? std::optional<LVec3F>{maths::normalize(*Direction)} : std::nullopt;
    this->Intent.LookAtTurnRate = maths::max(TurnRate, 0.0f);
}

void Jafg::AMovementComponent::ClearIntent() noexcept
{
    this->Intent.Move = LVec3F{0.0f};
    this->Intent.MoveWorldDirection.reset();
    this->Intent.PendingLook = LVec3F{0.0f};
    this->Intent.LookAtDirection.reset();
    this->Intent.Buttons &= ~(EMovementButtonBits::JumpPressed
        | EMovementButtonBits::JumpHeld
        | EMovementButtonBits::Crouch
        | EMovementButtonBits::Sprint
        );
}

void Jafg::AMovementComponent::SetFrozen(bool bFrozen)
{
    if (bFrozen)
    {
        this->SetMovementMode(EMovementMode::None);
    }
    else if (this->Mode == EMovementMode::None)
    {
        this->SetMovementMode(EMovementMode::Falling);
    }
}

void Jafg::AMovementComponent::SetNetRole(EMovementNetRole Role) noexcept
{
    if (this->NetRole == Role)
    {
        return;
    }

    LOG_VERBOSE(LogMovement, "[{}]: Changing net role from [{}] to [{}]."
        , this->GetNameAsString(), std::to_underlying(this->NetRole), std::to_underlying(Role)
        )
    this->NetRole = Role;
    this->PredictedMoves.clear();
    this->RemoteInputs.clear();
}

void Jafg::AMovementComponent::SetRemotelyControlled(bool b) noexcept
{
    this->bRemotelyControlled = b;
    this->RemoteInputs.clear();
}

void Jafg::AMovementComponent::QueueRemoteInput(LMovementInput const& Input) noexcept
{
    if (this->NetRole != EMovementNetRole::Authority || !this->bRemotelyControlled)
    {
        LOG_WARNING(LogMovement, "[{}]: Received a remote input but is not a remotely controlled authority. Ignoring."
            , this->GetNameAsString()
            )
        return;
    }

    u32 const Newest{this->RemoteInputs.empty() ? this->LastSimulatedSequence : this->RemoteInputs.back().Sequence};
    if (Input.Sequence <= Newest)
    {
        /* Clients send their commands redundantly. */
        return;
    }

    if (this->RemoteInputs.size() >= MaxRemoteInputs)
    {
        LOG_WARNING(LogMovement, "[{}]: Too many buffered remote inputs [{}]. Dropping the oldest."
            , this->GetNameAsString(), this->RemoteInputs.size()
            )
        this->RemoteInputs.erase(this->RemoteInputs.begin());
    }

    this->RemoteInputs.push_back(::SanitizeRemoteInput(Input));
}

void Jafg::AMovementComponent::ApplyAuthoritativeSnapshot(LMovementSnapshot const& Snapshot)
{
    switch (this->NetRole)
    {
    case EMovementNetRole::Authority:
    {
        LOG_WARNING(LogMovement, "[{}]: An authority does not accept authoritative snapshots. Ignoring."
            , this->GetNameAsString()
            )
        return;
    }
    case EMovementNetRole::SimulatedProxy:
    {
        this->RestoreSnapshot(Snapshot);
        this->LastSimulatedSequence = Snapshot.Sequence;
        this->UpdatePresentation(0.0f);
        return;
    }
    case EMovementNetRole::AutonomousProxy:
    {
        break;
    }
    }

    if (Snapshot.Sequence < this->LastAcknowledgedSequence)
    {
        /* Out of order. A newer snapshot was already applied. */
        return;
    }
    this->LastAcknowledgedSequence = Snapshot.Sequence;

    /* Everything before the acknowledged command is no longer of any interest. */
    this->PredictedMoves.erase(this->PredictedMoves.begin(), std::ranges::find_if(this->PredictedMoves, [&Snapshot](LPredictedMove const& Move)
    {
        return Move.Input.Sequence >= Snapshot.Sequence;
    }));

    bool bCorrect{true};
    if (!this->PredictedMoves.empty() && this->PredictedMoves.front().Input.Sequence == Snapshot.Sequence)
    {
        bCorrect = this->NeedsCorrection(this->PredictedMoves.front().Result, Snapshot);
        this->PredictedMoves.erase(this->PredictedMoves.begin());
    }

    if (!bCorrect)
    {
        return;
    }

    ++this->CorrectionCount;
    LOG_VERBOSE(LogMovement, "[{}]: Prediction diverged at sequence [{}]. Replaying [{}] moves."
        , this->GetNameAsString(), Snapshot.Sequence, this->PredictedMoves.size()
        )

    EMovementMode const ModeBefore{this->Mode};
    this->bReplaying = true;

    this->RestoreSnapshot(Snapshot);
    this->LastSimulatedSequence = Snapshot.Sequence;
    for (LPredictedMove& Move: this->PredictedMoves)
    {
        this->SimulateTick(Move.Input, Move.Dt);
        this->LastSimulatedSequence = Move.Input.Sequence;
        Move.Result = this->CaptureSnapshot();
    }

    this->bReplaying = false;
    if (this->Mode != ModeBefore)
    {
        this->OnMovementModeChangedImpl(ModeBefore, this->Mode);
        this->OnMovementModeChanged.Broadcast(*this, ModeBefore, this->Mode);
    }

    this->UpdatePresentation(0.0f);
}

bool Jafg::AMovementComponent::NeedsCorrection(LMovementSnapshot const& Predicted, LMovementSnapshot const& Authoritative) const noexcept
{
    if (Predicted.Mode != Authoritative.Mode || Predicted.Flags != Authoritative.Flags)
    {
        return true;
    }
    if (maths::squared_magnitude(Predicted.Position - Authoritative.Position)
        > static_cast<LWorldReal>(maths::squared(this->CorrectionPositionTolerance)))
    {
        return true;
    }
    if (maths::squared_magnitude(Predicted.Velocity - Authoritative.Velocity) > maths::squared(this->CorrectionVelocityTolerance))
    {
        return true;
    }
    /* Both q and -q represent the same rotation. */
    if (maths::abs(glm::dot(Predicted.Rotation, Authoritative.Rotation)) < 1.0f - maths::not_so_small_number_f)
    {
        return true;
    }
    return false;
}

void Jafg::AMovementComponent::SetMovementMode(EMovementMode New)
{
    if (this->Mode == New)
    {
        return;
    }

    EMovementMode const Previous{std::exchange(this->Mode, New)};
    if (!this->bReplaying)
    {
        this->OnMovementModeChangedImpl(Previous, New);
        this->OnMovementModeChanged.Broadcast(*this, Previous, New);
    }
}

Jafg::LMovementInput Jafg::AMovementComponent::ConsumeIntent(f32 Dt) noexcept
{
    LMovementInput Result;

    Result.Sequence = this->NextSequence++;
    if (this->NextSequence == 0)
    {
        /* Zero is reserved for "no command". */
        this->NextSequence = 1;
    }

    Result.Move = this->Intent.MoveWorldDirection
        ? this->WorldDirectionToMoveAxes(*this->Intent.MoveWorldDirection)
        : this->Intent.Move;
    Result.Move = Movement::ClampMagnitude(Result.Move, 1.0f);

    Result.Look = this->Intent.PendingLook;
    if (this->Intent.LookAtDirection)
    {
        Result.Look += this->ComputeLookDeltaTowards(*this->Intent.LookAtDirection, this->Intent.LookAtTurnRate * Dt);
    }

    Result.Buttons = this->Intent.Buttons;

    this->Intent.PendingLook = LVec3F{0.0f};
    this->Intent.Buttons &= ~EMovementButtonFlags{EMovementButtonBits::JumpPressed};

    return Result;
}

void Jafg::AMovementComponent::SimulateRemoteInputs(f32 Dt)
{
    /*
     * If no command is available, the character waits for its owning client. Clients are expected to send their commands
     * redundantly, so that a single lost packet does not stall the character.
     */
    std::size_t const Count{this->RemoteInputs.size() > RemoteInputCatchUpThreshold ? 2uz : 1uz};
    for (std::size_t Idx{}; Idx < Count && !this->RemoteInputs.empty(); ++Idx)
    {
        LMovementInput const Input{this->RemoteInputs.front()};
        this->RemoteInputs.erase(this->RemoteInputs.begin());
        this->SimulateTick(Input, Dt);
        this->LastSimulatedSequence = Input.Sequence;
    }
}
