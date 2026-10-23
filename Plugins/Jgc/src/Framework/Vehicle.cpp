// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/Vehicle.h"
#include "Engine/Engine.h"
#include "Framework/PhysicsBody.h"

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
            Rigid.MakeBox(LWorldVec3{5});

            Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Activate);

            // Rigid.SetLinearVelocity({0,0,5});

            break;
        }
        default:
        {
            checkNoEntry()
        }
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
