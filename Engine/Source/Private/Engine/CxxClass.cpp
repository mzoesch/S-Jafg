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

void Jafg::JCxxClass::MarkAsGarbage_v2(ECxxRecordTearDownReason::Type Reason)
{
    check(Tasks::IsOnMasterThread())

    if (this->bGarbage)
    {
        LOG_WARNING(LogObjectInternal, "Class [{}] was already marked as garbage.", this->GetNameAsString())
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
            check( this->_IsGarbage() == false )
        }
    )

    if (this->_IsGarbage())
    {
        LOG_ERROR(LogObjectInternal, "Class [{}] was already marked as garbage. Failed to devour.", this->GetNameAsString())
    }
    else
    {
        this->MarkAsGarbage(EMarkAsGarbageBehavior::DevourNow, Reason);
    }

    return;
}

Jafg::LEngine& Jafg::JCxxClass::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::JCxxClass::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetLocalEgo();
}

Jafg::LCommandLineInterface& Jafg::JCxxClass::GetCommandLineInterface() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetCommandLineInterface();
}

void Jafg::JCxxClass::MarkAsGarbage(EMarkAsGarbageBehavior Behavior, ECxxRecordTearDownReason::Type Reason)
{
    check(Tasks::IsOnMasterThread())

    check(this->_IsGarbage() == false)
    this->bGarbage = true;

    this->OnGarbage(Reason);

    TUnique<JCxxClass> Self;
    if (Reason == ECxxRecordTearDownReason::OuterTearDown)
    {
        Self = this->Outer.PoachToNull(this);
    }
    else
    {
        Self = this->Outer.Poach(this);
    }
    check( Self.get() == this )

    if (Behavior == EMarkAsGarbageBehavior::Default)
    {
        Detail::GetGlobalCarnifex().AddGarbageChild(std::move(Self));
    }
    else if (Behavior == EMarkAsGarbageBehavior::DevourNow)
    {
        Detail::GetGlobalCarnifex().DevourGarbageChildNow(std::move(Self));
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

#if JAFG_DO_CHECKS
void Jafg::JCxxClass::_check_BeginClassLife(LBeginClassLifeInfo const& Info)
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
}

void Jafg::JCxxClass::_check_EndClassLife(LEndClassLifeInfo const& Info)
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
}
#endif /* JAFG_DO_CHECKS */
