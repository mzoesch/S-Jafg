// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

class LObjectContext;
class LWorld;
class JSubsystem;

//#
//# Represents a collection of subsystems that act inside a given
//# lifetime determined by its owning object.
//#
struct LSubsystemCollection final
{
    LSubsystemCollection() = delete;
    ENGINE_API explicit LSubsystemCollection(LObjectContext* InOuter) : Outer(InOuter) { }
    PROHIBIT_REALLOC_OF_ANY_FORM(LSubsystemCollection)
    ENGINE_API ~LSubsystemCollection() = default;

    ENGINE_API void LocateAllSubsystemsOfClass(const LObjectClass* InClass);

    ENGINE_API void InitializeSubsystems();
    ENGINE_API void TearDownSubsystems();

    //#
    //# Try to initialize a subsystem of the given class. The target subsystem must be in this collection where this
    //# this method was called on.
    //# Use this method if you must preserve the order of initialization of subsystems in a given collection.
    //#
    ENGINE_API void InitializeDependency(const LObjectClass* InStaticClass);
    template <typename TSubsystem>
    FORCEINLINE void InitializeDependency() { this->InitializeDependency(TSubsystem::StaticClass()); }

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

    ENGINE_API auto GetSubsystem(const LObjectClass* InStaticClass) -> JSubsystem*;
    FORCEINLINE auto GetCheckedSubsystem(const LObjectClass* InStaticClass) -> JSubsystem*
    {
        JSubsystem* Out = this->GetSubsystem(InStaticClass);
        check( Out )
        return Out;
    }

    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE auto GetSubsystem() -> TSubsystem*
    {
        return CheckedStaticCast<TSubsystem, bAllowForNullptr>(this->GetSubsystem(TSubsystem::StaticClass()));
    }
    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE auto GetSubsystem(const LObjectClass* InStaticClass) -> TSubsystem*
    {
        return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(InStaticClass))>, bAllowForNullptr>(this->GetSubsystem(InStaticClass));
    }

    template <typename TSubsystem>
    FORCEINLINE auto GetCheckedSubsystem() -> TSubsystem*
    {
        return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(TSubsystem::StaticClass()));
    }
    template <typename TSubsystem>
    FORCEINLINE auto GetCheckedSubsystem(const LObjectClass* InStaticClass) -> TSubsystem*
    {
        return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(InStaticClass));
    }

private:

    LObjectContext*      Outer;
    TdhArray<const LObjectClass*> Subsystems;
    TdhArray<JSubsystem*>         SubsystemInstances;
};

} /* Namespace Jafg */
