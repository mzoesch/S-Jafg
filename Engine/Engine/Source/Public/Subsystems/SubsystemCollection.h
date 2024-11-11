// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

namespace Private
{

class LObjectContext;

} /* ~Namespace Private */

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

    template <typename TSubsystem, typename Predicate, bool bAllowMissCast = false>
    FORCEINLINE void ForEachSubsystem(Predicate&& InPredicate)
    {
        for (JSubsystem* Subsystem : this->SubsystemInstances)
        {
            checkSlow( Subsystem )
            if (TSubsystem* SubsystemT = DynamicCast<TSubsystem>(Subsystem); SubsystemT)
            {
                std::forward<Predicate>(InPredicate)(SubsystemT);
                continue;
            }

            if constexpr (bAllowMissCast)
            {
                continue;
            }

            panicMsgf(
                "Failed to dynamically cast subsystem [{}] to [{}].",
                Subsystem->GetFullName(),
                TSubsystem::StaticClass()->GetSpacedClassName()
            )
            continue;
        }

        return;
    }

    template <typename TSubsystem, typename Predicate>
    FORCEINLINE void ForEachSubsystemUnsafe(Predicate&& InPredicate)
    {
        for (JSubsystem* Subsystem : this->SubsystemInstances)
        {
            checkSlow( Subsystem )
            std::forward<Predicate>(InPredicate)(reinterpret_cast<TSubsystem*>(Subsystem));
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
