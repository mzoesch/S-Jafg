// Copyright mzoesch. All rights reserved.

#include "Framework/CharacterMovementComponent.h"
#include "Framework/Actor.h"
#include "Framework/SupremePolicies.h"
#include "Engine/World.h"
#include "PhysicsForeignCore.h"

namespace
{

using EGroundState = JPH::CharacterBase::EGroundState;

FORCEINLINE JPH::Vec3 ToJolt(LVec3F const& V) noexcept { return JPH::Vec3{V.x, V.y, V.z}; }
FORCEINLINE JPH::RVec3 ToJoltR(LWorldVec3 const& V) noexcept { return JPH::RVec3{V.x, V.y, V.z}; }
FORCEINLINE JPH::Quat ToJolt(LWorldQuat const& Q) noexcept { return JPH::Quat{Q.x, Q.y, Q.z, Q.w}; }
FORCEINLINE LVec3F FromJolt(JPH::Vec3Arg V) noexcept { return LVec3F{V.GetX(), V.GetY(), V.GetZ()}; }
FORCEINLINE LWorldVec3 FromJoltR(JPH::RVec3Arg V) noexcept { return LWorldVec3{V.GetX(), V.GetY(), V.GetZ()}; }
FORCEINLINE LWorldQuat FromJolt(JPH::QuatArg Q) noexcept { return LWorldQuat{Q.GetX(), Q.GetY(), Q.GetZ(), Q.GetW()}; }

//# Marks the user data of the inner bodies of all virtual characters.
inline constexpr u64 CharacterUserDataTag{ Jafg::Detail::CharacterBodyUserDataTag };

//#
//# Characters collide with each other through #JPH::CharacterVsCharacterCollision. Their inner bodies must therefore
//# be ignored by character queries (including the own one), or characters would collide twice with each other.
//#
class character_body_filter final : public JPH::BodyFilter
{
public:

    virtual bool ShouldCollideLocked(JPH::Body const& Body) const override
    {
        return (Body.GetUserData() & CharacterUserDataTag) == 0;
    }
};

struct query_filters final
{
    explicit query_filters(JPH::PhysicsSystem const& System) noexcept
        : BroadPhase{System.GetDefaultBroadPhaseLayerFilter(Jafg::EPhysicsLayer::Dynamic)}
        , Object{System.GetDefaultLayerFilter(Jafg::EPhysicsLayer::Dynamic)}
    {
    }

    JPH::DefaultBroadPhaseLayerFilter BroadPhase;
    JPH::DefaultObjectLayerFilter Object;
    character_body_filter Body;
    JPH::ShapeFilter Shape;
};

//#
//# A capsule whose origin is at its bottom (the feet).
//# @param RadiusFraction Shrinks the radius but keeps the height of the cylinder (used for inner bodies).
//#
NODISCARD JPH::RefConst<JPH::Shape> MakeCapsuleShape(f32 Height, f32 Radius, f32 RadiusFraction)
{
    f32 const HalfCylinder{maths::max(0.5f * Height - Radius, 0.01f)};
    JPH::RotatedTranslatedShapeSettings const Settings{
          JPH::Vec3{0.0f, 0.5f * Height, 0.0f}
        , JPH::Quat::sIdentity()
        , new JPH::CapsuleShape{HalfCylinder, Radius * RadiusFraction}
        };
    JPH::ShapeSettings::ShapeResult const Result{Settings.Create()};
    if (Result.HasError())
    {
        LOG_FATAL(LogMovement, "Failed to create a capsule with a height of [{}] and a radius of [{}]: {}."
            , Height, Radius, Result.GetError().c_str()
            )
    }
    return Result.Get();
}

//#
//# Bodies that are free to rotate in all three axes. All other modes keep the body upright.
//# Frozen bodies take whatever rotation they are given (e.g., the seat of a vehicle).
//#
NODISCARD constexpr bool IsFreeRotationMode(Jafg::EMovementMode Mode) noexcept
{
    return Mode == Jafg::EMovementMode::ZeroG || Mode == Jafg::EMovementMode::None;
}

//# The magnetic boots attach to surfaces that are at most this far (cosine) away from the feet (~110 degrees).
inline constexpr f32 MagBootsMinSurfaceDot{ -0.34f };
//# Relative speed along up above which a character is considered to move away from the ground (e.g., jumping).
inline constexpr f32 SeparationSpeed{ 0.1f };

} /* ~Namespace <Anonymous> */

namespace Jafg::Detail
{

struct LCharacterMovementProxy final : public JPH::CharacterContactListener
{
    explicit LCharacterMovementProxy(LPhysicsSubsystem& InSubsystem) noexcept : Subsystem{InSubsystem} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LCharacterMovementProxy)

    virtual ~LCharacterMovementProxy() override
    {
        check(Tasks::IsOnMasterThread())
        if (this->Character)
        {
            this->Subsystem.CharacterVsCharacterCollision.Remove(this->Character);
            this->Character->SetCharacterVsCharacterCollision(nullptr);
            this->Character->SetListener(nullptr);
            /* Removes and destroys the inner body. */
            this->Character = nullptr;
        }
    }

    virtual void OnContactAdded(JPH::CharacterVirtual const*, JPH::CharacterContact const&, JPH::CharacterContactSettings& Settings) override
    {
        this->ApplyContactSettings(Settings);
    }

    virtual void OnContactPersisted(JPH::CharacterVirtual const*, JPH::CharacterContact const&, JPH::CharacterContactSettings& Settings) override
    {
        this->ApplyContactSettings(Settings);
    }

    void ApplyContactSettings(JPH::CharacterContactSettings& Settings) const noexcept
    {
        Settings.mCanPushCharacter = this->bCanBePushedByBodies;
        Settings.mCanReceiveImpulses = this->bCanPushBodies;
    }

    //#
    //# Removes the character from the world while disabled: The inner body leaves the broad phase, and other
    //# characters no longer collide with it.
    //#
    void SetCollisionEnabled(bool bEnabled) noexcept
    {
        check(Tasks::IsOnMasterThread())
        if (this->bCollisionEnabled == bEnabled)
        {
            return;
        }
        this->bCollisionEnabled = bEnabled;

        JPH::BodyInterface& Interface{this->Subsystem.PhysicsSystem.GetBodyInterfaceNoLock()};
        JPH::BodyID const Inner{this->Character->GetInnerBodyID()};
        if (bEnabled)
        {
            if (!Inner.IsInvalid() && !Interface.IsAdded(Inner))
            {
                Interface.AddBody(Inner, JPH::EActivation::Activate);
            }
            this->Subsystem.CharacterVsCharacterCollision.Add(this->Character);
        }
        else
        {
            /* The destructor of the character removes the inner body again. That is fine for removed bodies. */
            Interface.RemoveBody(Inner);
            this->Subsystem.CharacterVsCharacterCollision.Remove(this->Character);
        }
    }

