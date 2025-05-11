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

//# Common subsystem getters that an outer may want to use to avoid boilerplate.
#define SUBSYSTEM_COLLECTION_OUTER_GETTERS(Member, MySubsystem)                                                                                                          \
    FORCEINLINE       LSubsystemCollection* GetCollection() { return &this->Member; }                                                                                    \
    FORCEINLINE const LSubsystemCollection* GetCollection() const { return &this->Member; }                                                                              \
    FORCEINLINE       MySubsystem* GetSubsystem(const LObjectClass* InStaticClass) { return this->Member.GetSubsystem<MySubsystem>(InStaticClass); }                     \
    FORCEINLINE const MySubsystem* GetSubsystem(const LObjectClass* InStaticClass) const { return this->Member.GetSubsystem<MySubsystem>(InStaticClass); }               \
    FORCEINLINE       MySubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass) { return this->Member.GetCheckedSubsystem<MySubsystem>(InStaticClass); }       \
    FORCEINLINE const MySubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass) const { return this->Member.GetCheckedSubsystem<MySubsystem>(InStaticClass); } \
    template <typename TSubsystem, bool bAllowForNullptr = true>                               \
    FORCEINLINE TSubsystem* GetSubsystem()                                                     \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetSubsystem<TSubsystem, bAllowForNullptr>();                      \
    }                                                                                          \
    template <typename TSubsystem, bool bAllowForNullptr = true>                               \
    FORCEINLINE const TSubsystem* GetSubsystem() const                                         \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetSubsystem<TSubsystem, bAllowForNullptr>();                      \
    }                                                                                          \
    template <typename TSubsystem, bool bAllowForNullptr = true>                               \
    FORCEINLINE TSubsystem* GetSubsystem(const LObjectClass* InStaticClass)                    \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetSubsystem<TSubsystem, bAllowForNullptr>(InStaticClass);         \
    }                                                                                          \
    template <typename TSubsystem, bool bAllowForNullptr = true>                               \
    FORCEINLINE const TSubsystem* GetSubsystem(const LObjectClass* InStaticClass) const        \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetSubsystem<TSubsystem, bAllowForNullptr>(InStaticClass);         \
    }                                                                                          \
    template <typename TSubsystem>                                                             \
    FORCEINLINE TSubsystem* GetCheckedSubsystem()                                              \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetCheckedSubsystem<TSubsystem>();                                 \
    }                                                                                          \
    template <typename TSubsystem>                                                             \
    FORCEINLINE const TSubsystem* GetCheckedSubsystem() const                                  \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetCheckedSubsystem<TSubsystem>();                                 \
    }                                                                                          \
    template <typename TSubsystem>                                                             \
    FORCEINLINE TSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass)             \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetCheckedSubsystem<TSubsystem>(InStaticClass);                    \
    }                                                                                          \
    template <typename TSubsystem>                                                             \
    FORCEINLINE const TSubsystem* GetCheckedSubsystem(const LObjectClass* InStaticClass) const \
    {                                                                                          \
        static_assert(std::is_base_of_v<MySubsystem, TSubsystem>);                             \
        return this->Member.GetCheckedSubsystem<TSubsystem>(InStaticClass);                    \
    }

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

    //#
    //# Tears down all subsystems in this collection.
    //# The in predicate is called inbetween tearing down the priority and non-priority subsystems.
    //#
    template <typename Predicate>
    FORCEINLINE void TearDownSubsystems(Predicate&& InPredicate);
    FORCEINLINE void TearDownSubsystems();

    FORCEINLINE bool IsOuterValid() const { return this->Outer != nullptr; }
    FORCEINLINE auto GetOuter() const -> const LObjectContext* { return this->Outer; }
    FORCEINLINE bool IsOuterClassValid() const { return this->OuterClass != nullptr; }
    FORCEINLINE auto GetOuterClass() const -> const LObjectClass* { return this->OuterClass; }
    FORCEINLINE auto GetSubsystems() const -> const TArray<JSubsystem*>& { return this->SubsystemInstances; }

    //#
    //# Try to initialize a subsystem of the given class. The target subsystem must be in this collection where this
    //# this method was called on.
    //# Use this method if you must preserve the order of initialization of subsystems in a given collection.
    //#
    ENGINE_API void InitializeDependency(const LObjectClass* InStaticClass);
    template <typename TSubsystem>
    FORCEINLINE void InitializeDependency() { this->InitializeDependency(TSubsystem::StaticClass()); }

    //# Use this if you execute the predicate on all subsystems in the collection and do not need any specific subsystem.
    template <typename Predicate>
    FORCEINLINE void ForEachSubsystem(Predicate&& InPredicate);

    //# Use this if you execute the predicate on all subsystems in the collection with the type it was initialized with.
    template <typename TSubsystem, typename Predicate>
    FORCEINLINE void ForEachSubsystem(Predicate&& InPredicate);

    //# Use this if you only want to execute the predicate on subsystems that are a subtype of the collection type.
    template <typename TSubsystem, typename Predicate, bool bAllowMissCast = false>
    FORCEINLINE void ForEachSubtypeSubsystem(Predicate&& InPredicate);

    ENGINE_API JSubsystem* GetSubsystem(const LObjectClass* InStaticClass);
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

    ENGINE_API void TearDownPrioritySubsystems();
    ENGINE_API void TearDownNonPrioritySubsystems();

    LObjectContext*     Outer = nullptr;
    const LObjectClass* OuterClass = nullptr;
    TArray<JSubsystem*> SubsystemInstances;
};

