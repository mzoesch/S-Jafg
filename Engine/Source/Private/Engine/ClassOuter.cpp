// Copyright mzoesch. All rights reserved.

#include "Engine/ClassOuter.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"
#include "Engine/Carnifex.h"
#include "Stats/Stats.h"

Jafg::LClassOuter::LClassOuter(LString HumanReadableName, bool bRegisterToEngine /* = true */) noexcept
{
    check( Tasks::IsOnMasterThread() )

    this->HumanReadableName = std::move(HumanReadableName);

    if (bRegisterToEngine)
    {
        this->bWasRegisteredToEngine = true;
        this->RegisterToEngine();
    }

    return;
}

void Jafg::LClassOuter::TearDown() noexcept
{
    if (this->HumanReadableName.starts_with("<torn-down:"))
    {
        check(this->Employees.empty())
        check(this->bWasRegisteredToEngine == false)
        return;
    }

    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogClassOuter, "Tearing down class outer [{}].", this->HumanReadableName)

    checkCode
    (
        for (auto& E : this->Employees)
        {
            check(E.get() != nullptr)
            check(E->_IsGarbage() == false)
            check(&E->GetOuter() == this)
        }
    )
    this->OnTearDown();

    LOG_TRACE
    (
        LogClassOuter,
        "Class outer [{}] found {} left over garbage employees. Begin to kill them.",
        this->HumanReadableName, this->Employees.size()
    )

    while (this->Employees.empty() == false)
    {
        auto& E{this->Employees.back()};
        /* Null only allowed in tear down. */
        if (E.get() == nullptr)
        {
            this->Employees.pop_back();
            continue;
        }
        E->MarkAsGarbage_v2(ECxxRecordTearDownReason::OuterTearDown);
        continue;
    }

    if (this->bWasRegisteredToEngine)
    {
        this->UnregisterFromEngine();
        this->bWasRegisteredToEngine = false;
    }

    this->HumanReadableName = "<torn-down:" + this->HumanReadableName + '>';

    check(this->Employees.empty())
    check(this->bWasRegisteredToEngine == false)

    algo::orphan(&this->Employees);
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    return;
}

std::size_t Jafg::LClassOuter::KillEmployeesFromForeignPlugin(
    LLoadedPluginHandle PluginHandle,
    ECxxRecordTearDownReason::Type Reason /* = ECxxRecordTearDownReason::PluginUnload */
    )
{
    check( Tasks::IsOnMasterThread() )
    check( PluginHandle.IsValid() )

    if (this->Employees.empty())
    {
        return 0;
    }

    STAT_CYCLE_FUNCTION()

    std::size_t KillCount{};
    bool bTouched{};
    do
    {
        for (auto const& E : this->Employees)
        {
            auto const& Class{E->GetVirtualTable()};
            if (Class.GetPluginHandle() != PluginHandle)
            {
                continue;
            }

            bTouched = true;
            ++KillCount;
            E->MarkAsGarbage_v2(Reason);

            break;
        }
    } while (bTouched);

    if (KillCount > 0)
    {
        LOG_VERBOSE(LogClassOuter, "Removed {} employees from outer [{}] that were loaded by a foreign plugin.", KillCount, this->HumanReadableName)
        Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    }

    return KillCount;
}

void Jafg::LClassOuter::RegisterToEngine()
{
    check( Tasks::IsOnMasterThread() )

    if (GEngine)
    {
        GMutableEngine->RegisterClassOuter(this);
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [this]()
        {
            check(GMutableEngine)
            GMutableEngine->RegisterClassOuter(this);
            return;
        });
    }

    return;
}

void Jafg::LClassOuter::UnregisterFromEngine()
{
    check(Tasks::IsOnMasterThread())
    check(GMutableEngine)

    GMutableEngine->UnregisterClassOuter(this);

    return;
}
