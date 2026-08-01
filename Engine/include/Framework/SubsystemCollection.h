// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Subsystem.h"

namespace Jafg
{

class LWorld;
class JSubsystem;
struct LSubsystemCollection;

//# Common subsystem getters that an outer may want to use to avoid boilerplate.
#define SUBSYSTEM_COLLECTION_OUTER_GETTERS(Member, MySubsystem)                                                                                         \
    FORCEINLINE LSubsystemCollection      * GetCollection() noexcept { return &this->Member; }                                                          \
    FORCEINLINE LSubsystemCollection const* GetCollection() const noexcept { return &this->Member; }                                                    \
    template<algo::void_predicate<MySubsystem> TPredicate>                                                                                              \
    FORCEINLINE void ForEachSubsystem(TPredicate&& Predicate) { this->Member.ForEachSubsystem<MySubsystem>(std::forward<TPredicate>(Predicate)); }      \
    template<typename TSubsystem, algo::void_predicate<TSubsystem> TPredicate, bool bAllowMissCast = false>                                             \
        requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                                                \
    FORCEINLINE void ForEachSubtypeSubsystem(TPredicate&& Predicate)                                                                                    \
        { this->Member.ForEachSubsystem<TSubsystem, bAllowMissCast>(std::forward<TPredicate>(Predicate)); }                                             \
    template<algo::void_mutable_predicate<MySubsystem*> TPredicate>                                                                                     \
    FORCEINLINE void ForEachMutableSubsystem(TPredicate&& Predicate)                                                                                    \
        { this->Member.ForEachMutableSubsystem<MySubsystem>(std::forward<TPredicate>(Predicate)); }                                                     \
    template<typename TSubsystem, algo::void_mutable_predicate<TSubsystem*> TPredicate, bool bAllowMissCast = false>                                    \
        requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                                                \
    FORCEINLINE void ForEachMutableSubtypeSubsystem(TPredicate&& Predicate)                                                                             \
        { this->Member.ForEachMutableSubtypeSubsystem<TSubsystem, bAllowMissCast>(std::forward<TPredicate>(Predicate)); }                               \
    FORCEINLINE MySubsystem      * GetSubsystem(LJxxClass const* Class) { return this->Member.GetSubsystem<MySubsystem>(Class); }                       \
    FORCEINLINE MySubsystem const* GetSubsystem(LJxxClass const* Class) const { return this->Member.GetSubsystem<MySubsystem>(Class); }                 \
    FORCEINLINE MySubsystem      * GetSubsystemChecked(LJxxClass const* Class) { return this->Member.GetSubsystemChecked<MySubsystem>(Class); }         \
    FORCEINLINE MySubsystem const* GetSubsystemChecked(LJxxClass const* Class) const { return this->Member.GetSubsystemChecked<MySubsystem>(Class); }   \
    FORCEINLINE MySubsystem      * GetSubsystemAsserted(LJxxClass const* Class) { return this->Member.GetSubsystemAsserted<MySubsystem>(Class); }       \
    FORCEINLINE MySubsystem const* GetSubsystemAsserted(LJxxClass const* Class) const { return this->Member.GetSubsystemAsserted<MySubsystem>(Class); } \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem      * GetSubsystem() { return this->Member.GetSubsystem<TSubsystem>(); }                                                    \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem const* GetSubsystem() const { return this->Member.GetSubsystem<TSubsystem>(); }                                              \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem      * GetSubsystemChecked() { return this->Member.GetSubsystemChecked<TSubsystem>(); }                                      \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem const* GetSubsystemChecked() const { return this->Member.GetSubsystemChecked<TSubsystem>(); }                                \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem      * GetSubsystemAsserted() { return this->Member.GetSubsystemAsserted<TSubsystem>(); }                                    \
    template<typename TSubsystem> requires(std::is_base_of_v<MySubsystem, TSubsystem> && !std::is_same_v<MySubsystem, TSubsystem>)                      \
    FORCEINLINE TSubsystem const* GetSubsystemAsserted() const { return this->Member.GetSubsystemAsserted<TSubsystem>(); }

//# Represents a collection of subsystems that act inside a given lifetime determined by its owning object.
struct LSubsystemCollection final
{
    constexpr LSubsystemCollection() noexcept = delete;
    constexpr LSubsystemCollection(LString InFriendlyName) noexcept : FriendlyName(std::move(InFriendlyName)) {}
    void InitializeDeferred(LClassOuter* InOuter, LString InFriendlyName = {}) noexcept
    {
        check( InOuter && this->Outer == nullptr )
        check( this->Class == nullptr )
        check( this->SubsystemInstances.empty() )
        this->Outer = InOuter;
        if (InFriendlyName.empty() == false)
        {
            this->FriendlyName = std::move(InFriendlyName);
        }
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LSubsystemCollection)
    inline ~LSubsystemCollection() noexcept { check( this->SubsystemInstances.empty() ) }

