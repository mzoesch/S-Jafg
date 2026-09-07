// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Framework/PhysicsCore.h"
#include "Framework/RigidObject.h"

#ifndef JoltPhysics_USE_ASSERTS
    #error "JoltPhysics_USE_ASSERTS is not defined."
#endif /* JoltPhysics_USE_ASSERTS */
#ifdef JPH_ENABLE_ASSERTS
    #error "JPH_ENABLE_ASSERTS is defined; use JoltPhysics_USE_ASSERTIONS_IN_DISTRIBUTION instead. In development builds assertions are always enabled."
#endif /* JPH_ENABLE_ASSERTS */
#if JoltPhysics_USE_ASSERTS && JAFG_IN_SHIPPING
    #define JPH_ENABLE_ASSERTS
#endif /* JoltPhysics_USE_ASSERTS && JAFG_IN_SHIPPING */
#if JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT
    #define JPH_ENABLE_ASSERTS
#endif /* JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT */

#ifndef JoltPhysics_DOUBLE_PRECISION
    #error "JoltPhysics_DOUBLE_PRECISION is not defined."
#endif /* !JoltPhysics_DOUBLE_PRECISION */
#ifdef JPH_DOUBLE_PRECISION
    #error "JPH_DOUBLE_PRECISION is defined; use JoltPhysics_DOUBLE_PRECISION instead."
#endif /* JPH_DOUBLE_PRECISION */
#if JoltPhysics_DOUBLE_PRECISION
    #define JPH_DOUBLE_PRECISION
#endif /* JoltPhysics_DOUBLE_PRECISION */

#ifndef JoltPhysics_CROSS_PLATFORM_DETERMINISTIC
    #error "JoltPhysics_CROSS_PLATFORM_DETERMINISTIC is not defined."
#endif /* !JoltPhysics_CROSS_PLATFORM_DETERMINISTIC */
#ifdef JPH_CROSS_PLATFORM_DETERMINISTIC
    #error "JPH_CROSS_PLATFORM_DETERMINISTIC is defined; use JoltPhysics_CROSS_PLATFORM_DETERMINISTIC instead."
#endif /* JPH_CROSS_PLATFORM_DETERMINISTIC */
#if JoltPhysics_CROSS_PLATFORM_DETERMINISTIC
    #define JPH_CROSS_PLATFORM_DETERMINISTIC
#endif /* JoltPhysics_CROSS_PLATFORM_DETERMINISTIC */

#ifndef JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED
    #error "JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED is not defined."
#endif /* !JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED */
#ifdef JPH_FLOATING_POINT_EXCEPTIONS_ENABLED
    #error "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED is defined; use JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED instead."
#endif /* JPH_FLOATING_POINT_EXCEPTIONS_ENABLED */
#if JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED
    #define JPH_FLOATING_POINT_EXCEPTIONS_ENABLED
#endif /* JoltPhysics_FLOATING_POINT_EXCEPTIONS_ENABLED */

#ifndef JoltPhysics_OBJECT_LAYER_BITS
    #error "JoltPhysics_OBJECT_LAYER_BITS is not defined."
#endif /* !JoltPhysics_OBJECT_LAYER_BITS */
#ifdef OBJECT_LAYER_BITS
    #error "OBJECT_LAYER_BITS is defined; use JoltPhysics_OBJECT_LAYER_BITS instead."
#endif /* OBJECT_LAYER_BITS */
#define OBJECT_LAYER_BITS JoltPhysics_OBJECT_LAYER_BITS

#ifndef JoltPhysics_TRACK_BROADPHASE_STATS
    #error "JoltPhysics_TRACK_BROADPHASE_STATS is not defined."
#endif /* !JoltPhysics_TRACK_BROADPHASE_STATS */
#ifdef TRACK_BROADPHASE_STATS
    #error "TRACK_BROADPHASE_STATS is defined; use JoltPhysics_TRACK_BROADPHASE_STATS instead."
#endif /* TRACK_BROADPHASE_STATS */
#if JoltPhysics_TRACK_BROADPHASE_STATS
    #define JPH_TRACK_BROADPHASE_STATS
#endif /* JoltPhysics_TRACK_BROADPHASE_STATS */

#ifndef JoltPhysics_TRACK_NARROWPHASE_STATS
    #error "JoltPhysics_TRACK_NARROWPHASE_STATS is not defined."
#endif /* !JoltPhysics_TRACK_NARROWPHASE_STATS */
#ifdef JPH_TRACK_NARROWPHASE_STATS
    #error "JPH_TRACK_NARROWPHASE_STATS is defined; use JoltPhysics_TRACK_NARROWPHASE_STATS instead."
