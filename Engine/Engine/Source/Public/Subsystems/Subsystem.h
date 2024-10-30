// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Subsystem.generated.h"

namespace Jafg
{

struct LSubsystemCollection;

/**
 * A subsystem that shares its lifetime with their owning object.
 *
 * To create your own subsystem lifetime:
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class JSubsystem : public JObject
{
    friend LSubsystemCollection;

    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSubsystem)

    virtual void BeginLife() override final {                   Super::BeginLife();       }
    virtual void EndLife()   override final {                   Super::EndLife();         }
    virtual void KillYourSelfNow(const bool bMayBeGarbage = false) override final
    {
        this->TearDown();
        Super::KillYourSelfNow(bMayBeGarbage);
    }

    /**
     * Weather a subsystem should be created given its new context.
     *
     * @note The object will still be instanced if ShouldCreateSubsystem returns false, but will be killed
     *       soon after.
     */
    virtual bool ShouldCreateSubsystem(const Private::LObjectContext* InOuter) const { return true; }
    virtual void Initialize(LSubsystemCollection& Collection)                        { }
    virtual void TearDown()                                                          { }
};

} /* Namespace Jafg */

/*
 * Include for children as they will need this always.
 */
#include "Subsystems/SubsystemCollection.h"
