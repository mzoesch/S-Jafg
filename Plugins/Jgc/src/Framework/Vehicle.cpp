// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/Vehicle.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/MovementComponent.h"
#include "Framework/Pawn.h"
#include "Framework/PhysicsBody.h"
#include "Framework/PhysicsSystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Jgc/Framework/JgcCharacterInputComponent.h"
#include "Jgc/Framework/JgcInputComponent.h"

void Jgc::AVehicleComponent::OnAttach(Jafg::AActor& Owner)
{
    Super::OnAttach(Owner);
    check(Owner.HasRootComponent() && Owner.GetRootComponent().IsA<Jafg::ARigidComponent>())

    check(this->GetVehicleType() != EVehicle::Identity)
    switch (this->GetVehicleType())
    {
        case EVehicle::Dynamic3D:
        {
            auto& Rigid{this->GetRigidComponent()};
            check(!Rigid.IsAddedToSimulation())
            Rigid.SetPhysicsMotion(Jafg::EPhysicsMotion::Dynamic);
            Rigid.SetPhysicsLayer(Jafg::EPhysicsLayer::Dynamic);
            Rigid.MakeConvex(this->GetMutableVehicleMesh().GetMutableMesh());
            Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Activate);

            break;
        }
        default:
        {
            checkNoEntry()
        }
    }
}

void Jgc::AVehicleComponent::OnGarbage(Jafg::EJxxRecordTearDownReason Reason)
{
    if (Reason == Jafg::EJxxRecordTearDownReason::Default)
    {
        if (this->HasDriver())
        {
            this->Exit();
        }
    }
    /* Everything else is torn down as well. */
    this->Driver = nullptr;

    Super::OnGarbage(Reason);
}

bool Jgc::AVehicleComponent::CanInteract(Jafg::APawn const& Instigator) const
{
    return Super::CanInteract(Instigator) && !this->HasDriver();
}

bool Jgc::AVehicleComponent::Interact(Jafg::APawn& Instigator)
{
    return this->Enter(Instigator);
}

LString Jgc::AVehicleComponent::GetInteractionText() const
{
    return "Enter";
}

Jafg::APawn* Jgc::AVehicleComponent::GetDriver() const noexcept
{
    return this->Driver && Jafg::IsValidFast(this->GetOuter(), this->Driver) ? this->Driver : nullptr;
}

bool Jgc::AVehicleComponent::Enter(Jafg::APawn& Pawn)
{
    if (this->HasDriver())
    {
        LOG_VERBOSE(LogJgc, "[{}]: Pawn [{}] cannot enter. The seat is taken by [{}]."
            , this->GetNameAsString(), Pawn.GetNameAsString(), this->Driver->GetNameAsString()
            )
        return false;
    }

    LOG_VERBOSE(LogJgc, "[{}]: Pawn [{}] enters.", this->GetNameAsString(), Pawn.GetNameAsString())

    auto const& Root{this->GetRigidComponent()};
    auto const& PawnRoot{Pawn.GetRootComponent()};
    LWorldQuat const InverseRotation{maths::inverse(maths::normalize(Root.GetLocalRotator()))};
    this->DriverEntryLocation = InverseRotation * (PawnRoot.GetLocalTranslation() - Root.GetLocalTranslation());
    this->DriverEntryRotation = maths::normalize(InverseRotation * PawnRoot.GetLocalRotator());
    this->Driver = &Pawn;

    if (auto* Movement{Pawn.GetComponent<Jafg::AMovementComponent>()})
    {
        Movement->ClearIntent();
        Movement->SetFrozen(true);
    }
    if (auto* CharacterInput{Pawn.GetComponent<AJgcCharacterInputComponent>()})
    {
        CharacterInput->OnEnteredVehicle(*this);
    }
    if (auto* VehicleInput{Pawn.GetComponent<AJgcInputComponent>()})
    {
        (void)VehicleInput->PossessVehicle(this);
    }

    return true;
}