#endif /* JPH_TRACK_NARROWPHASE_STATS */
#if JoltPhysics_TRACK_NARROWPHASE_STATS
    #define JPH_TRACK_NARROWPHASE_STATS
#endif /* JoltPhysics_TRACK_NARROWPHASE_STATS */

#ifndef JoltPhysics_TRACK_SIMULATION_STATS
    #error "JoltPhysics_TRACK_SIMULATION_STATS is not defined."
#endif /* !JoltPhysics_TRACK_SIMULATION_STATS */
#ifdef JPH_TRACK_SIMULATION_STATS
    #error "JPH_TRACK_SIMULATION_STATS is defined; use JoltPhysics_TRACK_SIMULATION_STATS instead."
#endif /* JPH_TRACK_SIMULATION_STATS */
#if JoltPhysics_TRACK_SIMULATION_STATS
    #define JPH_TRACK_SIMULATION_STATS
#endif /* JoltPhysics_TRACK_SIMULATION_STATS */

#ifndef JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION
    #error "JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION is not defined."
#endif /* !JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION */
#ifdef JPH_DEBUG_RENDERER
    #error "JPH_DEBUG_RENDERER is defined; use JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION instead. In development builds debug rendering is always enabled."
#endif /* JPH_DEBUG_RENDERER */
#if JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION && JAFG_IN_SHIPPING
    #define JPH_DEBUG_RENDERER
#endif /* JoltPhysics_DEBUG_RENDERER_IN_DISTRIBUTION && JAFG_IN_SHIPPING */
#if JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT
    #define JPH_DEBUG_RENDERER
#endif /* JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT */

#ifndef JoltPhysics_PROFILER_IN_DISTRIBUTION
    #error "JoltPhysics_PROFILER_IN_DISTRIBUTION is not defined."
#endif /* !JoltPhysics_PROFILER_IN_DISTRIBUTION */
#ifdef JPH_PROFILE_ENABLED
    #error "JPH_PROFILE_ENABLED is defined; use JoltPhysics_PROFILER_IN_DISTRIBUTION instead. In development builds profiling is always enabled."
#endif /* JPH_PROFILE_ENABLED */
#if JoltPhysics_PROFILER_IN_DISTRIBUTION && JAFG_IN_SHIPPING
    #define JPH_PROFILE_ENABLED
#endif /* JoltPhysics_PROFILER_IN_DISTRIBUTION && JAFG_IN_SHIPPING */
#if JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT
    #define JPH_PROFILE_ENABLED
#endif /* JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT */

namespace Jafg
{

class AEditorPersonaControllerComponent;

} /* ~Namespace Jafg */

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

static_assert(std::same_as<Jafg::EPhysicsLayer::value_type, JPH::ObjectLayer>);
static_assert(std::same_as<Jafg::EPhysicsBroadPhase::value_type, JPH::BroadPhaseLayer::Type>);
static_assert(std::same_as<LWorldReal, JPH::Real>);

static_assert(std::same_as<std::underlying_type_t<Jafg::EPhysicsMotion>, std::underlying_type_t<JPH::EMotionType>>);
static_assert(std::to_underlying(Jafg::EPhysicsMotion::Static) == std::to_underlying(JPH::EMotionType::Static));
static_assert(std::to_underlying(Jafg::EPhysicsMotion::Kinematic) == std::to_underlying(JPH::EMotionType::Kinematic));
static_assert(std::to_underlying(Jafg::EPhysicsMotion::Dynamic) == std::to_underlying(JPH::EMotionType::Dynamic));

static_assert(std::same_as<std::underlying_type_t<Jafg::Physx::ESoftBodyConstraintColor>, std::underlying_type_t<JPH::ESoftBodyConstraintColor>>);
static_assert(std::to_underlying(Jafg::Physx::ESoftBodyConstraintColor::ConstraintType) == std::to_underlying(JPH::ESoftBodyConstraintColor::ConstraintType));
static_assert(std::to_underlying(Jafg::Physx::ESoftBodyConstraintColor::ConstraintGroup) == std::to_underlying(JPH::ESoftBodyConstraintColor::ConstraintGroup));
static_assert(std::to_underlying(Jafg::Physx::ESoftBodyConstraintColor::ConstraintOrder) == std::to_underlying(JPH::ESoftBodyConstraintColor::ConstraintOrder));

