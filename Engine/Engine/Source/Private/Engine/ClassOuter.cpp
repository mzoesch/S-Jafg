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
    else
    {
        check( this->bWasRegisteredToEngine == false )
    }

    return;
}

void Jafg::LClassOuter::TearDown() noexcept
{
    if (this->HumanReadableName.starts_with("<torn-down:"))
    {
        check( this->Employees.empty() )
        check( this->bWasRegisteredToEngine == false )

        return;
    }

    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogClassOuter, "Tearing down class outer [{}].", this->HumanReadableName)

    checkCode
    (
        for (auto& E : this->Employees)
        {
            check( E.get() != nullptr )
            check( E->IsGarbage() == false )
            check( E->GetOuter() == this )

            continue;
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
        auto& E{ this->Employees.back() };

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

    check( this->Employees.empty() )
    check( this->bWasRegisteredToEngine == false )

    algo::orphan(&this->Employees);

    Private::GetGlobalCarnifex().KillAllGarbageChildren();

    return;
}

#if LAL_DO_CHECKS
void Jafg::LClassOuter::CheckValidityForCDRStates() const noexcept
{
    LSize CDRCount{ 0 };
    LSize NonCDRCount{ 0 };

    algo::for_each(this->Employees, [&CDRCount, &NonCDRCount](auto const& E)
    {
        if (E->IsCDR())
        {
            ++CDRCount;
        }
        else
        {
            ++NonCDRCount;
        }

        return;
    });

    if (CDRCount != 0 && NonCDRCount != 0)
    {
        LOG_WARNING(LogClassOuter, "Class outer [{}] employs both normal [{}] and CDR [{}] classes.",
              this->HumanReadableName
            , NonCDRCount
            , CDRCount
            )
    }

    return;
}
#endif /* LAL_DO_CHECKS */

LSize Jafg::LClassOuter::KillEmployeesFromForeignPlugin(
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

    checkCode( this->CheckValidityForCDRStates() )

    LSize KillCount { 0 };

    bool bTouched { false };
    do
    {
        for (auto const& E : this->Employees)
        {
            auto const& Class{ E->GetVirtualTable() };
            if (Class.GetPluginHandle() != PluginHandle)
            {
                continue;
            }

#if LAL_DO_CHECKS
            const LSize Size { this->Employees.size() };
#endif /* LAL_DO_CHECKS */

            bTouched = true;
            ++KillCount;
            E->MarkAsGarbage_v2(Reason);

#if LAL_DO_CHECKS
            check( this->Employees.size() < Size )
#endif /* LAL_DO_CHECKS */

            break;
        }
    } while (bTouched);

    if (KillCount > 0)
    {
        LOG_VERBOSE(LogClassOuter, "Removed {} employees from outer [{}] that were loaded by a foreign plugin.", KillCount, this->HumanReadableName)
        Private::GetGlobalCarnifex().KillAllGarbageChildren();
    }

    return KillCount;
}

void Jafg::LClassOuter::RegisterToEngine()
{
    check( Tasks::IsOnMasterThread() )

    if (GEngine)
    {
        GEngine->RegisterClassOuter(this);
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [this]()
        {
            check( GEngine )
            GEngine->RegisterClassOuter(this);
            return;
        });
    }

    return;
}

void Jafg::LClassOuter::UnregisterFromEngine()
{
    check( Tasks::IsOnMasterThread() )
    check( GEngine )

    GEngine->UnregisterClassOuter(this);

    return;
}
