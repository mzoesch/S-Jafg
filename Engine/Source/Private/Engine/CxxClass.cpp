// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClass.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"
#include "Engine/Carnifex.h"

#if JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE
    #include <stacktrace>
#endif /* JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE */

namespace
{

#if JAFG_DO_CHECKS
    #if JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE
        std::unordered_map<void const*, std::stacktrace> DoubleDestroyCheckTable;
    #else /**/
        std::unordered_map<void const*, LString> DoubleDestroyCheckTable;
    #endif /* !JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE */
#endif /* JAFG_DO_CHECKS */

} /* ~Namespace <Anonymous> */

Jafg::JCxxClass::JCxxClass(LCxxObjectInitializer const& CxxObjectInitializer) noexceptcheck
    : JafgVirtualTable{CxxObjectInitializer.Class}, Outer{&CxxObjectInitializer.Outer}
{
    check( this->Outer )

    check( algo::contains(this->Outer->GetEmployees(), this, &TUnique<JCxxClass>::get) == false )
    this->Outer->Employees.push_back(TUnique<JCxxClass>(this));

    return;
}

void Jafg::JCxxClass::MarkAsGarbage_v2(ECxxRecordTearDownReason::Type Reason)
{
    check( Tasks::IsOnMasterThread() )

    if (this->bGarbage)
    {
        return;
    }

    this->MarkAsGarbage(EMarkAsGarbageBehavior::Default, Reason);

    return;
}

void Jafg::JCxxClass::KillYourSelfNow_v2(ECxxRecordTearDownReason::Type Reason /* = ECxxRecordTearDownReason::Default */, bool bMayBeGarbage /* = false */)
{
    checkCode
    (
        if (bMayBeGarbage == false)
        {
            check( this->IsGarbage() == false )
        }
    )

    if (this->IsGarbage() == false)
    {
        this->MarkAsGarbage(EMarkAsGarbageBehavior::DevourNow, Reason);
    }

    return;
}

Jafg::LEngine& Jafg::JCxxClass::GetEngine() const noexcept
{
    check( GEngine && "Absence of GEngine if undefined behavior." )
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::JCxxClass::GetLocalEgo() const noexcept
{
    return this->GetEngine().GetLocalEgo();
}

Jafg::LCommandLineInterface& Jafg::JCxxClass::GetCommandLineInterface() const noexcept
{
    return this->GetEngine().GetCommandLineInterface();
}

#if JAFG_DO_CHECKS
void Jafg::JCxxClass::CheckDoubleDestroy(void const* Ptr)
{
    return;

    if (const auto It{ DoubleDestroyCheckTable.find(Ptr) }; It != DoubleDestroyCheckTable.end())
    {
        panicMsgf("Double destroy detected. At memory address [{}]. Previous destroy stacktrace:\n{}"
            , Ptr
            , It->second
            )
    }

#if JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE
    DoubleDestroyCheckTable.emplace(Ptr, std::stacktrace::current());
#else /* JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE */
    DoubleDestroyCheckTable.emplace(Ptr, "<unknown-stacktrace>");
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE */

    return;
}
#endif /* JAFG_DO_CHECKS */

void Jafg::JCxxClass::MarkAsGarbage(EMarkAsGarbageBehavior Behavior, ECxxRecordTearDownReason::Type Reason)
{
    check( Tasks::IsOnMasterThread() )

    check( this->IsGarbage() == false )
    this->bGarbage = true;

    check( this->Outer )

    TUnique<JCxxClass> Self;
    if (Reason == ECxxRecordTearDownReason::OuterTearDown)
    {
        Self = this->Outer->PoachToNull(this);
    }
    else
    {
        Self = this->Outer->Poach(this);
    }
    check( Self.get() == this )

    LClassOuter* PoachedOuter{ this->Outer };

    Private::LCxxRecordMiscellaneousAccessor::ChangeOuter(Self.get(), nullptr);

    if (this->IsDefault())
    {
        this->OnDefaultGarbageInternal(Reason, *PoachedOuter);
    }
    else
    {
        this->OnGarbage(Reason, *PoachedOuter);
    }

    if (Behavior == EMarkAsGarbageBehavior::Default)
    {
        Private::GetGlobalCarnifex().AddGarbageChild(std::move(Self));
    }
    else if (Behavior == EMarkAsGarbageBehavior::DevourNow)
    {
        Private::GetGlobalCarnifex().DevourGarbageChildNow(std::move(Self));
    }
    else if (Behavior == EMarkAsGarbageBehavior::Ignore)
    {
        LOG_WARNING(LogObjectInternal, "Object [{}] is not being deleted by the carnifex.", this->GetNameAsString());
        Self.release();
    }
    else
    {
        unreachable()
    }

    return;
}

void Jafg::JCxxClass::OnDefaultGarbageInternal(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter)
{
    check( this->IsDefault() )
    check( this->IsGarbage() )

    this->OnGarbageDefault(Reason, PreviousOuter);

    if (this->GetVirtualTable().IsConfig())
    {
        PushConfigFromCxxObject(this->GetVirtualTable());
    }

    check( this->GetVirtualTable().IsCDRValid() )
    check( this->GetVirtualTable().GetCDR() == this )

    auto Self{ Private::LCxxClassMiscellaneousAccessor::ExchangeCDR(&this->GetMutableVirtualTable(), nullptr) };
    check( Self.get() == this )

    check( this->GetVirtualTable().IsCDRValid() == false )

    Self.release();
    check( Self.get() == nullptr )

    return;
}