    LPhysicsSubsystem& Subsystem;
    JPH::Ref<JPH::CharacterVirtual> Character;
    JPH::RefConst<JPH::Shape> StandingShape;
    JPH::RefConst<JPH::Shape> CrouchingShape;
    JPH::RefConst<JPH::Shape> StandingInnerShape;
    JPH::RefConst<JPH::Shape> CrouchingInnerShape;
    bool bCanPushBodies{ true };
    bool bCanBePushedByBodies{ true };
    bool bCollisionEnabled{ true };
};

void DestroyCharacterMovementProxy(LCharacterMovementProxy* Proxy) noexcept
{
    delete Proxy;
}

} /* ~Namespace Jafg::Detail */

void Jafg::ACharacterMovementComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    this->CreateCharacter();
}

void Jafg::ACharacterMovementComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    this->DestroyCharacter();
    Super::OnGarbage(Reason);
}

LVec3F Jafg::ACharacterMovementComponent::GetVelocity() const noexcept
{
    return this->Proxy ? ::FromJolt(this->Proxy->Character->GetLinearVelocity()) : LVec3F{0.0f};
}

LVec3F Jafg::ACharacterMovementComponent::GetUp() const noexcept
{
    return this->Proxy ? ::FromJolt(this->Proxy->Character->GetUp()) : maths::up_vector<LVec3F>;
}

void Jafg::ACharacterMovementComponent::Teleport(LWorldVec3 const& Location, std::optional<LWorldQuat> Rotation /* = {} */)
{
    if (!this->Proxy)
    {
        auto& Root{this->GetOwningActor().GetRootComponent()};
        Root.SetLocalTranslationByTeleport(Location);
        if (Rotation)
        {
            Root.SetLocalRotatorByTeleport(*Rotation);
        }
        return;
    }

    auto& Character{*this->Proxy->Character};
    Character.SetPosition(::ToJoltR(Location));
    if (Rotation)
    {
        LWorldQuat const Normalized{maths::normalize(*Rotation)};
        if (::IsFreeRotationMode(this->GetMovementMode()))
        {
            Character.SetRotation(::ToJolt(Normalized));
            Character.SetUp(::ToJolt(Normalized * maths::up_vector<LVec3F>));
        }
        else
        {
            Character.SetRotation(::ToJolt(Movement::MakeUpright(Normalized, ::FromJolt(Character.GetUp()))));
        }
    }

    query_filters const Filters{this->Proxy->Subsystem.PhysicsSystem};
    Character.RefreshContacts(Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape, this->Proxy->Subsystem.TempAllocator);

    this->UpdatePresentation(0.0f);
}

void Jafg::ACharacterMovementComponent::SetVelocity(LVec3F const& Velocity)
{
    if (this->Proxy)
    {
        this->Proxy->Character->SetLinearVelocity(::ToJolt(Velocity));
    }
}

Jafg::LMovementSnapshot Jafg::ACharacterMovementComponent::CaptureSnapshot() const
{
    LMovementSnapshot Result;
    Result.Sequence = this->GetLastSimulatedSequence();
    Result.Mode = this->GetMovementMode();
    Result.Flags = this->StateFlags;
    if (this->Proxy)
    {
        auto const& Character{*this->Proxy->Character};
        Result.Position = ::FromJoltR(Character.GetPosition());
        Result.Rotation = ::FromJolt(Character.GetRotation());
        Result.Velocity = ::FromJolt(Character.GetLinearVelocity());
        Result.Up = ::FromJolt(Character.GetUp());
    }
    Result.ViewPitch = this->ViewPitch;
    Result.CoyoteTime = this->CoyoteTimeRemaining;
    Result.JumpBuffer = this->JumpBufferRemaining;
    Result.MagBootsCooldown = this->MagBootsCooldownRemaining;
    Result.BaseBodyId = this->BaseBodyId;
    Result.BaseRotation = this->BaseRotation;
    Result.Custom = this->CustomState;
    return Result;
}

bool Jafg::ACharacterMovementComponent::IsOnGround() const noexcept
{
    return this->Proxy && this->Proxy->Character->GetGroundState() == EGroundState::OnGround;
}

LVec3F Jafg::ACharacterMovementComponent::GetGroundNormal() const noexcept
{
    if (!this->Proxy || this->Proxy->Character->GetGroundState() == EGroundState::InAir)
    {
        return LVec3F{0.0f};
    }
    return ::FromJolt(this->Proxy->Character->GetGroundNormal());
}

LVec3F Jafg::ACharacterMovementComponent::GetGroundVelocity() const noexcept
{
    if (!this->Proxy || this->Proxy->Character->GetGroundState() == EGroundState::InAir)
    {
        return LVec3F{0.0f};
    }
    return ::FromJolt(this->Proxy->Character->GetGroundVelocity());
}

LWorldQuat Jafg::ACharacterMovementComponent::GetBodyRotation() const noexcept
{
    return this->Proxy ? ::FromJolt(this->Proxy->Character->GetRotation()) : this->GetOwningActor().GetRootComponent().GetLocalRotator();
}

void Jafg::ACharacterMovementComponent::SetCapsuleDimensions(f32 InStandingHeight, f32 InCrouchingHeight, f32 InRadius)
{
    if (InRadius < 0.05f || InStandingHeight < 2.0f * InRadius + 0.05f || InCrouchingHeight < 2.0f * InRadius + 0.05f
        || InCrouchingHeight > InStandingHeight)
    {
        LOG_ERROR(LogMovement, "[{}]: Invalid capsule dimensions. Standing height [{}], crouching height [{}], radius [{}]."
            , this->GetNameAsString(), InStandingHeight, InCrouchingHeight, InRadius
            )
        return;
    }

    this->StandingHeight = InStandingHeight;
    this->CrouchingHeight = InCrouchingHeight;
    this->CapsuleRadius = InRadius;

    if (this->Proxy)
    {
        auto& P{*this->Proxy};
        this->RebuildShapes();
        P.Character->SetSupportingVolume(JPH::Plane{JPH::Vec3::sAxisY(), -this->CapsuleRadius});

        /* The old shapes are gone. Force the new ones. */
        bool const bCrouched{this->IsCrouched()};
        query_filters const Filters{P.Subsystem.PhysicsSystem};
        (void)P.Character->SetShape(
              bCrouched ? P.CrouchingShape.GetPtr() : P.StandingShape.GetPtr()
            , std::numeric_limits<f32>::max()
            , Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape
            , P.Subsystem.TempAllocator
            );
        P.Character->SetInnerBodyShape(bCrouched ? P.CrouchingInnerShape.GetPtr() : P.StandingInnerShape.GetPtr());
    }
}