static_assert(std::same_as<std::underlying_type_t<Jafg::Physx::EShapeColor>, std::underlying_type_t<JPH::BodyManager::EShapeColor>>);
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::Instance) == std::to_underlying(JPH::BodyManager::EShapeColor::InstanceColor));
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::ShapeType) == std::to_underlying(JPH::BodyManager::EShapeColor::ShapeTypeColor));
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::MotionType) == std::to_underlying(JPH::BodyManager::EShapeColor::MotionTypeColor));
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::Sleep) == std::to_underlying(JPH::BodyManager::EShapeColor::SleepColor));
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::Island) == std::to_underlying(JPH::BodyManager::EShapeColor::IslandColor));
static_assert(std::to_underlying(Jafg::Physx::EShapeColor::Material) == std::to_underlying(JPH::BodyManager::EShapeColor::MaterialColor));

static_assert(std::same_as<std::underlying_type_t<Jafg::ESimulationAddingBehavior>, std::underlying_type_t<JPH::EActivation>>);
static_assert(std::to_underlying(Jafg::ESimulationAddingBehavior::Activate) == std::to_underlying(JPH::EActivation::Activate));
static_assert(std::to_underlying(Jafg::ESimulationAddingBehavior::Preserve) == std::to_underlying(JPH::EActivation::DontActivate));

namespace physx
{

class layer_filter: public JPH::ObjectLayerPairFilter
{
public:

    virtual bool ShouldCollide(JPH::ObjectLayer A, JPH::ObjectLayer B) const noexcept override
    {
        switch (A)
        {
        case Jafg::EPhysicsLayer::Static: return B == Jafg::EPhysicsLayer::Dynamic;
        case Jafg::EPhysicsLayer::Dynamic: return true;
        default:
        {
            checkNoEntry()
            return false;
        }
        }
    }
};

class broad_phase_layer_interface: public JPH::BroadPhaseLayerInterface
{
public:

    constexpr broad_phase_layer_interface() noexcept
    {
        this->Map[Jafg::EPhysicsLayer::Static] = JPH::BroadPhaseLayer{Jafg::EPhysicsBroadPhase::Static};
        this->Map[Jafg::EPhysicsLayer::Dynamic] = JPH::BroadPhaseLayer{Jafg::EPhysicsBroadPhase::Dynamic};
    }

    virtual uint GetNumBroadPhaseLayers() const override { return Jafg::EPhysicsBroadPhase::Count; }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer Layer) const override
    {
        check(Layer < Jafg::EPhysicsLayer::Count)
        return this->Map[Layer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual char const* GetBroadPhaseLayerName(JPH::BroadPhaseLayer Layer) const override
    {
        switch (static_cast<JPH::BroadPhaseLayer::Type>(Layer))
        {
        case Jafg::EPhysicsBroadPhase::Static: return "Static";
        case Jafg::EPhysicsBroadPhase::Dynamic: return "Dynamic";
        default:
        {
            checkNoEntry()
            return "Unknown";
        }
        }
    }
#endif /* defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED) */

private:

    JPH::BroadPhaseLayer Map[Jafg::EPhysicsBroadPhase::Count];
};

class broad_phase_layer_filter: public JPH::ObjectVsBroadPhaseLayerFilter
{
public:

    virtual bool ShouldCollide(JPH::ObjectLayer A, JPH::BroadPhaseLayer B) const override
    {
        switch (A)
        {
        case Jafg::EPhysicsLayer::Static: return B.GetValue() == Jafg::EPhysicsBroadPhase::Dynamic;
        case Jafg::EPhysicsLayer::Dynamic: return true;
        default:
        {
            checkNoEntry()
            return false;
        }
        }
    }
};

class contact_listener: public JPH::ContactListener
{
public:

    virtual JPH::ValidateResult OnContactValidate(JPH::Body const& A, JPH::Body const& B, JPH::RVec3Arg Offset, JPH::CollideShapeResult const& Result) override
    {
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(JPH::Body const& A, JPH::Body const& B, JPH::ContactManifold const& Manifold, JPH::ContactSettings &Settings) override
    {
    }

    virtual void OnContactPersisted(JPH::Body const& A, JPH::Body const& B, JPH::ContactManifold const& Manifold, JPH::ContactSettings &Settings) override
    {
    }

    virtual void OnContactRemoved(JPH::SubShapeIDPair const& inSubShapePair) override
    {
    }
};

class body_activation_listener: public JPH::BodyActivationListener
{
public:

    virtual void OnBodyActivated(JPH::BodyID const& Id, JPH::uint64 UserData) override
    {
    }

    virtual void OnBodyDeactivated(JPH::BodyID const& Id, JPH::uint64 UserData) override
    {
    }
};

class debug_renderer: public JPH::DebugRendererSimple
{
public:

    void PrepareFrame(LWorldEye const& Eye, Jafg::AEditorPersonaControllerComponent* Comp) noexcept
    {
        check(!this->Comp)
        this->Comp = Comp;
        this->SetCameraPos({Eye.translation.x, Eye.translation.y, Eye.translation.z});
    }

    void AdvanceFrameIfUsed() noexcept
    {
        if (this->Comp)
        {
            this->Comp = nullptr;
            this->NextFrame();
        }
    }

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, std::string_view const& inString, JPH::ColorArg inColor, float inHeight) override;

    NODISCARD static debug_renderer* get() noexcept { return static_cast<debug_renderer*>(sInstance); }

private:

    Jafg::AEditorPersonaControllerComponent* Comp{};
};

} /* ~Namespace physx */

namespace Jafg::Detail
{

struct LPhysicsSubsystem final
{
    LPhysicsSubsystem() noexcept = delete;