    ENGINE_API void InitializeSubsystems(TSubclassOf<JSubsystem> Class, bool bRegisterDeferredDelegate = true);
    template<typename TSubsystem> requires std::is_base_of_v<JSubsystem, TSubsystem>
    FORCEINLINE void InitializeSubsystems(bool bRegisterDeferredDelegate = true) noexcept { this->InitializeSubsystems(TSubsystem::StaticClass(), bRegisterDeferredDelegate); }

    //#
    //# Same as #InitializeSubsystems but only for deferred subsystems.
    //# This will be automatically called if bRegisterDeferredDelegate is set during the initial initializing phase.
    //# You may call this yourselves if you want a custom behavior.
    //#
    ENGINE_API void InitializeSubsystemsForDeferred();

    bool OnForeignPluginLoaded(LLoadedPlugin const& Plugin);

    //#
    //# Tears down all subsystems in this collection.
    //# The in predicate is called inbetween tearing down the priority and non-priority subsystems.
    //#
    template<typename TPredicate> requires std::is_invocable_r_v<void, TPredicate>
    FORCEINLINE void TearDownSubsystems(TPredicate&& Predicate);
    FORCEINLINE void TearDownSubsystems();

    FORCEINLINE bool IsOuterValid() const noexcept { return this->Outer != nullptr; }
    FORCEINLINE LClassOuter const* GetOuter() const noexcept { return this->Outer; }
    FORCEINLINE bool IsClassValid() const noexcept { return this->Class != nullptr; }
    FORCEINLINE TSubclassOf<JSubsystem> GetClass() const noexcept { return this->Class; }
    FORCEINLINE TArray<JSubsystem*> const& GetSubsystems() const noexcept { return this->SubsystemInstances; }

    //#
    //# Try to initialize a subsystem of the given class. The target subsystem must be in this collection where this
    //# this method was called on.
    //# Use this method if you must preserve the order of initialization of subsystems in a given collection.
    //#
    ENGINE_API void InitializeDependency(JSubsystem* Requester, TSubclassOf<JSubsystem> Class);
    template<typename TSubsystem>
    FORCEINLINE void InitializeDependency(JSubsystem* Requester) noexcept { this->InitializeDependency(Requester, TSubsystem::StaticClass()); }

    //# Execute the given predicate on all subsystems in this collection.
    template<algo::void_predicate<JSubsystem> TPredicate>
    FORCEINLINE void ForEachSubsystem(TPredicate&& Predicate) const;
    template<algo::void_mutable_predicate<JSubsystem*> TPredicate>
    FORCEINLINE void ForEachMutableSubsystem(TPredicate&& Predicate);
    //# Same as above but will cast the subsystem to the given type. Only use this if all subsystems in this collection are of the #TSubsystem type.
    template<typename TSubsystem, algo::void_predicate<TSubsystem> TPredicate> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE void ForEachSubsystem(TPredicate&& Predicate) const;
    template<typename TSubsystem, algo::void_mutable_predicate<TSubsystem*> TPredicate> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE void ForEachMutableSubsystem(TPredicate&& Predicate);
    //# Use this if you only want to execute the predicate on subsystems that are a subtype of the collection type.
    template <typename TSubsystem, algo::void_predicate<TSubsystem> TPredicate, bool bAllowMissCast = false> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE void ForEachSubtypeSubsystem(TPredicate&& Predicate) const;
    template <typename TSubsystem, algo::void_mutable_predicate<TSubsystem*> TPredicate, bool bAllowMissCast = false> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE void ForEachMutableSubtypeSubsystem(TPredicate&& Predicate);

