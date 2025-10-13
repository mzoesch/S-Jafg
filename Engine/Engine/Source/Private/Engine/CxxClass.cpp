// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClass.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"
#include "Engine/Carnifex.h"

#if LAL_PLATFORM_SUPPORTS_STD_STACKTRACE
    #include <stacktrace>
#endif /* LAL_PLATFORM_SUPPORTS_STD_STACKTRACE */

namespace
{

#if LAL_DO_CHECKS
    #if LAL_PLATFORM_SUPPORTS_STD_STACKTRACE
        std::unordered_map<void const*, std::stacktrace> DoubleDestroyCheckTable;
    #else /**/
        std::unordered_map<void const*, LString> DoubleDestroyCheckTable;
    #endif /* !LAL_PLATFORM_SUPPORTS_STD_STACKTRACE */
#endif /* LAL_DO_CHECKS */

} /* ~Namespace <Anonymous> */

Jafg::JCxxClass::JCxxClass(LCxxObjectInitializer const& CxxObjectInitializer) noexceptcheck
    : Outer{&CxxObjectInitializer.Outer}
{
    check( this->Outer )

    check( algo::contains(this->Outer->GetEmployees(), this, &TUnique<JCxxClass>::get) == false )
    this->Outer->Employees.push_back(TUnique<JCxxClass>(this));

    check( this->IsVirtualTableValid() == false )

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

Jafg::LEngine* Jafg::JCxxClass::GetEngine() const noexcept
{
    return GEngine;
}

Jafg::LLocalEgo* Jafg::JCxxClass::GetLocalEgo() const
{
    if (auto* Engine{ this->GetEngine() })
    {
        return Engine->GetLocalEgo();
    }

    return nullptr;
}

Jafg::LCommandLineInterface* Jafg::JCxxClass::GetCommandLineInterface() const
{
    if (auto* Engine{ this->GetEngine() })
    {
        return Engine->GetCommandLineInterface();
    }

    return nullptr;
}

#if LAL_DO_CHECKS
void Jafg::JCxxClass::CheckDoubleDestroy(void const* Ptr)
{
    if (const auto It{ DoubleDestroyCheckTable.find(Ptr) }; It != DoubleDestroyCheckTable.end())
    {
        panicMsgf("Double destroy detected. At memory address [{}]. Previous destroy stacktrace:\n{}"
            , Ptr
            , It->second
            )
    }

#if LAL_PLATFORM_SUPPORTS_STD_STACKTRACE
    DoubleDestroyCheckTable.emplace(Ptr, std::stacktrace::current());
#else /* LAL_PLATFORM_SUPPORTS_STD_STACKTRACE */
    DoubleDestroyCheckTable.emplace(Ptr, "<unknown-stacktrace>");
#endif /* !LAL_PLATFORM_SUPPORTS_STD_STACKTRACE */

    return;
}
#endif /* LAL_DO_CHECKS */

void Jafg::JCxxClass::MarkAsGarbage(EMarkAsGarbageBehavior Behavior, ECxxRecordTearDownReason::Type Reason)
{
    check( Tasks::IsOnMasterThread() )

    check( this->IsGarbage() == false )
    this->bGarbage = true;

    check( this->Outer )

    TUnique<JCxxClass> Self;
    {
        auto& Employees{ this->Outer->Employees };
        auto const It{ algo::find(Employees, this, &TUnique<JCxxClass>::get) };
        check( It != Employees.end() )
        Self = std::move(*It);
        Employees.erase(It);
    }

    if (this->IsDefault())
    {
        this->OnDefaultGarbageInternal(Reason);
    }
    else
    {
        this->OnGarbage(Reason);
    }

    Self->Outer->RemoveDanglingReferencesToObject(Self.get());

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

void Jafg::JCxxClass::OnDefaultGarbageInternal(ECxxRecordTearDownReason::Type Reason)
{
    check( this->IsDefault() )
    check( this->IsGarbage() )

    this->OnGarbageDefault(Reason);

    check( this->IsVirtualTableValid() )
    if (this->GetVirtualTable()->IsConfig())
    {
        PushConfigFromCxxObject(*this->GetVirtualTable());
    }

    return;
}
