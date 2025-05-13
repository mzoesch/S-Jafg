// Copyright mzoesch. All rights reserved.

#include "Engine/ObjectContext.h"

#include "Engine/Engine.h"
#include "Engine/ObjectBase.h"
#include "Stats/Stats.h"

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

    if (this->Employees.IsEmpty() == false)
    {
        STAT_CYCLE_FUNCTION()

        LOG_TRACE
        (
            LogCarnifex,
            "Context [{}] found {} garbage employees. Begin to kill them.",
            this->HumanReadableName, this->Employees.GetSize()
        )

        for (JObjectBase* const& Employee : this->Employees)
        {
            checkSlow( Employee )

            if (Employee->GetVTable())
            {
                Employee->MarkAsGarbage();
                continue;
            }

            /*
             * The default content referrer.
             */
            check( Employee->bGarbage == false )
            Employee->bGarbage = true;
            Employee->OnDefaultGarbage();
            delete Employee;

            continue;
        }

        this->Employees.Empty();
    }

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
