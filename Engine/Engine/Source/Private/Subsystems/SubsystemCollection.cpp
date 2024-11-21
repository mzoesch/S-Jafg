// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/SubsystemCollection.h"
#include "Engine/ObjectBaseUtility.h"
#include "Subsystems/Subsystem.h"

void Jafg::LSubsystemCollection::LocateAllSubsystemsOfClass(const LObjectClass* InClass)
{
    LOG_VERBOSE(LogSubsystemCollection, "Locating all subsystems of class {}.", InClass->GetSpacedClassName())

    Private::GObjectRegistry->GetRegisteredObjectsOfClass(InClass, this->Subsystems);

    for (const LObjectClass* SubsystemClass : this->Subsystems)
    {
        if (SubsystemClass->IsAbstract())
        {
            continue;
        }

        this->SubsystemInstances.Emplace(NewObject<JSubsystem>(this->Outer, SubsystemClass));

        continue;
    }

    return;
}

void Jafg::LSubsystemCollection::InitializeSubsystems()
{
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

    return;
}

void Jafg::LSubsystemCollection::TearDownSubsystems()
{
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