void Jafg::ACharacterMovementComponent::SimulateTick(LMovementInput const& Input, f32 Dt)
{
    STAT_FUNCTION()

    if (!this->Proxy || Dt <= 0.0f || this->GetMovementMode() == EMovementMode::None)
    {
        return;
    }

    auto& Character{*this->Proxy->Character};
    auto& Subsystem{this->Proxy->Subsystem};
    query_filters const Filters{Subsystem.PhysicsSystem};

    this->SyncExternalChanges();
    this->ApplyRuntimeSettings();

    EMovementMode const Previous{this->GetMovementMode()};

    //# ---------- States and timers ----------

    /* States are owned by the controlling entity. */
    this->StateFlags &= ~(EMovementStateBits::Jetpack
        | EMovementStateBits::Dampeners
        | EMovementStateBits::MagBoots
        | EMovementStateBits::JustJumped
        );
    if (Input.Has(EMovementButtonBits::Jetpack))
    {
        this->StateFlags |= EMovementStateBits::Jetpack;
    }
    if (Input.Has(EMovementButtonBits::Dampeners))
    {
        this->StateFlags |= EMovementStateBits::Dampeners;
    }
    if (Input.Has(EMovementButtonBits::MagBoots))
    {
        this->StateFlags |= EMovementStateBits::MagBoots;
    }
    bool const bJetpack{Input.Has(EMovementButtonBits::Jetpack)};
    bool const bDampeners{Input.Has(EMovementButtonBits::Dampeners)};

    this->CoyoteTimeRemaining = maths::max(this->CoyoteTimeRemaining - Dt, 0.0f);
    this->JumpBufferRemaining = maths::max(this->JumpBufferRemaining - Dt, 0.0f);
    this->MagBootsCooldownRemaining = maths::max(this->MagBootsCooldownRemaining - Dt, 0.0f);
    if (Input.Has(EMovementButtonBits::JumpPressed))
    {
        this->JumpBufferRemaining = maths::max(this->JumpBufferTime, Dt);
    }

    //# ---------- Environment ----------

    LVec3F const Gravity{this->ComputeGravity(::FromJoltR(Character.GetPosition()))};
    this->LastGravity = Gravity;
    f32 const GravityMagnitude{maths::magnitude(Gravity)};
    bool const bWeightless{GravityMagnitude < this->ZeroGravityThreshold};
    bool const bMagBoots{Input.Has(EMovementButtonBits::MagBoots)
        && GravityMagnitude <= this->MagBootsMaxGravity
        && this->MagBootsCooldownRemaining <= 0.0f
        };

    //# ---------- Orientation ----------

    this->FollowBase();
    this->ApplyLook(Input.Look, Previous);

    std::optional<LSurface> const Surface{bMagBoots ? this->FindMagneticSurface() : std::nullopt};
    auto AlignUp{[&](LVec3F const& Target, f32 MaxAngle)
    {
        LVec3F const OldUp{::FromJolt(Character.GetUp())};
        LVec3F const NewUp{Movement::RotateTowards(OldUp, Target, MaxAngle)};
        if (maths::dot(OldUp, NewUp) < 1.0f - maths::small_number_f)
        {
            LWorldQuat const Delta{Movement::FromToRotation(OldUp, NewUp)};
            if (IsGroundedMovementMode(Previous))
            {
                /* Keep the feet planted. */
                Character.SetRotation(::ToJolt(maths::normalize(Delta * ::FromJolt(Character.GetRotation()))));
            }
            else
            {
                this->RotateBodyAroundEye(Delta);
            }
        }
        Character.SetUp(::ToJolt(NewUp));
    }};
    if (Surface)
    {
        AlignUp(Surface->Normal, this->MagBootsAlignRate * Dt);
    }
    else if (!bWeightless)
    {
        AlignUp(-Gravity / GravityMagnitude, this->UpAlignmentRate * Dt);
    }
    else
    {
        /* Weightless bodies define their up themselves. */
        Character.SetUp(::ToJolt(::FromJolt(Character.GetRotation()) * maths::up_vector<LVec3F>));
    }

    //# ---------- Mode ----------

    Character.UpdateGroundVelocity();
    LVec3F const Up{::FromJolt(Character.GetUp())};
    EGroundState const GroundState{Character.GetGroundState()};
    LVec3F const GroundVelocity{GroundState != EGroundState::InAir ? ::FromJolt(Character.GetGroundVelocity()) : LVec3F{0.0f}};
    LVec3F Velocity{::FromJolt(Character.GetLinearVelocity())};
    bool const bMovingAwayFromGround{maths::dot(Velocity - GroundVelocity, Up) > ::SeparationSpeed};

    EMovementMode Mode;
    if (Surface && Surface->bTouching && !bMovingAwayFromGround)
    {
        Mode = EMovementMode::MagWalking;
    }
    else if (bWeightless)
    {
        Mode = EMovementMode::ZeroG;
    }
    else if (GroundState == EGroundState::OnGround && !bMovingAwayFromGround)
    {
        Mode = EMovementMode::Walking;
    }
    else if (bJetpack)
    {
        Mode = EMovementMode::Flying;
    }
    else
    {
        Mode = EMovementMode::Falling;
    }

    if (Previous == EMovementMode::Walking && !IsGroundedMovementMode(Mode))
    {
        this->CoyoteTimeRemaining = this->CoyoteTime;
    }
    if (::IsFreeRotationMode(Mode) && !::IsFreeRotationMode(Previous))
    {
        this->FoldViewPitchIntoBody();
    }
    this->SetMovementMode(Mode);

    //# ---------- Crouching ----------

    bool const bWantsCrouch{Input.Has(EMovementButtonBits::Crouch)
        && (Mode == EMovementMode::Walking || Mode == EMovementMode::Falling || Mode == EMovementMode::MagWalking)
        };
    if (bWantsCrouch != this->IsCrouched())
    {
        (void)this->TrySetCrouched(bWantsCrouch, false);
    }

    //# ---------- Velocity ----------

    f32 const Efficiency{maths::clamp(this->GetThrusterEfficiency(Input, Dt), 0.0f, 1.0f)};
    f32 const VerticalAxis{maths::clamp(Input.Move.y
        + (Input.Has(EMovementButtonBits::JumpHeld) ? 1.0f : 0.0f)
        - (Input.Has(EMovementButtonBits::Crouch) ? 1.0f : 0.0f)
        , -1.0f, 1.0f)
        };
    LVec3F UpdateGravity{0.0f};
    f32 ThrustDeltaVelocity{};
    bool bJumped{};

    switch (Mode)
    {
    case EMovementMode::Walking:
    case EMovementMode::MagWalking:
    {
        bool const bMagnetic{Mode == EMovementMode::MagWalking};

        f32 Speed{this->WalkSpeed};
        if (bMagnetic)
        {
            Speed = this->MagBootsWalkSpeed;
        }
        else if (this->IsCrouched())
        {
            Speed = this->CrouchSpeed;
        }
        else if (Input.Has(EMovementButtonBits::Sprint) && Input.Move.z > 0.1f)
        {
            Speed = this->SprintSpeed;
        }

        LVec3F const Wish{this->GetFlatWishDirection(Input.Move)};
        LVec3F const Relative{Movement::ProjectOnPlane(Velocity - GroundVelocity, Up)};
        f32 const Rate{maths::squared_magnitude(Wish) > 0.0f ? this->GroundAcceleration : this->GroundBraking};
        Velocity = GroundVelocity + Movement::MoveTowards(Relative, Wish * Speed, Rate * Dt);

        if (this->JumpBufferRemaining > 0.0f)
        {
            if (bMagnetic)
            {
                Velocity += Up * this->MagBootsPushOffSpeed;
                this->MagBootsCooldownRemaining = this->MagBootsReattachDelay;
            }
            else
            {
                Velocity = Movement::ProjectOnPlane(Velocity, Up) + Up * (maths::dot(GroundVelocity, Up) + this->JumpSpeed);
            }
            bJumped = true;
        }

        UpdateGravity = bMagnetic ? -Up * this->MagBootsAdhesion : Gravity;
        Velocity += UpdateGravity * Dt;
        break;
    }
    case EMovementMode::Falling:
    {
        LVec3F const Vertical{Up * maths::dot(Velocity, Up)};
        LVec3F const Wish{this->GetFlatWishDirection(Input.Move)};
        Velocity = Vertical + Movement::Accelerate(
              Velocity - Vertical
            , Movement::NormalizeOr(Wish, LVec3F{0.0f})
            , maths::magnitude(Wish) * this->AirSpeed
            , this->AirAcceleration
            , Dt
            );
        if (GroundState == EGroundState::OnSteepGround)
        {
            /* Do not climb slopes that are too steep with air control. */
            Velocity = ::FromJolt(Character.CancelVelocityTowardsSteepSlopes(::ToJolt(Velocity)));
        }

        if (this->JumpBufferRemaining > 0.0f && this->CoyoteTimeRemaining > 0.0f)
        {
            Velocity = Movement::ProjectOnPlane(Velocity, Up) + Up * this->JumpSpeed;
            bJumped = true;
        }

        Velocity += Gravity * Dt;
        if (this->FallingLinearDrag > 0.0f)
        {
            Velocity *= maths::max(1.0f - this->FallingLinearDrag * Dt, 0.0f);
        }
        UpdateGravity = Gravity;
        break;
    }
    case EMovementMode::Flying:
    {
        /* Jumping is thrusting while flying. */
        this->JumpBufferRemaining = 0.0f;

        LVec3F const Wish{Movement::ClampMagnitude(this->GetFlatWishDirection(Input.Move) + Up * VerticalAxis, 1.0f)};
        LVec3F const Direction{Movement::NormalizeOr(Wish, LVec3F{0.0f})};
        LVec3F const Reference{this->GetReferenceFrameVelocity()};
        LVec3F const Before{Velocity - Reference + Gravity * Dt};
        LVec3F Relative{Movement::Accelerate(Before, Direction, maths::magnitude(Wish) * this->JetpackMaxSpeed, this->JetpackAcceleration * Efficiency, Dt)};
        if (bDampeners)
        {
            /* Engaged dampeners hover and brake all motion that is not requested. */
            Relative = Movement::Dampen(Relative, Direction, this->JetpackDampenerDeceleration * Efficiency, Dt);
        }
        ThrustDeltaVelocity = maths::magnitude(Relative - Before);
        Velocity = Relative + Reference;
        UpdateGravity = Gravity;
        break;
    }
    case EMovementMode::ZeroG:
    {
        /* Jumping is thrusting while weightless. */
        this->JumpBufferRemaining = 0.0f;

        LWorldQuat const Body{::FromJolt(Character.GetRotation())};
        LVec3F const Axes{Movement::ClampMagnitude(LVec3F{Input.Move.x, VerticalAxis, Input.Move.z}, 1.0f)};
        LVec3F const Wish{Body * Movement::MoveAxesToBody(Axes)};
        LVec3F const Direction{Movement::NormalizeOr(Wish, LVec3F{0.0f})};
        LVec3F const Reference{this->GetReferenceFrameVelocity()};
        LVec3F const Before{Velocity - Reference};
        LVec3F Relative{Movement::Accelerate(Before, Direction, maths::magnitude(Wish) * this->EvaMaxSpeed, this->EvaAcceleration * Efficiency, Dt)};
        if (bDampeners)
        {
            Relative = Movement::Dampen(Relative, Direction, this->EvaDampenerDeceleration * Efficiency, Dt);
        }
        ThrustDeltaVelocity = maths::magnitude(Relative - Before);
        if (Surface)
        {
            /* The boots pull towards a surface in reach. */
            Relative -= Surface->Normal * (this->MagBootsAdhesion * Dt);
        }
        Velocity = Relative + Reference;
        break;
    }
    case EMovementMode::None:
    case EMovementMode::Custom:
    {
        break;
    }
    }

    if (bJumped)
    {
        this->JumpBufferRemaining = 0.0f;
        this->CoyoteTimeRemaining = 0.0f;
        this->StateFlags |= EMovementStateBits::JustJumped;
        if (!this->IsReplaying())
        {
            this->OnJumped();
        }
    }
    if (ThrustDeltaVelocity > 0.0f)
    {
        this->OnThrustersUsed(ThrustDeltaVelocity, Dt);
    }

    //# ---------- Collide and slide ----------

    Character.SetLinearVelocity(::ToJolt(Velocity));

    bool const bStickToFloor{IsGroundedMovementMode(Mode) && !bJumped};
    JPH::CharacterVirtual::ExtendedUpdateSettings UpdateSettings;
    UpdateSettings.mStickToFloorStepDown = bStickToFloor ? ::ToJolt(-Up * this->StickToFloorDistance) : JPH::Vec3::sZero();
    UpdateSettings.mWalkStairsStepUp = bStickToFloor ? ::ToJolt(Up * this->MaxStepHeight) : JPH::Vec3::sZero();
    Character.ExtendedUpdate(Dt, ::ToJolt(UpdateGravity), UpdateSettings
        , Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape, Subsystem.TempAllocator);

    /*
     * The virtual character slides along contacts but keeps the requested velocity. Remove everything that moves into
     * contacts (relative to their own motion), or the character would stick to ceilings and hulls.
     */
    LVec3F Constrained{::FromJolt(Character.GetLinearVelocity())};
    for (JPH::CharacterContact const& Contact: Character.GetActiveContacts())
    {
        if (!Contact.mHadCollision || Contact.mWasDiscarded || Contact.mIsSensorB)
        {
            continue;
        }
        LVec3F const Normal{::FromJolt(Contact.mContactNormal)};
        f32 const Into{maths::dot(Constrained - ::FromJolt(Contact.mLinearVelocity), Normal)};
        if (Into < 0.0f)
        {
            Constrained -= Normal * Into;
        }
    }
    Character.SetLinearVelocity(::ToJolt(Constrained));

    //# ---------- Transitions ----------

    EGroundState const NewGroundState{Character.GetGroundState()};
    LVec3F const NewGroundVelocity{NewGroundState != EGroundState::InAir ? ::FromJolt(Character.GetGroundVelocity()) : LVec3F{0.0f}};
    bool const bNowMovingAway{bJumped || maths::dot(::FromJolt(Character.GetLinearVelocity()) - NewGroundVelocity, Up) > ::SeparationSpeed};
    bool const bNowOnGround{NewGroundState == EGroundState::OnGround && !bNowMovingAway};
    bool const bNowTouching{NewGroundState != EGroundState::InAir && !bNowMovingAway};

    EMovementMode Next{Mode};
    switch (Mode)
    {
    case EMovementMode::Walking:
    {
        if (!bNowOnGround)
        {
            Next = bJetpack ? EMovementMode::Flying : EMovementMode::Falling;
        }
        break;
    }
    case EMovementMode::MagWalking:
    {
        if (!bNowTouching)
        {
            Next = bWeightless ? EMovementMode::ZeroG : bJetpack ? EMovementMode::Flying : EMovementMode::Falling;
        }
        break;
    }
    case EMovementMode::Falling:
    case EMovementMode::Flying:
    {
        if (bNowOnGround)
        {
            Next = EMovementMode::Walking;
        }
        break;
    }
    case EMovementMode::ZeroG:
    {
        if (bMagBoots && bNowTouching)
        {
            if (auto const NewSurface{this->FindMagneticSurface()}; NewSurface && NewSurface->bTouching)
            {
                Next = EMovementMode::MagWalking;
            }
        }
        break;
    }
    case EMovementMode::None:
    case EMovementMode::Custom:
    {
        break;
    }
    }

    if (Next != Mode)
    {
        if (Mode == EMovementMode::Walking && !bJumped)
        {
            this->CoyoteTimeRemaining = this->CoyoteTime;
        }
        if (!IsGroundedMovementMode(Mode) && IsGroundedMovementMode(Next))
        {
            this->CoyoteTimeRemaining = 0.0f;
            if (!this->IsReplaying())
            {
                this->OnLanded(Velocity - NewGroundVelocity);
            }
        }
        if (::IsFreeRotationMode(Next) && !::IsFreeRotationMode(Mode))
        {
            this->FoldViewPitchIntoBody();
        }
        this->SetMovementMode(Next);
    }
}

