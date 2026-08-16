// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PhysicsCore.h"
#include "Framework/RigidObject.h"

namespace JPH
{

class PhysicsSystem;

} /* ~Namespace JPH */

namespace Jafg
{

struct LPhysicsSystemCreateInfo final
{
    std::size_t RigidBodyLimit;
    std::optional<std::size_t> BodyMutexNumber;
    std::size_t BodyPairLimit;
    std::size_t ContactConstraintLimit;

    std::size_t TemporalUpdateStackSizeLimit;

    std::size_t JobLimit;
    std::size_t BarrierLimit;
    std::size_t ThreadLimit;

    f64 PhysicsStep;
    f64 AccumulatorLimit;
    std::size_t PhysicsSteps;
};

class LPhysicsSystem
{
public:

    LPhysicsSystem() = delete;
    ENGINE_API explicit LPhysicsSystem(ESkipInit) noexcept: SubSystem{} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LPhysicsSystem)
    explicit LPhysicsSystem(LPhysicsSystemCreateInfo Info): LPhysicsSystem{ESkipInit::Here} { *this = std::move(Info); }
    ENGINE_API LPhysicsSystem& operator=(LPhysicsSystemCreateInfo Info);
    ENGINE_API virtual ~LPhysicsSystem();

    //# If this returns false. All interactions with this class are UB.
    NODISCARD constexpr bool IsValid() const noexcept { return !!this->SubSystem; }

    NODISCARD ENGINE_API JPH::PhysicsSystem* operator->() noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem const* operator->() const noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem& operator*() noexcept;
    NODISCARD ENGINE_API JPH::PhysicsSystem const& operator*() const noexcept;

    void Advance(f64 Dt) noexcept
    {
        this->Accumulator += Dt;
        if (this->Accumulator >= this->AccumulatorLimit)
        {
            LOG_WARNING(LogPhysics, "Physics accumulator is too high: {} >= {}."
                , this->Accumulator, this->AccumulatorLimit)
            this->Accumulator = this->AccumulatorLimit;
        }
    }

    //# @return True, if physics were updated at least once.
    ENGINE_API bool TryUpdate();
    ENGINE_API void Update(std::size_t Steps);
    NODISCARD constexpr bool _check_IsUpdating() const noexcept { return this->_check_bUpdating.load(); }

private:

#if JAFG_DO_CHECKS
    std::atomic_bool _check_bUpdating;
#endif /* JAFG_DO_CHECKS */

    f64 PhysicsStep;
    f64 Accumulator;
    f64 AccumulatorLimit;
    std::size_t PhysicsSteps;

    Detail::LPhysicsSubsystem* SubSystem;
};

} /* ~Namespace Jafg */
