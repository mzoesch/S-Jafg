// Copyright mzoesch. All rights reserved.

#include "Subsystems/SubsystemCollection.h"
#include "Engine/Carnifex.h"
#include "Subsystems/Subsystem.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"

void Jafg::LSubsystemCollection::InitializeSubsystems(TSubclassOf<JSubsystem> Class, bool bRegisterDeferredDelegate /* = true */)
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    check(this->Outer)
    check(this->SubsystemInstances.empty())
    check(Class.HasClass())

    LOG_VERBOSE(LogSubsystemCollection, "Locating all subsystems of class {}.", Class->GetFullyQualifiedName())

    auto& Registry{Detail::GetGlobalCxxRecordRegistry()};
    for (TArray ValidClasses{Registry.GetClassesByBase(*Class.GetClass())}; auto const* Candidate : ValidClasses)
    {
        check( Candidate )
        if (Candidate->IsNotAbstract())
        {
            this->SubsystemInstances.emplace_back(NewObject(CastTo<JSubsystem>{}, {*this->Outer, *Candidate}));
        }
        continue;
    }

    for (auto It{ this->SubsystemInstances.begin() }; It != this->SubsystemInstances.end();)
    {
        check(*It)

        if ((*It)->IsInitialized())
        {
            It = algo::next(It);
            continue;
        }

        if ((*It)->ShouldCreateSubsystem(this->Outer))
        {
            LOG_TRACE(LogSubsystemCollection, "Initializing subsystem {}.", (*It)->GetNameAsString())
            (*It)->Initialize(*this);
            It = algo::next(It);
            continue;
        }

        (*It)->MarkAsGarbage_v2();
        this->SubsystemInstances.erase(It);
        continue;
    }

    /* This will make this collection, so do not set it before, while subsystems are still initializing. */
    this->Class = Class;

    if (bRegisterDeferredDelegate && GEngine)
    {
        this->OnForeignPluginLoadedHandle = GEngine->OnForeignPluginLoaded.Emplace(this, &LSubsystemCollection::OnForeignPluginLoaded);
    }

    this->SubsystemInstances.shrink_to_fit();

    checkCode
    (
        for (auto* E : this->SubsystemInstances)
        {
            check(E && E->IsInitialized() && &E->GetOuter() == this->Outer && E->GetVirtualTable().DerivesFrom(*Class.GetClass()))
        }
    )

    return;
}

void Jafg::LSubsystemCollection::InitializeSubsystemsForDeferred()
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    check(this->Outer)
    check(this->Class.HasClass())

    LOG_VERBOSE(LogSubsystemCollection, "[{}]: Locating all subsystems of class {} that are not loaded.", this->FriendlyName, this->Class->GetFullyQualifiedName())

    auto& Registry{Detail::GetGlobalCxxRecordRegistry()};
    for (TArray ValidClasses{Registry.GetClassesByBase(*this->Class.GetClass())}; auto const* Candidate : ValidClasses)
    {
        check(Candidate)

        if (Candidate->IsAbstract())
        {
            continue;
        }

        if (algo::contains(this->SubsystemInstances, Candidate, &JSubsystem::GetVirtualTableAsPointer) == false)
        {
            LOG_VERBOSE(LogSubsystemCollection, "Found potential subsystem [{}].", Candidate->GetFullyQualifiedName() )
            this->SubsystemInstances.emplace_back(NewObject(CastTo<JSubsystem>{}, {*this->Outer, *Candidate}));
        }

        continue;
    }

    for (auto It{this->SubsystemInstances.begin()}; It != this->SubsystemInstances.end();)
    {
        check(*It)

        if ((*It)->IsInitialized())
        {
            It = algo::next(It);
            continue;
        }

        if ((*It)->ShouldCreateSubsystem(this->Outer))
        {
            LOG_TRACE(LogSubsystemCollection, "Initializing subsystem {}.", (*It)->GetNameAsString())
            (*It)->Initialize(*this);
            It = algo::next(It);
            continue;
        }

        (*It)->MarkAsGarbage_v2();
        this->SubsystemInstances.erase(It);

        continue;
    }

    return;
}

void Jafg::LSubsystemCollection::OnForeignPluginLoaded(LLoadedPlugin const& Plugin)
{
    LOG_VERBOSE(LogSubsystemCollection, "Foreign plugin loaded, initializing dependent subsystems for [{}].", Plugin.GetIdentifier())
    this->InitializeSubsystemsForDeferred();

    return;
}

void Jafg::LSubsystemCollection::InitializeDependency(TSubclassOf<JSubsystem> Class)
{
    JSubsystem* Subsystem{ this->GetSubsystemAsserted(Class) };

    if (Subsystem->IsInitialized())
    {
        return;
    }

    if (Subsystem->ShouldCreateSubsystem(this->Outer) == false)
    {
        LOG_FATAL(
            LogSubsystemCollection,
            "Wanted to initialize dependent subsystem {} but it does not want to be created.",
            Subsystem->GetNameAsString()
            )
        return;
    }

    LOG_TRACE(LogSubsystemCollection, "Initializing subsystem {}.", Subsystem->GetNameAsString())
    Subsystem->Initialize(*this);

    return;
}

Jafg::JSubsystem const* Jafg::LSubsystemCollection::GetSubsystem(TSubclassOf<JSubsystem> Class) const
{
    for (JSubsystem const* Subsystem : this->SubsystemInstances)
    {
        check(Subsystem)
        if (&Subsystem->GetVirtualTable() == Class.GetClass())
        {
            return Subsystem;
        }
        continue;
    }

    return nullptr;
}

Jafg::JSubsystem* Jafg::LSubsystemCollection::GetSubsystem(TSubclassOf<JSubsystem> Class)
{
    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        check(Subsystem)
        if (&Subsystem->GetVirtualTable() == Class.GetClass())
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

    check(Tasks::IsOnMasterThread())
    check(this->Outer)

    i32 SubsystemCount{};
    for (auto*& Subsystem : this->SubsystemInstances)
    {
        check(Subsystem)

        if (Subsystem->IsPriorityTearDown())
        {
            Subsystem->MarkAsGarbage_v2();
            Subsystem = nullptr;
            ++SubsystemCount;
        }

        continue;
    }

    if (SubsystemCount > 0)
    {
        LOG_VERBOSE(LogSubsystemCollection, "Tore down {} priority subsystems for outer [{}].", SubsystemCount, this->Outer->GetHumanReadableName())
    }

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    return;
}

void Jafg::LSubsystemCollection::TearDownNonPrioritySubsystems()
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    check(this->Outer)

    LOG_VERBOSE(LogSubsystemCollection, "Tearing down {} subsystems for outer [{}].", this->SubsystemInstances.size(), this->Outer->GetHumanReadableName())

    for (JSubsystem* Subsystem : this->SubsystemInstances)
    {
        if (Subsystem)
        {
            Subsystem->MarkAsGarbage_v2();
        }
    }

    algo::orphan(&this->SubsystemInstances);
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    this->Outer = nullptr;
    this->Class.SetClass(nullptr);

    if (GEngine)
    {
        GEngine->OnForeignPluginLoaded.Remove(&this->OnForeignPluginLoadedHandle);
        check(this->OnForeignPluginLoadedHandle.IsValid() == false)
    }
    else
    {
        this->OnForeignPluginLoadedHandle.Reset();
    }

    return;
}