void Jafg::ACharacterMovementComponent::RestoreSnapshot(LMovementSnapshot const& Snapshot)
{
    if (!this->Proxy)
    {
        return;
    }

    auto& Character{*this->Proxy->Character};
    Character.SetPosition(::ToJoltR(Snapshot.Position));
    Character.SetRotation(::ToJolt(maths::normalize(Snapshot.Rotation)));
    Character.SetUp(::ToJolt(Movement::NormalizeOr(Snapshot.Up, maths::up_vector<LVec3F>)));
    Character.SetLinearVelocity(::ToJolt(Snapshot.Velocity));

    (void)this->TrySetCrouched(Snapshot.Has(EMovementStateBits::Crouched), true);
    this->StateFlags = Snapshot.Flags;
    this->ViewPitch = Snapshot.ViewPitch;
    this->CoyoteTimeRemaining = Snapshot.CoyoteTime;
    this->JumpBufferRemaining = Snapshot.JumpBuffer;
    this->MagBootsCooldownRemaining = Snapshot.MagBootsCooldown;
    this->BaseBodyId = Snapshot.BaseBodyId;
    this->BaseRotation = Snapshot.BaseRotation;
    this->CustomState = Snapshot.Custom;

    query_filters const Filters{this->Proxy->Subsystem.PhysicsSystem};
    Character.RefreshContacts(Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape, this->Proxy->Subsystem.TempAllocator);

    this->SetMovementMode(Snapshot.Mode);
}

