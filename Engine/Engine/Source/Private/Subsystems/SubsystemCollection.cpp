// Copyright mzoesch. All rights reserved.

#include "Subsystems/SubsystemCollection.h"
#include "Engine/Carnifex.h"
#include "Engine/ObjectBaseUtility.h"
#include "Subsystems/Subsystem.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"

void Jafg::LSubsystemCollection::DeferredInitialize(LObjectContext* InOuter, const bool bAllowDeferredSubsystems /* = false */)
{
    check( InOuter )
    check( this->Outer == nullptr )
    check( this->SubsystemInstances.IsEmpty() )

    this->Outer = InOuter;
    this->bAllowDeferredSubsystems = bAllowDeferredSubsystems;

    return;
}

void Jafg::LSubsystemCollection::InitializeSubsystems(const LObjectClass* InClass)
{
    STAT_CYCLE_FUNCTION()

    check( this->Outer )
    check( this->OuterClass == nullptr )
    check( this->SubsystemInstances.IsEmpty() )

    LOG_VERBOSE(LogSubsystemCollection, "Locating all subsystems of class {}.", InClass->GetSpacedClassName())

    TArray<const LObjectClass*> SubsystemsClasses;
    Private::GObjectRegistry->GetRegisteredObjectsOfClass(InClass, &SubsystemsClasses);

    for (const LObjectClass* SubsystemClass : SubsystemsClasses)
    {
        if (SubsystemClass->IsAbstract())
        {
            continue;
        }

        this->SubsystemInstances.Emplace(NewObject<JSubsystem>(this->Outer, SubsystemClass));

        continue;
    }

    for (i32 i = 0; i < this->SubsystemInstances.GetSize();)
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

    if (GEngine)
    {
        this->OnForeignPluginLoadedHandle = GEngine->OnForeignPluginLoaded.AddMember(this, &LSubsystemCollection::OnForeignPluginLoaded);
    }

    return;
}

void Jafg::LSubsystemCollection::InitializeSubsystemsDeferredOnly()
{
    STAT_CYCLE_FUNCTION()

    check( this->Outer )
    check( this->OuterClass != nullptr )
    check( this->IntermediateInstances.IsEmpty() )

    LOG_VERBOSE(LogSubsystemCollection, "Locating all subsystems of class {} that are not loaded.", this->OuterClass->GetSpacedClassName())

    TArray<const LObjectClass*> SubsystemsClasses;
    Private::GObjectRegistry->GetRegisteredObjectsOfClass(this->OuterClass, &SubsystemsClasses);

    for (const LObjectClass* SubsystemClass : SubsystemsClasses)
    {
        if (SubsystemClass->IsAbstract())
        {
            continue;
        }

        if (this->SubsystemInstances.ContainsByPredicate([SubsystemClass](const JSubsystem* Subsystem) -> bool
        {
            if (Subsystem->GetVTable() == SubsystemClass)
            {
                return true;
            }

            return false;
        }) == false)
        {
            LOG_VERBOSE(LogSubsystemCollection, "Found potential subsystem [{}].", SubsystemClass->GetSpacedClassName() )
            this->IntermediateInstances.Emplace(NewObject<JSubsystem>(this->Outer, SubsystemClass));
        }

        continue;
    }

    for (i32 i = 0; i < this->IntermediateInstances.GetSize();)
    {
        JSubsystem* Subsystem = this->IntermediateInstances[i];

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
        this->IntermediateInstances.RemoveAt(i);

        continue;
    }

    this->SubsystemInstances.Append(std::move(this->IntermediateInstances));

    return;
}

void Jafg::LSubsystemCollection::OnForeignPluginLoaded(const LObjectContext* InStaticClassContainer)
{
    check( InStaticClassContainer )

    if (this->bAllowDeferredSubsystems)
    {
        this->InitializeSubsystemsDeferredOnly();
    }

    return;
}

void Jafg::LSubsystemCollection::InitializeDependency(const LObjectClass* InStaticClass)
{
    JSubsystem* Subsystem = this->GetSubsystem(InStaticClass);

    if (Subsystem == nullptr)
    {
        for (JSubsystem* InnerSubsystem : this->IntermediateInstances)
        {
            checkSlow( Subsystem )
            if (InnerSubsystem->GetVTableChecked() == InStaticClass)
            {
                Subsystem = InnerSubsystem;
                break;
            }

            continue;
        }
    }

    jassert( Subsystem )

    if (Subsystem->IsInitialized())
    {
        return;
    }

    if (Subsystem->ShouldCreateSubsystem(this->Outer) == false)
    {
        LOG_WARNING
        (
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
        if (Subsystem->GetVTableChecked() == InStaticClass)
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
        if (Subsystem->GetVTableChecked() == InStaticClass)
        {
            return Subsystem;
        }

        continue;
    }

    return nullptr;
}

void Jafg::LSubsystemCollection::TearDownPrioritySubsystems()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )
    check( this->Outer )

    i32 SubsystemCount { 0 };

    for (i32 i = 0; i < this->SubsystemInstances.GetSize(); ++i)
    {
        JSubsystem*& Subsystem = this->SubsystemInstances[i];
        checkSlow( Subsystem )

        if (Subsystem->IsPriorityTearDown())
        {
            Subsystem->MarkAsGarbage();
            Subsystem = nullptr;
            checkSlow( this->SubsystemInstances[i] == nullptr )
            ++SubsystemCount;
        }

        continue;
    }

    if (SubsystemCount > 0)
    {
        LOG_VERBOSE(LogSubsystemCollection, "Tore down {} priority subsystems for outer [{}].", SubsystemCount, this->Outer->GetHumanReadableName())
    }

    this->Outer->GetCarnifex()->KillAllGarbageChildren();

    return;
}

void Jafg::LSubsystemCollection::TearDownNonPrioritySubsystems()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )
    check( this->Outer )

    LOG_VERBOSE(LogSubsystemCollection, "Tearing down {} subsystems for outer [{}].", this->SubsystemInstances.GetSize(), this->Outer->GetHumanReadableName())

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        if (Subsystem)
        {
            Subsystem->MarkAsGarbage();
        }

        continue;
    }

    this->SubsystemInstances.Empty();
    this->Outer->GetCarnifex()->KillAllGarbageChildren();
    this->Outer = nullptr;
    this->OuterClass = nullptr;

    if (GEngine)
    {
        GEngine->OnForeignPluginLoaded.Remove(&this->OnForeignPluginLoadedHandle);
        check( this->OnForeignPluginLoadedHandle.IsValid() == false )
    }
    else
    {
        this->OnForeignPluginLoadedHandle.Reset();
    }

    return;
}