    ENGINE_API  JSubsystem const* GetSubsystem(TSubclassOf<JSubsystem> Class) const;
    ENGINE_API  JSubsystem*       GetSubsystem(TSubclassOf<JSubsystem> Class);
    FORCEINLINE JSubsystem const* GetSubsystemChecked(TSubclassOf<JSubsystem> Class) const { JSubsystem const* Out { this->GetSubsystem(Class) }; check( Out ) return Out; }
    FORCEINLINE JSubsystem*       GetSubsystemChecked(TSubclassOf<JSubsystem> Class) { JSubsystem* Out { this->GetSubsystem(Class) }; check( Out ) return Out; }
    FORCEINLINE JSubsystem const* GetSubsystemAsserted(TSubclassOf<JSubsystem> Class) const { JSubsystem const* Out { this->GetSubsystem(Class) }; jassert( Out ) return Out; }
    FORCEINLINE JSubsystem*       GetSubsystemAsserted(TSubclassOf<JSubsystem> Class) { JSubsystem* Out { this->GetSubsystem(Class) }; jassert( Out ) return Out; }

    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystem() const { return StaticCast<TSubsystem>(this->GetSubsystem(TSubsystem::StaticClass())); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystem() { return StaticCast<TSubsystem>(this->GetSubsystem(TSubsystem::StaticClass())); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystemChecked() const { return StaticCastChecked<TSubsystem>(this->GetSubsystemChecked(TSubsystem::StaticClass())); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystemChecked() { return StaticCastChecked<TSubsystem>(this->GetSubsystemChecked(TSubsystem::StaticClass())); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystemAsserted() const { return StaticCastAsserted<TSubsystem>(this->GetSubsystemAsserted(TSubsystem::StaticClass())); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystemAsserted() { return StaticCastAsserted<TSubsystem>(this->GetSubsystemAsserted(TSubsystem::StaticClass())); }

    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystem(TSubclassOf<JSubsystem> Class) const { return StaticCast<TSubsystem>(this->GetSubsystem(Class)); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystem(TSubclassOf<JSubsystem> Class) { return StaticCast<TSubsystem>(this->GetSubsystem(Class)); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystemChecked(TSubclassOf<JSubsystem> Class) const { return StaticCastChecked<TSubsystem>(this->GetSubsystemChecked(Class)); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystemChecked(TSubclassOf<JSubsystem> Class) { return StaticCastChecked<TSubsystem>(this->GetSubsystemChecked(Class)); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem const* GetSubsystemAsserted(TSubclassOf<JSubsystem> Class) const { return StaticCastAsserted<TSubsystem>(this->GetSubsystemAsserted(Class)); }
    template<typename TSubsystem> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
    FORCEINLINE TSubsystem*       GetSubsystemAsserted(TSubclassOf<JSubsystem> Class) { return StaticCastAsserted<TSubsystem>(this->GetSubsystemAsserted(Class)); }

private:

    void TearDownPrioritySubsystems();
    void TearDownNonPrioritySubsystems();

    LString FriendlyName;

    //# The outer to use for all subsystems.
    LClassOuter* Outer{};

    //# The class to use.
    TSubclassOf<JSubsystem> Class;

    //# All initialized current subsystem instances.
    TArray<JSubsystem*> SubsystemInstances;

    LDelegateHandle OnForeignPluginLoadedHandle;
};

template<typename TPredicate> requires std::is_invocable_r_v<void, TPredicate>
FORCEINLINE void LSubsystemCollection::TearDownSubsystems(TPredicate&& Predicate)
{
    this->TearDownPrioritySubsystems();
    std::invoke(std::forward<TPredicate>(Predicate));
    this->TearDownNonPrioritySubsystems();

    return;
}

FORCEINLINE void LSubsystemCollection::TearDownSubsystems()
{
    this->TearDownPrioritySubsystems();
    this->TearDownNonPrioritySubsystems();

    return;
}

template<algo::void_predicate<JSubsystem> TPredicate>
FORCEINLINE void LSubsystemCollection::ForEachSubsystem(TPredicate&& Predicate) const
{
    check( this->Outer )

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        std::invoke(std::forward<TPredicate>(Predicate), *Subsystem);

        continue;
    }

    return;
}

template<algo::void_mutable_predicate<JSubsystem*> TPredicate>
FORCEINLINE void LSubsystemCollection::ForEachMutableSubsystem(TPredicate&& Predicate)
{
    check( this->Outer )

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        std::invoke(std::forward<TPredicate>(Predicate), Subsystem);

        continue;
    }

    return;
}

template<typename TSubsystem, algo::void_predicate<TSubsystem> TPredicate> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
FORCEINLINE void LSubsystemCollection::ForEachSubsystem(TPredicate&& Predicate) const
{
    check( this->Outer )
    check( TSubsystem::StaticClass() == this->Class )

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        std::invoke(std::forward<TPredicate>(Predicate), *StaticCast<TSubsystem>(Subsystem));
        continue;
    }

    return;
}

template<typename TSubsystem, algo::void_mutable_predicate<TSubsystem*> TPredicate> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
FORCEINLINE void LSubsystemCollection::ForEachMutableSubsystem(TPredicate&& Predicate)
{
    check(this->Outer)
    check(&TSubsystem::StaticClass() == *this->Class)
    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        std::invoke(std::forward<TPredicate>(Predicate), StaticCastChecked<TSubsystem>(Subsystem));
    }
    return;
}

template<typename TSubsystem, algo::void_predicate<TSubsystem> TPredicate, bool bAllowMissCast> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
FORCEINLINE void LSubsystemCollection::ForEachSubtypeSubsystem(TPredicate&& Predicate) const
{
    check(this->Outer)
    check(TSubsystem::StaticClass().DerivesFrom(this->Class))

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        check(Subsystem)
        if (TSubsystem* CastedSubsystem{DynamicCast<TSubsystem>(Subsystem)})
        {
            std::invoke(std::forward<TPredicate>(Predicate), *CastedSubsystem);
            continue;
        }

        if constexpr (bAllowMissCast)
        {
            continue;
        }

        panicMsgf("Failed to dynamically cast subsystem [{}] to [{}]."
            , Subsystem->GetNameAsString()
            , TSubsystem::StaticClass().GetFullyQualifiedName()
            )
    }

    return;
}

template<typename TSubsystem, algo::void_mutable_predicate<TSubsystem*> TPredicate, bool bAllowMissCast> requires(std::is_base_of_v<JSubsystem, TSubsystem> && !std::is_same_v<JSubsystem, TSubsystem>)
FORCEINLINE void LSubsystemCollection::ForEachMutableSubtypeSubsystem(TPredicate&& Predicate)
{
    check(this->Outer)
    check(TSubsystem::StaticClass().DerivesFrom(this->Class))

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        check(Subsystem)
        if (TSubsystem* CastedSubsystem{DynamicCast<TSubsystem>(Subsystem)})
        {
            std::invoke(std::forward<TPredicate>(Predicate), CastedSubsystem);
            continue;
        }

        if constexpr (bAllowMissCast)
        {
            continue;
        }

        panicMsgf("Failed to dynamically cast subsystem [{}] to [{}]."
            , Subsystem->GetNameAsString()
            , TSubsystem::StaticClass()->GetFullyQualifiedName()
        )
    }

    return;
}

} /* ~Namespace Jafg */
