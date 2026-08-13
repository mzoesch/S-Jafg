// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"
#include "Framework/Editor.h"
#include "Framework/PhysicsSystem.h"
#include "Framework/RigidObject.h"
#include "PhysicsForeignCore.h"

void physx::debug_renderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    check(this->Comp)

    LWorldVec3 From{static_cast<LWorldReal>(inFrom.GetX()), static_cast<LWorldReal>(inFrom.GetY()), static_cast<LWorldReal>(inFrom.GetZ())};
    LWorldVec3 To{static_cast<LWorldReal>(inTo.GetX()), static_cast<LWorldReal>(inTo.GetY()), static_cast<LWorldReal>(inTo.GetZ())};
    if (maths::squared_magnitude(To - From) > 0.001f)
    {
        this->Comp->AddLineNextTick({
            .Tint = Colors::Red,
            .Value = {.begin=From, .end=To,},
            });
    }
}

void physx::debug_renderer::DrawText3D(JPH::RVec3Arg inPosition, std::string_view const& inString, JPH::ColorArg inColor, float inHeight)
{
    check(this->Comp)

    this->Comp->AddTextNextTick({
        .Tint = LColor::FromU32(inColor.GetUInt32()),
        .Height = inHeight,
        .Location = {static_cast<LWorldReal>(inPosition.GetX()), static_cast<LWorldReal>(inPosition.GetY()), static_cast<LWorldReal>(inPosition.GetZ())},
        .Text = LString{inString},
        });
}

Jafg::LPhysicsSystem& Jafg::LPhysicsSystem::operator=(LPhysicsSystemCreateInfo  Info)
{
    if (this->IsValid())
    {
        checkNoEntry() // implement
    }

    if (Info.PhysicsStep <= 0.0_r)
    {
        LOG_FATAL(LogPhysics, "PhysicsStep must be positive: {} <= 0.0"
            , Info.PhysicsStep)
    }
    if (Info.PhysicsStep > static_cast<LWorldReal>(1.0/20.0))
    {
        LOG_FATAL(LogPhysics, "PhysicsStep is too large: {} > 20Hz"
            , Info.PhysicsStep)
    }
    if (Info.AccumulatorLimit <= 0.0_r)
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit must be positive: {} <= 0.0"
            , Info.AccumulatorLimit)
    }
    if (Info.AccumulatorLimit > static_cast<LWorldReal>(1.0/10.0))
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit is too large: {} > 10Hz"
            , Info.AccumulatorLimit)
    }
    if (Info.AccumulatorLimit < Info.PhysicsStep)
    {
        LOG_FATAL(LogPhysics, "AccumulatorLimit must be at least PhysicsStep: {} < {}"
            , Info.AccumulatorLimit, Info.PhysicsStep)
    }
    if (Info.PhysicsSteps < 1)
    {
        LOG_FATAL(LogPhysics, "PhysicsSteps must be at least 1: {} < 1"
            , Info.PhysicsSteps)
    }
    if (Info.PhysicsSteps > 8)
    {
        LOG_FATAL(LogPhysics, "PhysicsSteps is too large: {} > 8"
            , Info.PhysicsSteps)
    }

    this->PhysicsStep = Info.PhysicsStep;
    this->Accumulator = 0.0;
    this->AccumulatorLimit = Info.AccumulatorLimit;
    this->PhysicsSteps = Info.PhysicsSteps;
    this->SubSystem = Detail::LPhysicsSubsystem::FromCreateInfo(std::move(Info)).release();

    auto& System{*this->SubSystem};
    {
        auto& Interface{System->GetBodyInterfaceNoLock()};

        JPH::ShapeSettings::ShapeResult FloorShapeSettingsResult = JPH::BoxShapeSettings{JPH::Vec3(100.0f, 1.0f, 100.0f)}.Create();
        check(!FloorShapeSettingsResult.HasError())
        JPH::ShapeRefC const& FloorShape = FloorShapeSettingsResult.Get();
        JPH::BodyCreationSettings floor_settings (FloorShape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, EPhysicsLayer::Static);

        LRigidObjectCreateInfo CreateInfo{
            .Translation = {0.0, -1.0, 0.0},
            .Rotation = maths::identity<LWorldQuat>,
            .Motion = EMotion::Static,
            .Layer = EPhysicsLayer::Static,
            };


        System.Floor = LRigidObject::FromNative(Interface.CreateBody(floor_settings));
        check(System.Floor.IsValid())
        Interface.AddBody(System.Floor->GetID(), JPH::EActivation::DontActivate);
    }

    return *this;
}

Jafg::LPhysicsSystem::~LPhysicsSystem()
{
    delete this->SubSystem;
}

void Jafg::LPhysicsSystem::TearDown() const noexcept // temp only remove later.
{
    auto& Interface{this->SubSystem->PhysicsSystem.GetBodyInterfaceNoLock()};
    Interface.RemoveBody(this->SubSystem->Floor->GetID());
    Interface.DestroyBody(this->SubSystem->Floor->GetID());
}

JPH::PhysicsSystem* Jafg::LPhysicsSystem::operator->() noexcept
{
    check(this->IsValid())
    return &this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem const* Jafg::LPhysicsSystem::operator->() const noexcept
{
    check(this->IsValid())
    return &this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem& Jafg::LPhysicsSystem::operator*() noexcept
{
    check(this->IsValid())
    return this->SubSystem->PhysicsSystem;
}

JPH::PhysicsSystem const& Jafg::LPhysicsSystem::operator*() const noexcept
{
    check(this->IsValid())
    return this->SubSystem->PhysicsSystem;
}

bool Jafg::LPhysicsSystem::TryUpdate()
{
    check(this->IsValid())

    bool Result{};
    while (this->Accumulator >= this->PhysicsStep)
    {
        /* Can only be updated by this thread. */
        check(!this->_check_IsUpdating())
        checkCode(this->_check_bUpdating.store(true))

        this->SubSystem->PhysicsSystem.Update(static_cast<f32>(this->PhysicsStep) , static_cast<int>(this->PhysicsSteps)
            , &this->SubSystem->TempAllocator, &this->SubSystem->JobSystem);
        this->Accumulator -= this->PhysicsStep;

        Result = true;

        checkCode(this->_check_bUpdating.store(false))
    }

    return Result;
}

void Jafg::LPhysicsSystem::Update(std::size_t Steps)
{
    if (Steps > 0)
    {
        /* Can only be updated by this thread. */
        check(!this->_check_IsUpdating())
        checkCode(this->_check_bUpdating.store(true))

        while (Steps-- > 0)
        {
            this->SubSystem->PhysicsSystem.Update(static_cast<f32>(this->PhysicsStep) , static_cast<int>(this->PhysicsSteps)
                , &this->SubSystem->TempAllocator, &this->SubSystem->JobSystem);
        }
        this->Accumulator = 0.0f;

        checkCode(this->_check_bUpdating.store(false))
    }
}
