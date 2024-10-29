// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectClass.h"

namespace Jafg
{

namespace Private
{
class LObjectContext;
}

class LWorld;
class JSubsystem;

/**
 * Represents a collection of subsystems that act inside a given
 * lifetime determined by its owning object.
 */
struct LSubsystemCollection final
{
    LSubsystemCollection() = delete;
    ENGINE_API explicit LSubsystemCollection(Private::LObjectContext* InOuter) : Outer(InOuter) { }
    PROHIBIT_REALLOC_OF_ANY_FROM(LSubsystemCollection)
    ENGINE_API ~LSubsystemCollection() = default;

    ENGINE_API void LocateAllSubsystemsOfClass(const LObjectClass* InClass);

    ENGINE_API void InitializeSubsystems();
    ENGINE_API void TearDownSubsystems();

    template <typename Predicate>
    FORCEINLINE void ForEachSubsystem(Predicate&& InPredicate)
    {
        for (JSubsystem* Subsystem : this->SubsystemInstances)
        {
            checkSlow( Subsystem )
            std::forward<Predicate>(InPredicate)(Subsystem);
            continue;
        }

        return;
    }

private:

    Private::LObjectContext*      Outer;
    TdhArray<const LObjectClass*> Subsystems;
    TdhArray<JSubsystem*>         SubsystemInstances;
};

} /* Namespace Jafg */
