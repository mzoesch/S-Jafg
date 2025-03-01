// Copyright mzoesch. All rights reserved.

#pragma once

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
    LSubsystemCollection() = default;
    ENGINE_API explicit LSubsystemCollection(LObjectContext* InOuter) : Outer(InOuter) { }
    PROHIBIT_REALLOC_OF_ANY_FORM(LSubsystemCollection)
    ENGINE_API ~LSubsystemCollection() = default;

    FORCEINLINE bool IsValid() const { return this->Outer != nullptr; }

    ENGINE_API void DeferredInitialize(LObjectContext* InOuter);

    ENGINE_API void InitializeSubsystems(const LObjectClass* InClass);
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
        check( Outer )

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
        check( Outer )

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

            panicMsgf
            (
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
        check( Outer )

        for (JSubsystem* Subsystem : this->SubsystemInstances)
        {
            checkSlow( Subsystem )
            std::forward<Predicate>(InPredicate)(CheckedStaticCast<TSubsystem*>(Subsystem));
            continue;
        }

        return;
    }

    ENGINE_API        JSubsystem* GetSubsystem(const LObjectClass* InStaticClass);
    ENGINE_API  const JSubsystem* GetSubsystem(const LObjectClass* InStaticClass) const;
    FORCEINLINE       JSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass);
    FORCEINLINE const JSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass) const;

    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE       TSubsystem* GetSubsystem();
    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE const TSubsystem* GetSubsystem() const;

    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE       TSubsystem* GetSubsystem(const LObjectClass* InStaticClass);
    template <typename TSubsystem, bool bAllowForNullptr = true>
    FORCEINLINE const TSubsystem* GetSubsystem(const LObjectClass* InStaticClass) const;

    template <typename TSubsystem>
    FORCEINLINE       TSubsystem* GetCheckedSubsystem();
    template <typename TSubsystem>
    FORCEINLINE const TSubsystem* GetCheckedSubsystem() const;

    template <typename TSubsystem>
    FORCEINLINE       TSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass);
    template <typename TSubsystem>
    FORCEINLINE const TSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass) const;

private:

    LObjectContext*       Outer = nullptr;
    TdhArray<JSubsystem*> SubsystemInstances;
};

FORCEINLINE JSubsystem* LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass)
{
    JSubsystem* Out = this->GetSubsystem(InStaticClass);
    check( Out )
    return Out;
}

FORCEINLINE const JSubsystem* LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass) const
{
    const JSubsystem* Out = this->GetSubsystem(InStaticClass);
    check( Out )
    return Out;
}

template <typename TSubsystem, bool bAllowForNullptr>
FORCEINLINE TSubsystem* LSubsystemCollection::GetSubsystem()
{
    return CheckedStaticCast<TSubsystem, bAllowForNullptr>(this->GetSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem, bool bAllowForNullptr>
const TSubsystem* LSubsystemCollection::GetSubsystem() const
{
    return CheckedStaticCast<TSubsystem, bAllowForNullptr>(this->GetSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem, bool bAllowForNullptr>
auto LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass) -> TSubsystem*
{
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(InStaticClass))>, bAllowForNullptr>(this->GetSubsystem(InStaticClass));
}

template <typename TSubsystem, bool bAllowForNullptr>
const TSubsystem* LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass) const
{
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(InStaticClass))>, bAllowForNullptr>(this->GetSubsystem(InStaticClass));
}

template <typename TSubsystem>
auto LSubsystemCollection::GetCheckedSubsystem() -> TSubsystem*
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem>
const TSubsystem* LSubsystemCollection::GetCheckedSubsystem() const
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem>
auto LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass) -> TSubsystem*
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(InStaticClass));
}

template <typename TSubsystem>
const TSubsystem* LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass) const
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(InStaticClass));
}

} /* ~Namespace Jafg */
