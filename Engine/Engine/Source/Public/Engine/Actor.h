// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Engine/TickableObject.h"
#include "Actor.generated.h"

namespace Jafg
{

class LRendererComponent;
class LPhysicsComponent;

namespace EActorSweep
{

enum Type : u8
{
    Teleport,
    Sweep,
    SweepComplex,
};

} /* ~Namespace EActorSweep */

DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API AActor : public JObject, public LTickableObject
{
    GENERATED_CLASS_BODY()

    friend LWorld;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AActor)

public:

    virtual void BeginLife() override { Super::BeginLife(); }
    virtual void Tick(const float DeltaTime) override { check( this->IsGarbage() == false ) }
    virtual void EndLife() override;

    virtual void OnGarbage() override;

    FORCEINLINE auto   IsRendererComponentValid() const -> bool { return this->RendererComponent != nullptr; }
    FORCEINLINE auto   GetRendererComponent() const -> LRendererComponent* { return this->RendererComponent; }
    LPhysicsComponent* GetPhysicsComponent() const;

    void ChangeTransform(const LTransform& InTransform, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void AddTranslation(const LVector& InLocation, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void AddRotator(const LRotator& InRotator, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void AddScale(const LVector& InScale, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void SetTranslation(const LVector& InLocation, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void SetRotator(const LRotator& InRotator, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    void SetScale(const LVector& InScale, const EActorSweep::Type SweepType = EActorSweep::Teleport);
    FORCEINLINE auto GetTransform() const -> const LTransform& { return this->Transform; }
    FORCEINLINE auto GetTranslation() const -> const LVector& { return this->Transform.Translation; }
    FORCEINLINE auto GetRotator() const -> const LRotator& { return this->Transform.Rotator; }
    FORCEINLINE auto GetScale() const -> const LVector& { return this->Transform.Scale; }
    FORCEINLINE auto GetMutableTransform() -> LTransform& { return this->Transform; }
    FORCEINLINE auto GetMutableTranslation() -> LVector& { return this->Transform.Translation; }
    FORCEINLINE auto GetMutableRotator() -> LRotator& { return this->Transform.Rotator; }
    FORCEINLINE auto GetMutableScale() -> LVector& { return this->Transform.Scale; }

    FORCEINLINE auto GetTransformPtr() const -> const LTransform* { return &this->Transform; }

    FORCEINLINE auto CanEverTick() const -> bool { return this->bCanEverTick; }
    FORCEINLINE auto ShouldTick() const -> bool { return this->bShouldTick; }
    FORCEINLINE auto SetShouldTick(const bool bInShouldTick) -> void { this->bShouldTick = bInShouldTick; }

protected:

    void SetRendererComponent(LRendererComponent* InRendererComponent, const bool bFreeOld = true);
    void SetPhysicsComponent(LPhysicsComponent* InPhysicsComponent, const bool bFreeOld = true);
    FORCEINLINE void SetEverTickConstructorOnlyFlag() { this->bCanEverTick = true; }
    FORCEINLINE void CancelEverTickConstructorOnlyFlag() { this->bCanEverTick = false; }

    FORCEINLINE void DisableStrongActorContext() { this->bWeakContext = true; return; }

private:

    //# Make this virtual private to not confuse it with #ShouldTick.
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }

    LRendererComponent* RendererComponent = nullptr;
    LPhysicsComponent* PhysicsComponent = nullptr;

    LTransform Transform = { };

    //#
    //# Whether this Actor should ever be able to tick or not.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTick : 1 = false;

    //#
    //# Whether this actor should tick now or not. This flag does nothing if bCanEverTick is false.
    //#
    bool bShouldTick : 1  = true;

    bool bWeakContext : 1 = false;
};

} /* ~Namespace Jafg */

//#
//# Keep this include as every action with any actor will need a ton of function from this include.
//#
#include "ActorUtility.h"
