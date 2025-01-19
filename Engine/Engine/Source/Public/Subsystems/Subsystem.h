// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Subsystem.generated.h"

namespace Jafg
{

class LEngine;
class LApplicationInstance;
class LCommandLineInterface;
struct LSubsystemCollection;

/**
 * A subsystem that shares its lifetime with their owning object.
 *
 * To create your own subsystem lifetime:
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JSubsystem : public Private::JObjectBase
{
    friend LSubsystemCollection;

    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSubsystem)

    virtual void BeginLife() override final { Super::BeginLife();                   }
    virtual void EndLife()   override final { Super::EndLife();                     }
    virtual void OnGarbage() override final { Super::OnGarbage(); this->TearDown(); }

    /**
     * Weather a subsystem should be created given its new context.
     *
     * @note The object will still be instanced if ShouldCreateSubsystem returns false, but will be killed
     *       soon after.
     */
    virtual bool ShouldCreateSubsystem(const Private::LObjectContext* InOuter) const { return true; }
    virtual void Initialize(LSubsystemCollection& Collection);
    virtual void TearDown() { }

    FORCEINLINE bool IsInitialized() const { return this->bIsInitialized; }
    FORCEINLINE bool IsPriorityTearDown() const { return this->bPriorityTearDown; }
    FORCEINLINE void SetPriorityTearDown(const bool bPriority) { this->bPriorityTearDown = bPriority; }

    auto GetEngine() const -> LEngine*;
    auto GetCommandLineInterface() const -> LCommandLineInterface*;
    auto GetApplicationInstance() const -> LApplicationInstance*;

private:

    bool bPriorityTearDown = false;
    bool bIsInitialized = false;
};

} /* Namespace Jafg */

/*
 * Include for children as they will need this always.
 */
#include "Subsystems/SubsystemCollection.h"
