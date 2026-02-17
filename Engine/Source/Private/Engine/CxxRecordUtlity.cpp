// Copyright mzoesch. All rights reserved.

#include "Engine/CxxRecordUtility.h"
#include "Engine/Carnifex.h"

void Jafg::Detail::LCxxRecordRegistry::AddNewPendingClassPackage(TUnique<LRegistryClassPackage> Package)
{
    check(Tasks::IsOnMasterThread())
    check(Package.get() != nullptr)
    check(Package->GetFullyQualifiedName().empty() == false)
    check(Package->IsClass())

    check(algo::contains(this->PendingPackages, Package->GetFullyQualifiedName(), [](auto const& E){ return E->GetFullyQualifiedName(); }) == false)
    check(algo::contains(this->RegisteredPackages, Package->GetFullyQualifiedName(), [](auto const& E){ return E->GetFullyQualifiedName(); }) == false)

    check(this->bAllowNewPendingPackages)

    this->PendingPackages.emplace_back(std::move(Package));

    return;
}

void Jafg::Detail::LCxxRecordRegistry::LoadPendingPackages(const LLoadedPluginHandle Handle)
{
    check(this->bAllowNewPendingPackages == false)

    if (this->PendingPackages.empty())
    {
        return;
    }

    check(Tasks::IsOnMasterThread())
    LOG_VERBOSE(LogPackager, "Loading [{}] pending packages...", this->PendingPackages.size())

    TArray<LCxxClass*> NewClasses;

    for (auto& Package : this->PendingPackages)
    {
        check(Package->GetFullyQualifiedName().empty() == false)
        check(Package->Origin.IsValid() == false)
        Package->Origin = Handle;
        auto& P{this->RegisteredPackages.emplace_back(std::move(Package))};
        if (P->IsClass())
        {
            check(algo::contains(NewClasses, &P->AsClass().StaticClass) == false)
            NewClasses.emplace_back(&P->AsClass().StaticClass);
        }
    }
    checkCode
    (
        for (auto const& Package : this->PendingPackages)
        {
            check(Package.get() == nullptr)
        }
    )
    algo::orphan(&this->PendingPackages);

    /* Resolves object hierarchy. */
    for (auto& Package : this->RegisteredPackages)
    {
        if (Package->IsClass() == false)
        {
            continue;
        }

        auto& Class{Package->AsClass().StaticClass};
        if (Class.Parent)
        {
            continue;
        }

        if (Class.ParentName == "NextIsBaseCxxClass")
        {
            continue;
        }

        Class.Parent = &this->_GetClassByNameWeakAsserted(Class.ParentName)->StaticClass;
        check(Class.Parent && std::addressof(Class) != Class.Parent)

        check(algo::contains(Class.Parent->Children, &Class) == false)
        Class.Parent->Children.emplace_back(&Class);

        continue;
    }

    for (auto Class : NewClasses)
    {
        Class->GetBeginClassLifeFn()({*Class});
    }

    if (this->SingletonOuter == nullptr)
    {
        this->SingletonOuter = new LClassOuter{"SingletonOuter"};
    }
    for (auto* Class : NewClasses)
    {
        if (Class->IsSingleton() && Class->IsAbstract() == false)
        {
            Class->Singleton = NewObject({*this->SingletonOuter, *Class});
        }
    }

    /* Validates */
    TOptional<LString> RootClassName;
    for (auto const& Package : this->RegisteredPackages)
    {
        check( Package.get() != nullptr )

        if (Package->GetFullyQualifiedName().empty())
        {
            panic( "Found loaded package with empty name." )
            continue;
        }

        if (Package->IsClass())
        {
            auto& ClassPackage{Package->AsClass()};

            if (ClassPackage.StaticClass.Parent == nullptr)
            {
                if (RootClassName.has_value())
                {
                    panicMsgf( "Found loaded package [{}] with no parent or found multiple root objects. Previous expected root class is [{}].",
                        ClassPackage.GetFullyQualifiedName(),
                        *RootClassName
                        )
                    continue;
                }

                RootClassName = ClassPackage.GetFullyQualifiedName();
                continue;
            }

            if (ClassPackage.StaticClass.IsSingleton() && ClassPackage.StaticClass.IsAbstract() == false)
            {
                if (ClassPackage.StaticClass.Singleton == nullptr)
                {
                    LOG_FATAL(LogObjectInternal, "Found loaded non-abstract singleton package [{}] with not singleton object.",
                        ClassPackage.GetFullyQualifiedName()
                        )
                }
            }
        }
        else
        {
            unreachable()
        }

        for (auto const& OtherPackage : this->RegisteredPackages)
        {
            if (&Package == &OtherPackage)
            {
                continue;
            }

            if (Package->GetFullyQualifiedName() == OtherPackage->GetFullyQualifiedName())
            {
                panicMsgf( "Found duplicate package names [{}].", Package->GetFullyQualifiedName() )
                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void Jafg::Detail::LCxxRecordRegistry::KillPendingPackages()
{
    if (this->PendingPackages.empty())
    {
        return;
    }

    check(Tasks::IsOnMasterThread())
    LOG_VERBOSE(LogPackager, "Killing [{}] pending packages...", this->PendingPackages.size())

    algo::orphan(&this->PendingPackages);

    return;
}

void Jafg::Detail::LCxxRecordRegistry::SetAllowNewPendingPackages(const bool bAllow) noexcept
{
    check(Tasks::IsOnMasterThread())

    check(this->bAllowNewPendingPackages != bAllow)
    this->bAllowNewPendingPackages = bAllow;

    return;
}

LSize Jafg::Detail::LCxxRecordRegistry::RemovePackagesOf(const LLoadedPluginHandle Handle)
{
    check(Tasks::IsOnMasterThread())
    check(this->bAllowNewPendingPackages == false)
    check(Handle.IsValid())
    check(this->PendingPackages.empty())

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    if (this->SingletonOuter)
    {
        for (auto Idx{0uz}; Idx < this->SingletonOuter->GetEmployees().size();)
        {
            auto const& Singleton{this->SingletonOuter->GetEmployees()[Idx]};
            if (Singleton->GetVirtualTable().GetPluginHandle() == Handle)
            {
                Singleton->MarkAsGarbage_v2(ECxxRecordTearDownReason::PluginUnload);
                continue;
            }
            ++Idx;
            continue;
        }
    }

#if JAFG_DO_CHECKS
    LSize It1{};
#endif /* JAFG_DO_CHECKS */
    for (auto It{this->RegisteredPackages.begin()}; this->RegisteredPackages.end() != It; ++It)
    {
        check((*It)->Origin.IsValid())
        if ((*It)->Origin != Handle)
        {
            continue;
        }
        if ((*It)->IsClass())
        {
            (*It)->AsClass().StaticClass.GetEndClassLifeFn()({(*It)->AsClass().StaticClass});
        }
#if JAFG_DO_CHECKS
        ++It1;
#endif /* JAFG_DO_CHECKS */
        continue;
    }

    LSize Out{};
    for (auto It{this->RegisteredPackages.begin()}; this->RegisteredPackages.end() != It;)
    {
        check((*It)->Origin.IsValid())
        if ((*It)->Origin != Handle)
        {
            It = algo::next(It);
            continue;
        }
        if ((*It)->IsClass())
        {
            auto& ClassPackage{(*It)->AsClass()};
            algo::erase_once_checked(&ClassPackage.StaticClass.GetParent()->GetChildren(), &ClassPackage.StaticClass);
            for (LCxxClass* Child : ClassPackage.StaticClass.GetChildren())
            {
                check(Child)
                check(Child->GetParent() == &ClassPackage.StaticClass)
                Child->Parent = nullptr;
                continue;
            }
        }
        ++Out;
        this->RegisteredPackages.erase(It);
        continue;
    }
    check(Out == It1)

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    //# Check for invalid parents...
    for (auto& Package : this->RegisteredPackages)
    {
        if (Package->IsClass() == false)
        {
            continue;
        }

        auto& Class{Package->AsClass().StaticClass};
        if (Class.Parent == nullptr)
        {
            if (Class.ParentName == "NextIsBaseCxxClass")
            {
                continue;
            }

            panicMsgf( "Found class [{}] with invalid parent [{}] after removing packages of plugin [{}]. Possible dangling dependency on foreign plugin.",
                Class.GetFullyQualifiedName(),
                Class.ParentName,
                static_cast<u32>(Handle)
                )
            continue;
        }

        continue;
    }

    this->RegisteredPackages.shrink_to_fit();

    return Out;
}

void Jafg::Detail::LCxxRecordRegistry::TearDown()
{
    check(Tasks::IsOnMasterThread())

    delete this->SingletonOuter;
    this->SingletonOuter = nullptr;

    // TODO: Add more security, by adding asserts that check if a class that called either Begin/End also calls their equivalent Begin/End.
    for (auto It{this->RegisteredPackages.begin()}; this->RegisteredPackages.end() != It; ++It)
    {
        if ((*It)->IsClass())
        {
            (*It)->AsClass().StaticClass.GetEndClassLifeFn()({(*It)->AsClass().StaticClass});
        }
    }

    algo::orphan(&this->PendingPackages);
    algo::orphan(&this->RegisteredPackages);

    return;
}

Jafg::LCarnifex& Jafg::Detail::GetGlobalCarnifex() noexcept
{
    static LCarnifex Instance;
    return Instance;
}

Jafg::Detail::LCxxRecordRegistry& Jafg::Detail::GetGlobalCxxRecordRegistry() noexcept
{
    static LCxxRecordRegistry Instance;
    return Instance;
}
