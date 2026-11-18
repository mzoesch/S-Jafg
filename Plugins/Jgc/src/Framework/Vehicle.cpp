// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/Vehicle.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/AvatarComponent.h"
#include "Framework/MovementComponent.h"
#include "Framework/PersonaController.h"
#include "Framework/PhysicsBody.h"
#include "Framework/PhysicsSystem.h"
#include "Framework/StaticMeshComponent.h"

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
    this->DrivenAvatar = nullptr;

    Super::OnGarbage(Reason);
}

bool Jgc::AVehicleComponent::CanInteract(Jafg::AAvatarComponent const& Instigator) const
{
    return Super::CanInteract(Instigator) && !this->HasDriver() && this->GetOwningAvatar();
}

bool Jgc::AVehicleComponent::Interact(Jafg::AAvatarComponent& Instigator)
{
    return this->Enter(Instigator);
}

f32 Jgc::AVehicleComponent::GetSpeed() const noexcept
{
    auto const& Rigid{this->GetRigidComponent()};
    return Rigid.IsAddedToSimulation() ? maths::magnitude(Rigid.GetRigidObject().GetLinearVelocity()) : 0.0f;
}

LString Jgc::AVehicleComponent::GetInteractionText() const
{
    return "Enter";
}

Jafg::AAvatarComponent* Jgc::AVehicleComponent::GetDriver() const noexcept
{
    return this->Driver && Jafg::IsValidFast(this->GetOuter(), this->Driver) ? this->Driver : nullptr;
}

bool Jgc::AVehicleComponent::Enter(Jafg::AAvatarComponent& NewDriver)
{
    if (this->HasDriver())
    {
        LOG_VERBOSE(LogJgc, "[{}]: [{}] cannot enter. The seat is taken by [{}]."
            , this->GetNameAsString(), NewDriver.GetOwningActor().GetNameAsString(), this->Driver->GetOwningActor().GetNameAsString()
            )
        return false;
    }

    Jafg::AAvatarComponent* const VehicleAvatar{this->GetOwningAvatar()};
    if (!VehicleAvatar)
    {
        LOG_WARNING(LogJgc, "[{}]: Cannot be entered. [{}] has no [{}]."
            , this->GetNameAsString(), this->GetOwningActor().GetNameAsString(), algo::type_name<Jafg::AAvatarComponent>()
            )
        return false;
    }

    auto& DriverActor{NewDriver.GetOwningActor()};
    if (!DriverActor.AttachTo(this->GetRigidComponent()))
    {
        return false;
    }

    LOG_VERBOSE(LogJgc, "[{}]: [{}] enters.", this->GetNameAsString(), DriverActor.GetNameAsString())
    this->Driver = &NewDriver;
    this->DrivenAvatar = VehicleAvatar;

    if (auto* Movement{DriverActor.GetComponent<Jafg::AMovementComponent>()})
    {
        Movement->ClearIntent();
        Movement->SetFrozen(true);
    }
    /* The driver hands its controls over to the vehicle. */
    if (auto* Ctrl{NewDriver.GetController()})
    {
        Ctrl->Possess(VehicleAvatar);
    }

    return true;
}

void Jgc::AVehicleComponent::Exit()
{
    Jafg::AAvatarComponent* const OldDriver{this->GetDriver()};
    this->Driver = nullptr;
    if (!OldDriver)
    {
        return;
    }

    auto& DriverActor{OldDriver->GetOwningActor()};
    LOG_VERBOSE(LogJgc, "[{}]: [{}] exits.", this->GetNameAsString(), DriverActor.GetNameAsString())

    /* Stays where it sits, which is where it entered relative to this vehicle. */
    DriverActor.Detach(Jafg::EAttachmentRule::KeepWorld);

    if (auto* Movement{DriverActor.GetComponent<Jafg::AMovementComponent>()})
    {
        Movement->SetFrozen(false);
        if (auto const& Root{this->GetRigidComponent()}; Root.IsAddedToSimulation())
        {
            Movement->SetVelocity(const_cast<Jafg::ARigidComponent&>(Root).GetParentPhysicsSystem().GetBodyInterfaceNoLock()
                .GetPointVelocity(Root.GetRigidObject().GetID(), DriverActor.GetRootComponent().GetWorldTranslationSlow()));
        }
    }

    Jafg::AAvatarComponent* const VehicleAvatar{std::exchange(this->DrivenAvatar, nullptr)};
    if (VehicleAvatar && Jafg::IsValidFast(this->GetOuter(), VehicleAvatar))
    {
        if (auto* Ctrl{VehicleAvatar->GetController()})
        {
            Ctrl->Possess(OldDriver);
        }
    }
}

void Jgc::AVehicleComponent::ParentPhysicsTick(f32 Dt, bool bFollowup)
{
    Super::ParentPhysicsTick(Dt, bFollowup);

    /* Without dampeners the vehicle keeps drifting and turning. */
    if (auto Rigid{this->GetRigidComponent().GetRigidObject()}; this->bDampeners && Rigid.IsActive())
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