    NODISCARD static TUnique<LPhysicsSubsystem> FromCreateInfo(LPhysicsSystemCreateInfo&& Info)
    {
        STAT_FUNCTION()

        if (Info.RigidBodyLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "RigidBodyLimit is too large: {} > {}"
                , Info.RigidBodyLimit, std::numeric_limits<JPH::uint>::max())
        }
        if (Info.BodyMutexNumber && *Info.BodyMutexNumber > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "BodyMutexNumber is too large: {} > {}"
                , *Info.BodyMutexNumber, std::numeric_limits<JPH::uint>::max())
        }
        if (!Info.BodyMutexNumber)
        {
            Info.BodyMutexNumber = 0uz;
        }
        if (Info.BodyPairLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "BodyPairLimit is too large: {} > {}"
                , Info.BodyPairLimit, std::numeric_limits<JPH::uint>::max())
        }
        if (Info.ContactConstraintLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "ContactConstraintLimit is too large: {} > {}"
                , Info.ContactConstraintLimit, std::numeric_limits<JPH::uint>::max())
        }

        if (Info.TemporalUpdateStackSizeLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "TemporalUpdateStackSizeLimit is too large: {} > {}"
                , Info.TemporalUpdateStackSizeLimit, std::numeric_limits<JPH::uint>::max())
        }

        if (Info.JobLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "JobLimit is too large: {} > {}"
                , Info.JobLimit, std::numeric_limits<JPH::uint>::max())
        }
        if (Info.BarrierLimit > std::numeric_limits<JPH::uint>::max())
        {
            LOG_FATAL(LogPhysics, "BarrierLimit is too large: {} > {}"
                , Info.BarrierLimit, std::numeric_limits<JPH::uint>::max())
        }
        if (Info.ThreadLimit > std::numeric_limits<int>::max())
        {
            LOG_FATAL(LogPhysics, "ThreadLimit is too large: {} > {}"
                , Info.ThreadLimit, std::numeric_limits<int>::max())
        }

        return TUnique<LPhysicsSubsystem>{new LPhysicsSubsystem{Info}};
    }

    physx::broad_phase_layer_interface BroadPhaseLayerInterface;
    physx::broad_phase_layer_filter BroadPhaseLayerFilter;
    physx::layer_filter LayerFilter;
    JPH::PhysicsSystem PhysicsSystem;
    JPH::JobSystemThreadPool JobSystem;
    JPH::TempAllocatorImpl TempAllocator;
    physx::body_activation_listener BodyActivationListener;
    physx::contact_listener ContactListener;

    NODISCARD constexpr JPH::PhysicsSystem* operator->() noexcept { return &this->PhysicsSystem; }
    NODISCARD constexpr JPH::PhysicsSystem const* operator->() const noexcept { return &this->PhysicsSystem; }
    NODISCARD constexpr JPH::PhysicsSystem& operator*() noexcept { return this->PhysicsSystem; }
    NODISCARD constexpr JPH::PhysicsSystem const& operator*() const noexcept { return this->PhysicsSystem; }

private:

    explicit LPhysicsSubsystem(LPhysicsSystemCreateInfo const& Info)
        : JobSystem{static_cast<JPH::uint>(Info.JobLimit), static_cast<JPH::uint>(Info.BarrierLimit), static_cast<int>(Info.ThreadLimit)}
        , TempAllocator{Info.TemporalUpdateStackSizeLimit}
    {
        this->PhysicsSystem.Init(
            static_cast<JPH::uint>(Info.RigidBodyLimit),
            static_cast<JPH::uint>(*Info.BodyMutexNumber),
            static_cast<JPH::uint>(Info.BodyPairLimit),
            static_cast<JPH::uint>(Info.ContactConstraintLimit),
            this->BroadPhaseLayerInterface,
            this->BroadPhaseLayerFilter,
            this->LayerFilter
            );
        this->PhysicsSystem.SetBodyActivationListener(&this->BodyActivationListener);
        this->PhysicsSystem.SetContactListener(&this->ContactListener);
    }
};

} /* ~Namespace Jafg::Detail */