void Jgc::AVehicleComponent::Exit()
{
    Jafg::APawn* Pawn{this->GetDriver()};
    this->Driver = nullptr;
    if (!Pawn)
    {
        return;
    }

    LOG_VERBOSE(LogJgc, "[{}]: Pawn [{}] exits.", this->GetNameAsString(), Pawn->GetNameAsString())

    if (auto* VehicleInput{Pawn->GetComponent<AJgcInputComponent>()}; VehicleInput && VehicleInput->GetVehicle() == this)
    {
        (void)VehicleInput->PossessVehicle(nullptr);
    }

    auto& Root{this->GetRigidComponent()};
    LWorldQuat const VehicleRotation{maths::normalize(Root.GetLocalRotator())};
    LWorldVec3 const Location{Root.GetLocalTranslation() + VehicleRotation * this->DriverEntryLocation};
    LWorldQuat const Rotation{maths::normalize(VehicleRotation * this->DriverEntryRotation)};

    if (auto* Movement{Pawn->GetComponent<Jafg::AMovementComponent>()})
    {
        Movement->Teleport(Location, Rotation);
        Movement->SetFrozen(false);
        if (Root.IsAddedToSimulation())
        {
            Movement->SetVelocity(Root.GetParentPhysicsSystem().GetBodyInterfaceNoLock()
                .GetPointVelocity(Root.GetRigidObject().GetID(), Location));
        }
    }
    else
    {
        Pawn->GetRootComponent().SetLocalTranslationByTeleport(Location);
        Pawn->GetRootComponent().SetLocalRotatorByTeleport(Rotation);
    }

    if (auto* CharacterInput{Pawn->GetComponent<AJgcCharacterInputComponent>()})
    {
        CharacterInput->OnExitedVehicle();
    }
}

void Jgc::AVehicleComponent::ParentPhysicsTick(f32 Dt, bool bFollowup)
{
    Super::ParentPhysicsTick(Dt, bFollowup);

    if (auto Rigid{this->GetRigidComponent().GetRigidObject()}; Rigid.IsActive())
    {
        if (this->bLastUserThrust < this->GetEngine().FrameCount)
        {
            constexpr f32 Deceleration = 100.0f;

            LVec3F Direction{Rigid.GetLinearVelocity()};
            f32 V1{maths::squared_magnitude(Direction)};
            if (V1 > 0.0f)
            {
                f32 V2{maths::max(0.0f, V1 - Deceleration * Dt)};
                Rigid.SetLinearVelocity(Direction * (V2/V1));
            }
        }

        if (this->bLastUserTorque < this->GetEngine().FrameCount)
        {
            constexpr f32 Deceleration = 0.01f;

            LVec3F Direction{Rigid.GetAngularVelocity()};
            f32 V1{maths::squared_magnitude(Direction)};
            if (V1 > 0.0f)
            {
                f32 V2{maths::max(0.0f, V1 - Deceleration * Dt)};
                Rigid.SetAngularVelocity(Direction * (V2/V1));
            }
        }
    }
}

Jafg::AStaticMeshComponent const& Jgc::AVehicleComponent::GetVehicleMesh() const
{
    check(std::accumulate(
          this->GetOwningActor().GetRootComponent().GetChildren().begin()
        , this->GetOwningActor().GetRootComponent().GetChildren().end()
        , 0u
        , [](std::size_t Count, auto const& Comp)
    {
        return Count + (Comp->template IsA<Jafg::AStaticMeshComponent>() ? 1u : 0u);
    }) == 1u)
    return this->GetOwningActor().GetRootComponent().GetComponentChecked<Jafg::AStaticMeshComponent>();
}

Jafg::AStaticMeshComponent& Jgc::AVehicleComponent::GetMutableVehicleMesh()
{
    check(std::accumulate(
          this->GetOwningActor().GetRootComponent().GetChildren().begin()
        , this->GetOwningActor().GetRootComponent().GetChildren().end()
        , 0u
        , [](std::size_t Count, auto const& Comp)
    {
        return Count + (Comp->template IsA<Jafg::AStaticMeshComponent>() ? 1u : 0u);
    }) == 1u)
    return this->GetOwningActor().GetRootComponent().GetComponentChecked<Jafg::AStaticMeshComponent>();
}

void Jgc::AVehicleComponent::AddThrust(LVec3F const& Thrust, bool bUserInput)
{
    auto& Rigid{this->GetRigidComponent()};
    Rigid.GetParentPhysicsSystem().GetBodyInterfaceNoLock().AddLocalForce(Rigid.GetRigidObject().GetID(), Thrust);
    check(Rigid.GetRigidObject().IsActive())
    if (bUserInput)
    {
        this->bLastUserThrust = this->GetEngine().FrameCount;
    }
}

void Jgc::AVehicleComponent::AddTorque(LVec3F const& Torque, bool bUserInput)
{
    auto& Rigid{this->GetRigidComponent()};
    Rigid.GetParentPhysicsSystem().GetBodyInterfaceNoLock().AddLocalTorque(Rigid.GetRigidObject().GetID(), Torque);
    check(Rigid.GetRigidObject().IsActive())
    if (bUserInput)
    {
        this->bLastUserTorque = this->GetEngine().FrameCount;
    }
}
