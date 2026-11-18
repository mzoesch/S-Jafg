// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "Framework/EyeComponent.h"
#include "AvatarComponent.generated.h"

namespace Jafg
{

class APersonaController;
class LLocalEgo;

//#
//# Makes its actor possessable. A persona controller possesses an avatar (see #APersonaController::Possess) and acts
//# through it: The components of the actor react to the possession (see #AActorComponent::OnPossessionChanged), and
//# the controller sees the world through the eye of the avatar (see #GetEye).
//#
//# Anything can be an avatar. An actor has at most one avatar.
//# Actors that need more attach one actor per possible avatar or create them on the fly.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API AAvatarComponent: public AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AAvatarComponent)

public:

    virtual void OnAttach(AActor& InOwner) override;
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    NODISCARD FORCEINLINE constexpr bool IsPossessed() const noexcept { return !!this->Controller; }
    NODISCARD bool IsPossessedLocally() const noexcept;
    NODISCARD FORCEINLINE constexpr APersonaController* GetController() const noexcept { return this->Controller; }
    NODISCARD FORCEINLINE APersonaController* GetControllerChecked() const noexcept { check(this->IsPossessed()) return this->Controller; }
    NODISCARD LLocalEgo* GetLocalEgoIfPossessed() const noexcept;

    //#
    //# The eye that the controller of this avatar sees through. Switch it for other perspectives (e.g., first and
    //# third person). Falls back to the first eye in the hierarchy of the actor if none was set.
    //# @return Null if the actor has no eye at all.
    //#
    NODISCARD AEyeComponent* GetEye() const noexcept;
    //# @param NewEye Must be part of the actor of this avatar. Null to use the first eye of the actor.
    void SetEye(AEyeComponent* NewEye) noexcept;

    //# Calls #Fn for every eye in the hierarchy of the actor of this avatar.
    template<typename TFn> requires std::is_invocable_v<TFn, AEyeComponent&>
    void ForEachEye(TFn&& Fn) const
    {
        if (ASceneComponent const* Root{this->GetActorRoot()})
        {
            AEyeComponent::ForEachEyeIn(*Root, Fn);
        }
    }

    //# The root of the owning actor. Null if there is none (yet).
    NODISCARD ASceneComponent const* GetActorRoot() const noexcept;

    //# Jafg internal method. Do not use. @see #APersonaController::Possess.
    void _SetController(APersonaController* New);

private:

    APersonaController* Controller{};
    AEyeComponent* Eye{};
};

} /* ~Namespace Jafg */
