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
    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        checkSlow( Subsystem )
        Subsystem->KillYourSelfNow();
        continue;
    }

    this->SubsystemInstances.Empty();

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