void Jafg::ACharacterMovementComponent::UpdatePresentation(f32 Dt)
{
    if (!this->Proxy || !this->IsOwningPawnValid())
    {
        return;
    }

    this->SyncExternalChanges();
    this->Proxy->SetCollisionEnabled(this->GetMovementMode() != EMovementMode::None);

    auto& Pawn{this->GetOwningPawn()};
    auto& Root{Pawn.GetRootComponent()};
    auto const& Character{*this->Proxy->Character};
    bool const bFreeRotation{::IsFreeRotationMode(this->GetMovementMode())};

    LWorldVec3 Position{::FromJoltR(Character.GetPosition())};
    LWorldQuat Rotation{::FromJolt(Character.GetRotation())};
    f32 Pitch{this->ViewPitch};
    f32 const EyeHeight{this->GetSimulatedEyeHeight()};

    /* Show the look input that was not yet simulated, so that the view reacts instantly. */
    if (LVec3F const& Pending{this->GetPendingLook()}; Pending != LVec3F{0.0f})
    {
        if (bFreeRotation)
        {
            LVec3F const EyeOffset{0.0f, EyeHeight, 0.0f};
            LWorldVec3 const Eye{Position + LWorldVec3{Rotation * EyeOffset}};
            Rotation = maths::normalize(Rotation * Movement::LookDeltaToRotation(Pending));
            Position = Eye - LWorldVec3{Rotation * EyeOffset};
        }
        else
        {
            f32 const MaxPitch{maths::radians(maths::clamp(this->MaxViewPitch, 0.0f, 89.9f))};
            Rotation = maths::normalize(maths::angle_axis(-Pending.x, ::FromJolt(Character.GetUp())) * Rotation);
            Pitch = maths::clamp(Pitch + Pending.y, -MaxPitch, MaxPitch);
        }
    }

    if (this->PresentedEyeHeight < 0.0f)
    {
        this->PresentedEyeHeight = EyeHeight;
    }
    else if (Dt > 0.0f)
    {
        this->PresentedEyeHeight += (EyeHeight - this->PresentedEyeHeight) * (1.0f - std::exp(-this->EyeHeightBlendSpeed * Dt));
    }

    if (!this->bHasWrittenRoot || Root.GetLocalTranslation() != Position)
    {
        Root.SetLocalTranslationByTeleport(Position);
    }
    if (!this->bHasWrittenRoot || Root.GetLocalRotator() != Rotation)
    {
        Root.SetLocalRotatorByTeleport(Rotation);
    }
    this->LastWrittenTranslation = Position;
    this->LastWrittenRotation = Rotation;
    this->bHasWrittenRoot = true;

    Pawn.SetViewLocalOffset(LWorldVec3{0.0f, this->PresentedEyeHeight, 0.0f});
    Pawn.SetViewLocalRotation(bFreeRotation ? maths::identity<LWorldQuat> : Movement::PitchToRotation(Pitch));
}

