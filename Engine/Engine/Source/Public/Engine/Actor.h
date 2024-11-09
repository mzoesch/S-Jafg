// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Tickable/TickableObject.h"
#include "Actor.generated.h"

namespace Jafg
{

class LRendererComponent;

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
    virtual void EndLife();

    virtual void OnGarbage() override;

    FORCEINLINE auto IsRendererComponentValid() const -> bool { return this->RendererComponent != nullptr; }
    FORCEINLINE auto GetRendererComponent() const -> LRendererComponent* { return this->RendererComponent; }

    FORCEINLINE auto CanEverTick() const -> bool { return this->bCanEverTick; }
    FORCEINLINE auto ShouldTick() const -> bool { return this->bShouldTick; }
    FORCEINLINE auto SetShouldTick(const bool bInShouldTick) -> void { this->bShouldTick = bInShouldTick; }

protected:

    void SetRendererComponent(LRendererComponent* InRendererComponent, const bool bFreeOld = true);
    FORCEINLINE void SetEverTickConstructorOnlyFlag() { this->bCanEverTick = true; }

private:

    /** Make this virtual private to not confuse it with #ShouldTick. */
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }

    LRendererComponent* RendererComponent = nullptr;

    /**
     * Whether this Actor should ever be able to tick or not.
     * This bool flag can only be set in the constructor of the actor - new objects if this class
     * will not be registered in the context tickable registry.
     */
    bool bCanEverTick : 1 = false;

    /**
     * Whether this actor should tick now or not. This flag does nothing if bCanEverTick is false.
     */
    bool bShouldTick : 1  = true;
};

} /* ~Namespace Jafg. */

/*
 * Keep this include as every action with any actor will need a ton of function from this include.
 */
#include "ActorUtility.h"
