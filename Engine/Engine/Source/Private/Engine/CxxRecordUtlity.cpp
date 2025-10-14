// Copyright mzoesch. All rights reserved.

#include "Engine/CxxRecordUtility.h"
#include "Engine/Carnifex.h"

namespace
{

//# Outer for CDRs.
Jafg::LClassOuter GOuter{"CDROuter", false};

} /* ~Namespace <Anonymous> */

Jafg::Private::LCxxRecordRegistry::LCxxRecordRegistry() noexcept
    : Outer(GOuter)
{
    return;
}

void Jafg::Private::LCxxRecordRegistry::AddNewPendingPackage(TUnique<LRegistryPackage> Package)
{
    check( Tasks::IsOnMasterThread() )
    check( Package.get() != nullptr )
    check( Package->GetFullyQualifiedName().empty() == false )

    check( algo::contains(this->PendingPackages, Package->GetFullyQualifiedName(), [](auto const& E){ return E->GetFullyQualifiedName(); }) == false )
    check( algo::contains(this->RegisteredPackages, Package->GetFullyQualifiedName(), [](auto const& E){ return E->GetFullyQualifiedName(); }) == false )

    check( this->bAllowNewPendingPackages )

    if (Package->IsClass())
    {
        auto& ClassPackage{ Package->AsClass() };
        ClassPackage.StaticClass.Flags = ClassPackage.Flags;
        check( (ClassPackage.StaticClass.Flags & ECxxClassFlags::Error) == ECxxClassFlags::None )
    }

    this->PendingPackages.emplace_back(std::move(Package));

    return;
}

void Jafg::Private::LCxxRecordRegistry::LoadPendingPackages(const LLoadedPluginHandle Handle)
{
    check( this->bAllowNewPendingPackages == false )
    check( this->bAllowCDRRegistration == false )

    if (this->PendingPackages.empty())
    {
        return;
    }

    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogPackager, "Loading [{}] pending packages...", this->PendingPackages.size())

    TArray<JCxxClass*> NewCDRs;

    for (auto& Package : this->PendingPackages)
    {
        check( Package->GetFullyQualifiedName().empty() == false )

        check( Package->Origin.IsValid() == false )
        Package->Origin = Handle;

        if (Package->IsClass())
        {
            auto& ClassPackage{ Package->AsClass() };

            this->bAllowCDRRegistration = true;
            ClassPackage.StaticClass.CDR = TUnique<JCxxClass>{ClassPackage.StaticClass.GetCDRFunction()};
            this->bAllowCDRRegistration = false;

            check( ClassPackage.StaticClass.CDR.get() )

            NewCDRs.emplace_back(ClassPackage.StaticClass.CDR.get());
        }
        else
        {
            unreachable()
        }

        this->RegisteredPackages.emplace_back(std::move(Package));
    }
    checkCode
    (
        for (auto const& Package : this->PendingPackages)
        {
            check( Package.get() == nullptr )
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

        auto& Class{ Package->AsClass().StaticClass };
        if (Class.Parent)
        {
            continue;
        }

        if (Class.ParentName == "NextIsBaseCxxClass")
        {
            continue;
        }

        Class.Parent = &this->GetClassByNameWeakAsserted(Class.ParentName)->StaticClass;
        check( std::addressof(Class) != Class.Parent )
        checkSlow( Class.Parent )

        check( algo::contains(Class.Parent->Children, &Class) == false )
        Class.Parent->Children.emplace_back(&Class);

        continue;
    }

    for (JCxxClass* Class : NewCDRs)
    {
        check( Class )
        Class->BeginLifeCDR();

        continue;
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
            auto& ClassPackage{ Package->AsClass() };

            if (ClassPackage.StaticClass.CDR.get() == nullptr)
            {
                panicMsgf( "Found loaded package [{}] with no CDR.", ClassPackage.GetFullyQualifiedName() )
                continue;
            }

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

void Jafg::Private::LCxxRecordRegistry::KillPendingPackages()
{
    if (this->PendingPackages.empty())
    {
        return;
    }

    check( Tasks::IsOnMasterThread() )
    LOG_VERBOSE(LogPackager, "Killing [{}] pending packages...", this->PendingPackages.size())

    algo::orphan(&this->PendingPackages);

    return;
}

void Jafg::Private::LCxxRecordRegistry::SetAllowNewPendingPackages(const bool bAllow) noexcept
{
    check( Tasks::IsOnMasterThread() )

    check( this->bAllowNewPendingPackages != bAllow )
    this->bAllowNewPendingPackages = bAllow;

    return;
}

LSize Jafg::Private::LCxxRecordRegistry::RemovePackagesOf(const LLoadedPluginHandle Handle)
{
    check( Tasks::IsOnMasterThread() )
    check( this->bAllowNewPendingPackages == false )
    check( Handle.IsValid() )
    check( this->PendingPackages.empty() )

    this->Outer.KillEmployeesFromForeignPlugin(Handle, ECxxRecordTearDownReason::PluginUnload);

    LSize Out{ 0 };

    for (auto It{ this->RegisteredPackages.begin() }; this->RegisteredPackages.end() != It;)
    {
        check( (*It)->Origin.IsValid() )

        if ((*It)->Origin != Handle)
        {
            It = algo::next(It);
            continue;
        }

        if ((*It)->IsClass())
        {
            auto& ClassPackage{ (*It)->AsClass() };

            algo::erase_once_checked(&ClassPackage.StaticClass.GetParent()->GetChildren(), &ClassPackage.StaticClass);
            for (LCxxClass* Child: ClassPackage.StaticClass.GetChildren())
            {
                check( Child )
                check( Child->GetParent() == &ClassPackage.StaticClass )
                Child->Parent = nullptr;

                continue;
            }
        }

        ++Out;
        this->RegisteredPackages.erase(It);

        continue;
    }

    //# Check for invalid parents...
    for (auto& Package : this->RegisteredPackages)
    {
        if (Package->IsClass() == false)
        {
            continue;
        }

        auto& Class{ Package->AsClass().StaticClass };
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

void Jafg::Private::LCxxRecordRegistry::TearDown()
{
    check( Tasks::IsOnMasterThread() )

    algo::orphan(&this->PendingPackages);
    this->Outer.TearDown();
    algo::orphan(&this->RegisteredPackages);

    return;
}

Jafg::LCarnifex& Jafg::Private::GetGlobalCarnifex() noexcept
{
    static LCarnifex Instance;
    return Instance;
}

Jafg::Private::LCxxRecordRegistry& Jafg::Private::GetGlobalCxxRecordRegistry() noexcept
{
    static LCxxRecordRegistry Instance;
    return Instance;
}
