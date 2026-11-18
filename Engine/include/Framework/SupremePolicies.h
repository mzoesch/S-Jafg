// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldObject.h"
#include "Framework/Lackey.h"
#include "SupremePolicies.generated.h"

namespace Jafg
{

class AActor;
class AAvatarStart;
class APersonaController;

DECLARE_JAFG_CLASS()
class ENGINE_API ASupremePolicies : public AWorldObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASupremePolicies)

public:

    //#
    //# The first hook to customize world initialization.
    //# AActors added in this phase have to be deferred as they will be automatically finalized later.
    //# This hook run before the #LTrack::OnWorldPreInit.
    //#
    virtual void OnWorldPreInit() {}

    //#
    //# The last hook to customize world initialization.
    //#
    virtual void OnWorldPostInit() {}

    //#
    //# Called when an incoming connection request is received.
    //# TODO: Add some more context information here when we build the networking stack.
    //#
    //# @return The persona controller to use for this connection. If nullptr is returned, the connection is rejected.
    //#         The persona controller must be deferred.
    //#
    enum EConnectionRequest
    {
        Local,
#if JAFG_WITH_EDITOR
        Editor,
#endif /* JAFG_WITH_EDITOR */
        Proxy,
    };
    virtual std::expected<TJxxUnique<APersonaController>, LString> OnIncomingConnectionRequest(EConnectionRequest Req);

    //#
    //# Called after a persona controller has been created.
    //#
    virtual void OnPersonaControllerCreated(APersonaController& Pc);

    //#
    //# Called whenever an avatar is spawned for a persona controller. The controller adopts the actor (see
    //# #APersonaController::AdoptActor) and possesses its avatar.
    //# The default spawns an actor of #DefaultAvatarClass and adds a root, an eye and an avatar if the class did not.
    //# @return An actor with an #AAvatarComponent. Empty to not possess anything.
    //#
    virtual TJxxUnique<AActor> SpawnAvatarForPersonaController(APersonaController const& Pc);
    //# @param Avatar The actor of the avatar that is about to start.
    //# @return The start for the avatar, or nullptr for default behavior.
    virtual AAvatarStart const* GetAvatarStartFor(AActor const& Avatar);

    //# Whether an avatar should be automatically spawned for a new persona controller.
    bool bCreateAvatar{ true };
    //# Whether avatar starts should be ignored when spawning avatars for persona controllers.
    bool bIgnoreAvatarStarts{ false };

    //# The default class for all persona controllers unless overridden by #OnIncomingConnectionRequest.
    TSubclassOf<APersonaController> PersonaControllerClass;
#if JAFG_WITH_EDITOR
    TSubclassOf<APersonaController> EditorPersonaControllerClass;
#endif /* JAFG_WITH_EDITOR */
    //# The class of the actors that #SpawnAvatarForPersonaController spawns.
    TSubclassOf<AActor> DefaultAvatarClass;
#if JAFG_WITH_EDITOR
    TSubclassOf<AActor> EditorDefaultAvatarClass;
#endif /* JAFG_WITH_EDITOR */

    ///////////////////////////////////////////////////////////////////////////////
    // Physics - If the overrides are not set the world will fallback to the equivalent methods.
    ///////////////////////////////////////////////////////////////////////////////

    //# ---------- World ----------

    //#
    //# The maximum number of rigid bodies that this policy prefers. This might not get accepted or clamped.
    //# @see #LWorld::GetMaxRigidBodies for the actual limit of the world.
    //#
    std::optional<std::size_t> PreferredRigidBodyLimit;
    NODISCARD virtual std::size_t GetPreferredRigidBodyLimit() const noexcept { return 65536uz; }

    //#
    //# The number of mutexes that this policy prefers. Should be a power of two. If not set, the world
    //# will try to pick the most optimal. The value should not exceed 64.
    //#
    std::optional<std::optional<std::size_t>> PreferredBodyMutexNumber;
    NODISCARD virtual std::optional<std::size_t> GetPreferredBodyMutexNumber() const noexcept { return {}; }

    //#
    //# The maximum number of body pairs that this policy prefers. Exceeding body pairs will fall through.
    //# Should be much higher than #GetPreferredContactConstraintLimit.
    //# This might not get accepted or clamped.
    //#
    std::optional<std::size_t> PreferredBodyPairLimit;
    NODISCARD virtual std::size_t GetPreferredBodyPairLimit() const noexcept { return 65536uz; }

    //#
    //# The maximum number of contact constraints that this policy prefers. Exceeding contacts will fall through.
    //# This might not get accepted or clamped.
    //#
    std::optional<std::size_t> PreferredContactConstraintLimit;
    NODISCARD virtual std::size_t GetPreferredContactConstraintLimit() const noexcept { return 1024uz; }

    //# In bytes.
    std::optional<std::size_t> PreferredTemporalUpdateStackSizeLimit;
    NODISCARD virtual std::size_t GetPreferredTemporalUpdateStackSizeLimit() const noexcept { return 10uz * 1024uz * 1024uz; } /* 10 MB */

    //# ---------- Concurrency ----------

    std::optional<std::size_t> PreferredJobLimit;
    NODISCARD virtual std::size_t GetPreferredJobLimit() const noexcept { return 2048uz; }

    std::optional<std::size_t> PreferredBarrierLimit;
    NODISCARD virtual std::size_t GetPreferredBarrierLimit() const noexcept { return 8uz; }

    std::optional<std::size_t> PreferredThreadLimit;
    NODISCARD virtual std::size_t GetPreferredThreadLimit() const noexcept
    {
        i64 Concurrency{static_cast<i64>(std::thread::hardware_concurrency()) - 3};
        if (Concurrency < 1)
        {
            Concurrency = 1;
        }
        return static_cast<std::size_t>(Concurrency);
    }

    //# ---------- Simulation ----------

    std::optional<f64> PreferredPhysicsStep;
    //# Fixed update rate.
    NODISCARD virtual f64 GetPreferredPhysicsStep() const noexcept { return 1.0/60; }
    //# Accumulator limit. Must be greater than the physics step.
    std::optional<f64> PreferredAccumulatorLimit;
    NODISCARD virtual f64 GetPreferredAccumulatorLimit() const noexcept
    {
        if (this->PreferredPhysicsStep)
        {
            return *this->PreferredPhysicsStep * 3.0;
        }
        return this->GetPreferredPhysicsStep() * 3.0;
    }
    //# How many steps to take for each step; so substeps.
    std::optional<std::size_t> PreferredPhysicsSteps;
    NODISCARD virtual std::size_t GetPreferredPhysicsSteps() const noexcept { return 1; }

    std::optional<LVec3F> PreferredGravity;
    NODISCARD virtual LVec3F GetPreferredGravity() const noexcept { return LVec3F{0.0, -9.81, 0.0}; }

    //#
    //# The gravity at a specific world translation.
    //# Must be deterministic for a given world state, as it is part of client prediction.
    //#
    //# @note Defaults to the uniform gravity of the physics system.
    //# @see #GetPreferredGravity
    //#
    NODISCARD virtual LVec3F GetGravityAt(LWorldVec3 const& Location) const noexcept
    {
        (void)Location;
        return this->PreferredGravity ? *this->PreferredGravity : this->GetPreferredGravity();
    }
};

} /* ~Namespace Jafg */