LVec3F Jafg::ACharacterMovementComponent::WorldDirectionToMoveAxes(LVec3F const& Direction) const noexcept
{
    return Movement::BodyToMoveAxes(glm::inverse(this->GetBodyRotation()) * Direction);
}

LVec3F Jafg::ACharacterMovementComponent::ComputeLookDeltaTowards(LVec3F const& Direction, f32 MaxAngle) const noexcept
{
    LVec3F const Local{glm::inverse(this->GetBodyRotation()) * Direction};

    /* Right is -X in body space. Positive yaw turns right. */
    f32 const Yaw{std::atan2(-Local.x, Local.z)};
    f32 Pitch;
    if (::IsFreeRotationMode(this->GetMovementMode()))
    {
        Pitch = std::atan2(Local.y, std::sqrt(Local.x * Local.x + Local.z * Local.z));
    }
    else
    {
        f32 const MaxPitch{maths::radians(maths::clamp(this->MaxViewPitch, 0.0f, 89.9f))};
        Pitch = maths::clamp(std::asin(maths::clamp(Local.y, -1.0f, 1.0f)), -MaxPitch, MaxPitch) - this->ViewPitch;
    }

    return LVec3F{
        maths::clamp(Yaw, -MaxAngle, MaxAngle),
        maths::clamp(Pitch, -MaxAngle, MaxAngle),
        0.0f,
        };
}

LVec3F Jafg::ACharacterMovementComponent::ComputeGravity(LWorldVec3 const& Location) const
{
    if (auto const* Policies{this->GetWorld().GetSupremePolicies()})
    {
        return Policies->GetGravityAt(Location);
    }
    return this->Proxy ? ::FromJolt(this->Proxy->Subsystem.PhysicsSystem.GetGravity()) : LVec3F{0.0f};
}

void Jafg::ACharacterMovementComponent::CreateCharacter()
{
    check(Tasks::IsOnMasterThread())
    check(!this->Proxy)

    auto& Root{this->GetOwningActor().GetRootComponent()};
    auto& Subsystem{Root.GetParentPhysicsSystem()._GetSubsystem()};

    this->Proxy = LProxyPtr{new Detail::LCharacterMovementProxy{Subsystem}, &Detail::DestroyCharacterMovementProxy};
    this->RebuildShapes();

    LWorldVec3 const Position{Root.GetLocalTranslation()};
    LWorldQuat const RootRotation{maths::normalize(Root.GetLocalRotator())};
    LVec3F const Gravity{this->ComputeGravity(Position)};
    f32 const GravityMagnitude{maths::magnitude(Gravity)};
    bool const bWeightless{GravityMagnitude < this->ZeroGravityThreshold};
    LVec3F const Up{bWeightless ? RootRotation * maths::up_vector<LVec3F> : -Gravity / GravityMagnitude};
    LWorldQuat const Rotation{bWeightless ? RootRotation : Movement::MakeUpright(RootRotation, Up)};

    JPH::Ref<JPH::CharacterVirtualSettings> const Settings{new JPH::CharacterVirtualSettings{}};
    Settings->mUp = ::ToJolt(Up);
    Settings->mMaxSlopeAngle = maths::radians(maths::clamp(this->MaxSlopeAngle, 0.0f, 89.0f));
    Settings->mShape = this->Proxy->StandingShape;
    Settings->mMass = maths::max(this->Mass, 1.0f);
    Settings->mMaxStrength = maths::max(this->MaxPushForce, 0.0f);
    /* Only contacts with the lower hemisphere support the character. */
    Settings->mSupportingVolume = JPH::Plane{JPH::Vec3::sAxisY(), -this->CapsuleRadius};
    /* Procedural meshes are full of internal edges. */
    Settings->mEnhancedInternalEdgeRemoval = true;
    Settings->mInnerBodyShape = this->Proxy->StandingInnerShape;
    Settings->mInnerBodyLayer = EPhysicsLayer::Dynamic;

    u64 const UserData{::CharacterUserDataTag | static_cast<u64>(reinterpret_cast<std::uintptr_t>(this))};
    auto& Character{*(this->Proxy->Character = new JPH::CharacterVirtual{
        Settings, ::ToJoltR(Position), ::ToJolt(Rotation), UserData, &Subsystem.PhysicsSystem})};
    Character.SetListener(&*this->Proxy);
    Character.SetCharacterVsCharacterCollision(&Subsystem.CharacterVsCharacterCollision);
    Subsystem.CharacterVsCharacterCollision.Add(&Character);

    this->ApplyRuntimeSettings();
    query_filters const Filters{Subsystem.PhysicsSystem};
    Character.RefreshContacts(Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape, Subsystem.TempAllocator);

    this->LastGravity = Gravity;
    this->StateFlags = {};
    this->ViewPitch = 0.0f;
    if (bWeightless)
    {
        this->SetMovementMode(EMovementMode::ZeroG);
    }
    else
    {
        this->SetMovementMode(this->IsOnGround() ? EMovementMode::Walking : EMovementMode::Falling);
    }

    LOG_VERBOSE(LogMovement, "[{}]: Created character for [{}] in mode [{}]."
        , this->GetNameAsString(), this->GetOwningActor().GetNameAsString(), serde::to_string(this->GetMovementMode())
        )

    this->bHasWrittenRoot = false;
    this->PresentedEyeHeight = -1.0f;
    this->UpdatePresentation(0.0f);
}

