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

Jafg::LEngine const& Jafg::JCxxClass::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return *GEngine;
}

Jafg::LEngine& Jafg::JCxxClass::GetMutableEngine() noexcept
{
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")
    return *GMutableEngine;
}

Jafg::LLocalEgo const& Jafg::JCxxClass::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetLocalEgo();
}
Jafg::LLocalEgo& Jafg::JCxxClass::GetMutableLocalEgo() noexcept
{
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")
    return GMutableEngine->GetLocalEgo();
}

Jafg::LCommandLineInterface const& Jafg::JCxxClass::GetCommandLineInterface() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetCommandLineInterface();
}

Jafg::LCommandLineInterface& Jafg::JCxxClass::GetMutableCommandLineInterface() noexcept
{
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")
    return GMutableEngine->GetCommandLineInterface();
}

void Jafg::JCxxClass::PullConfig(LPath const& InPath /* = {} */) noexcept
{
    // TODO: Add arg to ignore pulling
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")

    LPath Path{InPath};
    if (Path == LPath{})
    {
        Path = Finder::GetUserPreferencesFile();
    }

    auto& Class{this->GetVirtualTable()};
    LOG_VERBOSE(LogObjectInternal, "Pulling config for [{}]", this->GetNameAsString())
    check(Class.IsConfig())

    GMutableEngine->Config.PullConfigFile(Path);
    if (auto* Section{GMutableEngine->Config.GetConfigSection(Path, this->GetNameAsString())})
    {
        for (auto& [Key, Value] : *Section)
        {
            if (auto It{algo::find(Class.GetFields(), Key, &LCxxClassField::Identifier)}; It != Class.GetFields().end())
            {
                It->Set(this, Value);
            }
        }
    }

    return;
}

void Jafg::JCxxClass::PushConfig(LPath const& InPath /* = {} */) const noexcept
{
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")

    LPath Path{InPath};
    if (Path == LPath{})
    {
        Path = Finder::GetUserPreferencesFile();
    }

    auto& Class{this->GetVirtualTable()};
    LOG_VERBOSE(LogObjectInternal, "Pushing config for [{}]", this->GetNameAsString())
    check(Class.IsConfig())

    std::unordered_map<LString, LString> Entries;
    Entries.reserve(Class.GetFields().size());
    for (LCxxClassField const& Field : Class.GetFields())
    {
        Entries[LString{Field.Identifier}] = Field.Get(*this);
    }
    if (Entries.empty() == false)
    {
        GMutableEngine->Config.AddConfigSection(Path, this->GetNameAsString(), Entries);
    }

    return;
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
    check(Self.get() == this)

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
