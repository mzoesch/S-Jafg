// Copyright mzoesch. All rights reserved.

#include "Engine/ObjectContext.h"
#include "Engine/Engine.h"
#include "Engine/ObjectBase.h"
#include "Stats/Stats.h"
#include "Engine/Carnifex.h"

Jafg::LObjectContext::LObjectContext(EGlobalCarnifex)
{
    this->Initialize(Private::GCarnifexReferrer);
    return;
}

Jafg::LObjectContext::LObjectContext(EDeferredGlobalCarnifex)
{
    check( Tasks::IsOnMasterThread() )
    Tasks::Make
    (
        ENamedThreads::Master,
        ETaskTime::BeforeEngineInitButAfterAlloc,
        LTaskDelegate::CreateMember(this, &LObjectContext::InitializeWithGlobal)
    );

    return;
}

Jafg::LObjectContext& Jafg::LObjectContext::operator=(EGlobalCarnifex)
{
    this->Initialize(Private::GCarnifexReferrer);
    return *this;
}

void Jafg::LObjectContext::DeferredInitialize(EGlobalCarnifex)
{
    this->Initialize(Private::GCarnifexReferrer);
    return;
}

void Jafg::LObjectContext::DeferredInitialize(EDeferredGlobalCarnifex)
{
    Tasks::Make
    (
        ENamedThreads::Master,
        ETaskTime::BeforeEngineInitButAfterAlloc,
        LTaskDelegate::CreateMember(this, &LObjectContext::InitializeWithGlobal)
    );

    return;
}

void Jafg::LObjectContext::DeferredInitialize(LCarnifex* InCarnifex)
{
    this->Initialize(InCarnifex);
    return;
}

void Jafg::LObjectContext::TearDownContext()
{
    this->TearDownContextNoEngineUnregistration();
    this->UnregisterFromEngine();

    check( this->IsValid() == false )

    return;
}

void Jafg::LObjectContext::Initialize(LCarnifex* InCarnifex, const bool bRegister /* = true */)
{
    check( InCarnifex )
    check( this->IsValid() == false )
    this->Carnifex = InCarnifex;
    check( this->IsValid() )

    if (bRegister)
    {
        this->RegisterToEngine();
    }
    else
    {
        check( Tasks::IsOnMasterThread() )
    }

    return;
}

void Jafg::LObjectContext::SeparateAndKillEmployees(const LLoadedPluginHandle InPluginHandle)
{
    check( InPluginHandle )

    if (this->Employees.empty() == false)
    {
        STAT_CYCLE_FUNCTION()

        i32 Removed { 0 };
        bool bTouched { true };

        while (bTouched)
        {
            bTouched = false;

            for (auto& E : this->Employees)
            {
                if (const LObjectClass* Class = E->GetVTable(); Class)
                {
                    if (Class->GetPluginHandle() != InPluginHandle)
                    {
                        continue;
                    }

                    bTouched = true;
                    ++Removed;
                    E->MarkAsGarbage();
                    this->Employees.erase(TArray<JObjectBase*>::const_iterator{&E});
                    break;
                }

                continue;
            }

            continue;
        }

        if (Removed > 0)
        {
            LOG_VERBOSE(LogCarnifex, "Removed {} employees from context [{}] that were loaded by a foreign plugin.", Removed, this->HumanReadableName)
            check( this->Carnifex )
            this->Carnifex->KillAllGarbageChildren();
        }
    }

    return;
}

Jafg::LObjectContext::LObjectContext(const ENoEngineRegistration InValue)
{
    this->Initialize(Private::GCarnifexReferrer, false);
    return;
}

Jafg::LObjectContext::LObjectContext(const ENoEngineRegistration InValue, LCarnifex* InCarnifex)
{
    this->Initialize(InCarnifex, false);
    return;
}

void Jafg::LObjectContext::TearDownContextNoEngineUnregistration()
{
    check( this->Carnifex )

    if (this->Employees.empty() == false)
    {
        STAT_CYCLE_FUNCTION()

        LOG_TRACE
        (
            LogCarnifex,
            "Context [{}] found {} left over garbage employees. Begin to kill them.",
            this->HumanReadableName, this->Employees.size()
        )

        while (this->Employees.empty() == false)
        {
            JObjectBase* Employee { this->Employees.back() };

            checkSlow( Employee )

            if (Employee->GetVTable())
            {
                check( Employee->GetOuter() == this )
                Employee->MarkAsGarbage();

                check( algo::contains(this->Employees, Employee) == false )

                continue;
            }

            /* The default content referrer. */
            check( Employee->bGarbage == false )
            Employee->bGarbage = true;
            Employee->OnDefaultGarbageInternal();

            algo::erase_once_checked(&this->Employees, Employee);

            delete Employee;

            continue;
        }

        check( this->Employees.empty() )
    }

    this->Carnifex->KillAllGarbageChildren();
    this->Carnifex = nullptr;

    return;
}

void Jafg::LObjectContext::InitializeWithGlobal()
{
    this->Initialize(Private::GCarnifexReferrer);
    return;
}

void Jafg::LObjectContext::RegisterToEngine()
{
    check( Tasks::IsOnMasterThread() )

    if (GEngine == nullptr)
    {
        LOG_ERROR(LogObjectInternal, "Engine is invalid. Cannot register.")
        return;
    }

    GEngine->RegisterObjectContext(this);

    return;
}

void Jafg::LObjectContext::UnregisterFromEngine()
{
    check( Tasks::IsOnMasterThread() )

    if (GEngine == nullptr)
    {
        LOG_ERROR(LogObjectInternal, "Engine is invalid. Cannot unregister.")
        return;
    }

    GEngine->UnregisterObjectContext(this);

    return;
}
