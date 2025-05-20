// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Subsystem.generated.h"

namespace Jafg
{

class LLocalEgo;
class LEngine;
class LCommandLineInterface;
struct LSubsystemCollection;

//#
//# A subsystem that shares its lifetime with their owning object.
//#
//# To create your own subsystem lifetime:
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JSubsystem : public JObjectBase
{
    friend LSubsystemCollection;

    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSubsystem)

    virtual void BeginLife() override final { Super::BeginLife(); }
    virtual void EndLife()   override final { Super::EndLife();   }
    virtual void OnGarbage() override final { Super::OnGarbage(); if (this->IsInitialized()) { this->TearDown(); } }

    //#
    //# Weather a subsystem should be created given its new context.
    //#
    //# @note The object will still be instanced if ShouldCreateSubsystem returns false, but will be killed
    //#       soon after.
    //#
    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const { return true; }
    virtual void Initialize(LSubsystemCollection& Collection);
    virtual void TearDown() { }

    FORCEINLINE bool IsInitialized() const { return this->bIsInitialized; }
    FORCEINLINE bool IsPriorityTearDown() const { return this->bPriorityTearDown; }
    //# Please see the #bPriorityTearDown documentation for more information. DO NOT JUST SET THIS TO TRUE.
    FORCEINLINE void SetPriorityTearDown(const bool bPriority) { this->bPriorityTearDown = bPriority; }

    LEngine* GetEngine() const;
    LLocalEgo* GetLocalEgo() const;
    LCommandLineInterface* GetCommandLineInterface() const;

private:

    //#
    //# This flag is generally only for subsystems that have very special rules and states attached to them.
    //# In general this **ONLY** applies to subsystems that are threaded.
    //# If you see yourself setting this flag on a NON-THREADED subsystem, please reconsider your design choice.
    //#
    bool bPriorityTearDown : 1 { false };

    bool bIsInitialized : 1 { false };
};

} /* Namespace Jafg */

/*
 * Include for children as they will need this always.
 */
#include "Subsystems/SubsystemCollection.h"