void Jafg::ACharacterMovementComponent::DestroyCharacter() noexcept
{
    this->Proxy.reset();
}

void Jafg::ACharacterMovementComponent::RebuildShapes()
{
    check(this->Proxy)
    auto& P{*this->Proxy};
    P.StandingShape = ::MakeCapsuleShape(this->StandingHeight, this->CapsuleRadius, 1.0f);
    P.CrouchingShape = ::MakeCapsuleShape(this->CrouchingHeight, this->CapsuleRadius, 1.0f);
    /* Slightly smaller, so that the inner body does not interfere with the collide and slide of the character. */
    P.StandingInnerShape = ::MakeCapsuleShape(this->StandingHeight, this->CapsuleRadius, 0.9f);
    P.CrouchingInnerShape = ::MakeCapsuleShape(this->CrouchingHeight, this->CapsuleRadius, 0.9f);
}

bool Jafg::ACharacterMovementComponent::TrySetCrouched(bool bCrouch, bool bForce)
{
    check(this->Proxy)
    auto& P{*this->Proxy};
    auto& Character{*P.Character};

    JPH::Shape const* Target{bCrouch ? P.CrouchingShape.GetPtr() : P.StandingShape.GetPtr()};
    if (Character.GetShape() != Target)
    {
        auto& System{P.Subsystem.PhysicsSystem};
        query_filters const Filters{System};
        /* Shrinking always works. Growing must not get stuck in geometry. */
        f32 const MaxPenetration{bCrouch || bForce
            ? std::numeric_limits<f32>::max()
            : 1.5f * System.GetPhysicsSettings().mPenetrationSlop
            };
        if (!Character.SetShape(Target, MaxPenetration, Filters.BroadPhase, Filters.Object, Filters.Body, Filters.Shape, P.Subsystem.TempAllocator))
        {
            return false;
        }
        Character.SetInnerBodyShape(bCrouch ? P.CrouchingInnerShape.GetPtr() : P.StandingInnerShape.GetPtr());
    }

    if (bCrouch)
    {
        this->StateFlags |= EMovementStateBits::Crouched;
    }
    else
    {
        this->StateFlags &= ~EMovementStateFlags{EMovementStateBits::Crouched};
    }
    return true;
}

void Jafg::ACharacterMovementComponent::ApplyRuntimeSettings() noexcept
{
    check(this->Proxy)
    auto& Character{*this->Proxy->Character};
    Character.SetMaxSlopeAngle(maths::radians(maths::clamp(this->MaxSlopeAngle, 0.0f, 89.0f)));
    Character.SetMass(maths::max(this->Mass, 1.0f));
    Character.SetMaxStrength(maths::max(this->MaxPushForce, 0.0f));
    this->Proxy->bCanPushBodies = this->bCanPushBodies;
    this->Proxy->bCanBePushedByBodies = this->bCanBePushedByBodies;
}

void Jafg::ACharacterMovementComponent::SyncExternalChanges() noexcept
{
    if (!this->bHasWrittenRoot || this->IsReplaying())
    {
        return;
    }

    auto& Root{this->GetOwningActor().GetRootComponent()};
    auto& Character{*this->Proxy->Character};

    if (Root.GetLocalTranslation() != this->LastWrittenTranslation)
    {
        LOG_TRACE(LogMovement, "[{}]: Root was moved externally. Teleporting.", this->GetNameAsString())
        Character.SetPosition(::ToJoltR(Root.GetLocalTranslation()));
        this->LastWrittenTranslation = Root.GetLocalTranslation();
    }
    if (Root.GetLocalRotator() != this->LastWrittenRotation)
    {
        LWorldQuat const Rotation{maths::normalize(Root.GetLocalRotator())};
        if (::IsFreeRotationMode(this->GetMovementMode()))
        {
            Character.SetRotation(::ToJolt(Rotation));
            Character.SetUp(::ToJolt(Rotation * maths::up_vector<LVec3F>));
        }
        else
        {
            Character.SetRotation(::ToJolt(Movement::MakeUpright(Rotation, ::FromJolt(Character.GetUp()))));
        }
        this->LastWrittenRotation = Root.GetLocalRotator();
    }
}

void Jafg::ACharacterMovementComponent::FollowBase() noexcept
{
    auto& Character{*this->Proxy->Character};
    JPH::BodyID const Ground{Character.GetGroundBodyID()};
    if (!this->bFollowBaseRotation || !IsGroundedMovementMode(this->GetMovementMode()) || Ground.IsInvalid())
    {
        this->BaseBodyId = ~0u;
        return;
    }

    LWorldQuat CurrentRotation;
    {
        JPH::BodyLockRead const Lock{this->Proxy->Subsystem.PhysicsSystem.GetBodyLockInterface(), Ground};
        if (!Lock.Succeeded())
        {
            this->BaseBodyId = ~0u;
            return;
        }
        CurrentRotation = ::FromJolt(Lock.GetBody().GetRotation());
    }

    if (Ground.GetIndexAndSequenceNumber() == this->BaseBodyId)
    {
        /* Only the turn around up. Tilting is handled by the gravity / surface alignment. */
        LWorldQuat const Delta{CurrentRotation * glm::inverse(this->BaseRotation)};
        LWorldQuat const Twist{Movement::ExtractTwist(Delta, ::FromJolt(Character.GetUp()))};
        Character.SetRotation(::ToJolt(maths::normalize(Twist * ::FromJolt(Character.GetRotation()))));
    }

    this->BaseBodyId = Ground.GetIndexAndSequenceNumber();
    this->BaseRotation = CurrentRotation;
}

