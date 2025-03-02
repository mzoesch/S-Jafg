// Copyright mzoesch. All rights reserved.

#include "Subsystems/SubsystemCollection.h"
#include "Engine/ObjectBaseUtility.h"
#include "Subsystems/Subsystem.h"

void Jafg::LSubsystemCollection::DeferredInitialize(LObjectContext* InOuter)
{
    check( InOuter )
    check( this->Outer == nullptr )
    check( this->SubsystemInstances.IsEmpty() )

    this->Outer = InOuter;

    return;
}

void Jafg::LSubsystemCollection::InitializeSubsystems(const LObjectClass* InClass)
{
    check( this->Outer )
    check( this->OuterClass == nullptr )
    check( this->SubsystemInstances.IsEmpty() )

    LOG_VERBOSE(LogSubsystemCollection, "Locating all subsystems of class {}.", InClass->GetSpacedClassName())

    TdhArray<const LObjectClass*> SubsystemsClasses;
    Private::GObjectRegistry->GetRegisteredObjectsOfClass(InClass, SubsystemsClasses);

    for (const LObjectClass* SubsystemClass : SubsystemsClasses)
    {
        if (SubsystemClass->IsAbstract())
        {
            continue;
        }

        this->SubsystemInstances.Emplace(NewObject<JSubsystem>(this->Outer, SubsystemClass));

        continue;
    }

    for (int32 i = 0; i < this->SubsystemInstances.GetSize();)
    {
        JSubsystem* Subsystem = this->SubsystemInstances[i];

        checkSlow( Subsystem )

        if (Subsystem->IsInitialized())
        {
            ++i;
            continue;
        }

        if (Subsystem->ShouldCreateSubsystem(this->Outer))
        {
            LOG_TRACE(LogSubsystemCollection, "Initializing subsystem {}.", Subsystem->GetFullName())
            Subsystem->Initialize(*this);
            ++i;
            continue;
        }

        Subsystem->MarkAsGarbage();
        this->SubsystemInstances.RemoveAt(i);

        continue;
    }

    this->OuterClass = InClass;

    return;
}

void Jafg::LSubsystemCollection::TearDownSubsystems()
{
    check( this->Outer )

    LOG_VERBOSE(LogSubsystemCollection, "Tearing down {} subsystems for outer [{}].", this->SubsystemInstances.GetSize(), this->Outer->GetHumanReadableName())

    for (int32 i = 0; i < this->SubsystemInstances.GetSize(); ++i)
    {
        JSubsystem*& Subsystem = this->SubsystemInstances[i];
        checkSlow( Subsystem )

        if (Subsystem->IsPriorityTearDown())
        {
            Subsystem->KillYourSelfNow();
            Subsystem = nullptr;
            checkSlow( this->SubsystemInstances[i] == nullptr )
        }

        continue;
    }

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        if (Subsystem)
        {
            Subsystem->KillYourSelfNow();
        }

        continue;
    }

    this->SubsystemInstances.Empty();
    this->Outer = nullptr;

    return;
}

void Jafg::LSubsystemCollection::InitializeDependency(const LObjectClass* InStaticClass)
{
    JSubsystem* Subsystem = this->GetCheckedSubsystem(InStaticClass);

    if (Subsystem->IsInitialized())
    {
        return;
    }

    if (Subsystem->ShouldCreateSubsystem(this->Outer) == false)
    {
        LOG_WARNING(
            LogJafgInternal,
            "Wanted to initialize dependent subsystem {} but it does not want to be created.",
            Subsystem->GetFullName()
        )
        return;
    }

    LOG_TRACE(LogSubsystemCollection, "Initializing subsystem {}.", Subsystem->GetFullName())
    Subsystem->Initialize(*this);

    return;
}

Jafg::JSubsystem* Jafg::LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass)
{
    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        if (Subsystem->GetVTable() == InStaticClass)
        {
            return Subsystem;
        }

        continue;
    }

    return nullptr;
}

const Jafg::JSubsystem* Jafg::LSubsystemCollection::GetSubsystem(const LObjectClass* InStaticClass) const
{
    for (const JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        if (Subsystem->GetVTable() == InStaticClass)
        {
            return Subsystem;
        }

        continue;
    }

    return nullptr;
}