template<typename Predicate>
FORCEINLINE void LSubsystemCollection::TearDownSubsystems(Predicate&& InPredicate)
{
    this->TearDownPrioritySubsystems();
    InPredicate();
    this->TearDownNonPrioritySubsystems();

    return;
}

FORCEINLINE void LSubsystemCollection::TearDownSubsystems()
{
    this->TearDownPrioritySubsystems();
    this->TearDownNonPrioritySubsystems();
    return;
}

template <typename Predicate>
FORCEINLINE void LSubsystemCollection::ForEachSubsystem(Predicate&& InPredicate)
{
    check( this->Outer )

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        std::forward<Predicate>(InPredicate)(Subsystem);
        continue;
    }

    return;
}

template <typename TSubsystem, typename Predicate>
FORCEINLINE void LSubsystemCollection::ForEachSubsystem(Predicate&& InPredicate)
{
    check( this->Outer )
    check( TSubsystem::StaticClass() == this->OuterClass )

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        std::forward<Predicate>(InPredicate)(CheckedStaticCast<TSubsystem>(Subsystem));
        continue;
    }

    return;
}

template <typename TSubsystem, typename Predicate, bool bAllowMissCast>
FORCEINLINE void LSubsystemCollection::ForEachSubtypeSubsystem(Predicate&& InPredicate)
{
    check( this->Outer )
    check( TSubsystem::StaticClass()->DerivesFrom(this->OuterClass) )

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
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(TSubsystem::StaticClass()))>, bAllowForNullptr>(this->GetSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem, bool bAllowForNullptr>
FORCEINLINE const TSubsystem* LSubsystemCollection::GetSubsystem() const
{
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(TSubsystem::StaticClass()))>, bAllowForNullptr>(this->GetSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem, bool bAllowForNullptr>
FORCEINLINE TSubsystem* LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass)
{
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(InStaticClass))>, bAllowForNullptr>(this->GetSubsystem(InStaticClass));
}

template <typename TSubsystem, bool bAllowForNullptr>
FORCEINLINE const TSubsystem* LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass) const
{
    return CheckedStaticCast<TSubsystem, std::remove_pointer_t<decltype(this->GetSubsystem(InStaticClass))>, bAllowForNullptr>(this->GetSubsystem(InStaticClass));
}

template <typename TSubsystem>
FORCEINLINE TSubsystem* LSubsystemCollection::GetCheckedSubsystem()
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem>
FORCEINLINE const TSubsystem* LSubsystemCollection::GetCheckedSubsystem() const
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(TSubsystem::StaticClass()));
}

template <typename TSubsystem>
FORCEINLINE TSubsystem* LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass)
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(InStaticClass));
}

template <typename TSubsystem>
FORCEINLINE const TSubsystem* LSubsystemCollection::GetCheckedSubsystem(const LObjectClass* InStaticClass) const
{
    return CheckedStaticCast<TSubsystem>(this->GetCheckedSubsystem(InStaticClass));
}

} /* ~Namespace Jafg */