void Jafg::ACharacterMovementComponent::ApplyLook(LVec3F const& Look, EMovementMode Mode) noexcept
{
    if (Look == LVec3F{0.0f})
    {
        return;
    }

    auto& Character{*this->Proxy->Character};
    if (::IsFreeRotationMode(Mode))
    {
        LWorldQuat const Body{::FromJolt(Character.GetRotation())};
        /* The look delta is in body space. */
        this->RotateBodyAroundEye(Body * Movement::LookDeltaToRotation(Look) * glm::inverse(Body));
        Character.SetUp(::ToJolt(::FromJolt(Character.GetRotation()) * maths::up_vector<LVec3F>));
    }
    else
    {
        /* Yaw turns the body, pitch only the view. Roll is not possible while upright. */
        LVec3F const Up{::FromJolt(Character.GetUp())};
        Character.SetRotation(::ToJolt(maths::normalize(maths::angle_axis(-Look.x, Up) * ::FromJolt(Character.GetRotation()))));
        f32 const MaxPitch{maths::radians(maths::clamp(this->MaxViewPitch, 0.0f, 89.9f))};
        this->ViewPitch = maths::clamp(this->ViewPitch + Look.y, -MaxPitch, MaxPitch);
    }
}

void Jafg::ACharacterMovementComponent::RotateBodyAroundEye(LWorldQuat const& Delta) noexcept
{
    auto& Character{*this->Proxy->Character};
    LWorldQuat const Rotation{::FromJolt(Character.GetRotation())};
    LWorldVec3 const Position{::FromJoltR(Character.GetPosition())};
    LVec3F const EyeOffset{0.0f, this->GetSimulatedEyeHeight(), 0.0f};
    LWorldVec3 const Eye{Position + LWorldVec3{Rotation * EyeOffset}};

    LWorldQuat const NewRotation{maths::normalize(Delta * Rotation)};
    Character.SetRotation(::ToJolt(NewRotation));
    Character.SetPosition(::ToJoltR(Eye - LWorldVec3{NewRotation * EyeOffset}));
}

void Jafg::ACharacterMovementComponent::FoldViewPitchIntoBody() noexcept
{
    if (this->ViewPitch == 0.0f)
    {
        return;
    }

    auto& Character{*this->Proxy->Character};
    LWorldQuat const Body{::FromJolt(Character.GetRotation())};
    /* Keeps the view exactly where it is, but the body now looks there instead. */
    this->RotateBodyAroundEye(Body * Movement::PitchToRotation(this->ViewPitch) * glm::inverse(Body));
    Character.SetUp(::ToJolt(::FromJolt(Character.GetRotation()) * maths::up_vector<LVec3F>));
    this->ViewPitch = 0.0f;
}

f32 Jafg::ACharacterMovementComponent::GetSimulatedEyeHeight() const noexcept
{
    return this->IsCrouched() ? this->CrouchingEyeHeight : this->StandingEyeHeight;
}

std::optional<Jafg::ACharacterMovementComponent::LSurface> Jafg::ACharacterMovementComponent::FindMagneticSurface() const
{
    auto const& Character{*this->Proxy->Character};
    LVec3F const Up{::FromJolt(Character.GetUp())};

    /* Something is touched already. */
    if (Character.GetGroundState() != EGroundState::InAir && !Character.GetGroundBodyID().IsInvalid())
    {
        LVec3F const Normal{::FromJolt(Character.GetGroundNormal())};
        if (maths::squared_magnitude(Normal) > 0.5f && maths::dot(Normal, Up) > ::MagBootsMinSurfaceDot)
        {
            return LSurface{.Normal = maths::normalize(Normal), .bTouching = true};
        }
    }

    /* Look for a surface below the feet. */
    auto const& System{this->Proxy->Subsystem.PhysicsSystem};
    query_filters const Filters{System};
    f32 const CenterHeight{0.5f * (this->IsCrouched() ? this->CrouchingHeight : this->StandingHeight)};
    JPH::RRayCast const Ray{
          Character.GetPosition() + Character.GetRotation() * JPH::Vec3{0.0f, CenterHeight, 0.0f}
        , ::ToJolt(-Up * (CenterHeight + this->MagBootsReach))
        };
    JPH::RayCastResult Hit;
    if (System.GetNarrowPhaseQuery().CastRay(Ray, Hit, Filters.BroadPhase, Filters.Object, Filters.Body))
    {
        JPH::BodyLockRead const Lock{System.GetBodyLockInterface(), Hit.mBodyID};
        if (Lock.Succeeded())
        {
            LVec3F const Normal{::FromJolt(Lock.GetBody().GetWorldSpaceSurfaceNormal(Hit.mSubShapeID2, Ray.GetPointOnRay(Hit.mFraction)))};
            if (maths::dot(Normal, Up) > ::MagBootsMinSurfaceDot)
            {
                return LSurface{.Normal = Normal, .bTouching = false};
            }
        }
    }

    return std::nullopt;
}

LVec3F Jafg::ACharacterMovementComponent::GetFlatWishDirection(LVec3F const& Axes) const noexcept
{
    auto const& Character{*this->Proxy->Character};
    LVec3F const Up{::FromJolt(Character.GetUp())};
    LWorldQuat const Body{::FromJolt(Character.GetRotation())};
    LVec3F const Forward{Movement::NormalizeOr(
          Movement::ProjectOnPlane(Body * maths::forward_vector<LVec3F>, Up)
        , Movement::AnyPerpendicular(Up)
        )};
    /* Right is cross(forward, up) in this right-handed engine. */
    LVec3F const Right{maths::cross(Forward, Up)};
    return Movement::ClampMagnitude(Right * Axes.x + Forward * Axes.z, 1.0f);
}
