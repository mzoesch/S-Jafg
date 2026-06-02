// Copyright mzoesch. All rights reserved.

#include "Engine/Jxx.h"
#include "Engine/Engine.h"
#include "Stats/Stats.h"

Jafg::LEngine const& Jafg::LEngineGetters::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine is undefined behavior.")
    return *GEngine;
}

Jafg::LEngine& Jafg::LEngineGetters::GetMutableEngine() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine is undefined behavior.")
    return *Detail::GMutableEngine;
}

Jafg::LLocalEgo const& Jafg::LEngineGetters::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine is undefined behavior.")
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo& Jafg::LEngineGetters::GetMutableLocalEgo() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine is undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo();
}

Jafg::LFrontend const& Jafg::LEngineGetters::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine is undefined behavior.")
    return GEngine->GetLocalEgo().GetFrontend();
}

Jafg::LFrontend& Jafg::LEngineGetters::GetMutableFrontend() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine is undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo().GetFrontend();
}

Jafg::LCommandLineInterface const& Jafg::LEngineGetters::GetCommandLineInterface() const noexcept
{
    check(GEngine && "Absence of GEngine is undefined behavior.")
    return GEngine->GetCommandLineInterface();
}

Jafg::LCommandLineInterface& Jafg::LEngineGetters::GetMutableCommandLineInterface() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine is undefined behavior.")
    return Detail::GMutableEngine->GetCommandLineInterface();
}

Jafg::Detail::LReflectedTagRegistry& Jafg::Detail::GetJxxTagRegistry() noexcept
{
    static LReflectedTagRegistry Result;
    return Result;
}

void Jafg::Detail::LJxxRecordRegistry::AddNewPendingClassPackage(TUnique<LRegistryClassPackage> Package)
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

void Jafg::Detail::LJxxRecordRegistry::LoadPendingPackages(const LLoadedPluginHandle Handle)
{
    check(this->bAllowNewPendingPackages == false)

    if (this->PendingPackages.empty())
    {
        return;
    }

    check(Tasks::IsOnMasterThread())
    LOG_VERBOSE(LogPackager, "Loading [{}] pending packages...", this->PendingPackages.size())

    TArray<LJxxClass*> NewClasses;

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

        if (Class.ParentName == "NextIsBaseJxxClass")
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
    std::optional<LString> RootClassName;
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
            std::unreachable();
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

void Jafg::Detail::LJxxRecordRegistry::KillPendingPackages()
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

void Jafg::Detail::LJxxRecordRegistry::SetAllowNewPendingPackages(const bool bAllow) noexcept
{
    check(Tasks::IsOnMasterThread())

    check(this->bAllowNewPendingPackages != bAllow)
    this->bAllowNewPendingPackages = bAllow;

    return;
}

std::size_t Jafg::Detail::LJxxRecordRegistry::RemovePackagesOf(const LLoadedPluginHandle Handle)
{
    check(Tasks::IsOnMasterThread())
    check(this->bAllowNewPendingPackages == false)
    check(Handle.IsValid())
    check(this->PendingPackages.empty())

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    this->RemoveSingletonsOf(Handle);

#if JAFG_DO_CHECKS
    std::size_t It1{};
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

    std::size_t Out{};
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
            for (LJxxClass* Child : ClassPackage.StaticClass.GetChildren())
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
            if (Class.ParentName == "NextIsBaseJxxClass")
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

void Jafg::Detail::LJxxRecordRegistry::TearDown()
{
    check(Tasks::IsOnMasterThread())

    check(this->SingletonOuter)

    this->RemoveSingletonsOf(std::nullopt);
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

void Jafg::Detail::LJxxRecordRegistry::RemoveSingletonsOf(std::optional<LLoadedPluginHandle> Handle)
{
    LOG_VERBOSE(LogPackager, "Removing singletons of plugin [{}]...", Handle.has_value() ? algo::sprintf("0x{:X}", static_cast<u32>(*Handle)) : "nullopt")

    if (this->SingletonOuter)
    {
        for (auto Idx{0uz}; Idx < this->SingletonOuter->GetEmployees().size();)
        {
            auto const* Singleton{&*this->SingletonOuter->GetEmployees()[Idx]};
            check(Singleton->GetVirtualTable().IsSingleton())
            auto const& VTable{Singleton->GetVirtualTable()};
            if (!Handle || VTable.GetPluginHandle() == Handle)
            {
                check(VTable.Singleton)
                check(VTable.Singleton == &*Singleton)
                this->SingletonOuter->GetEmployees()[Idx]->MarkAsGarbage_v2(EJxxRecordTearDownReason::PluginUnload);
                check(VTable.Singleton == &*Singleton)
                const_cast<LJxxClass&>(VTable).Singleton = nullptr;
                continue;
            }
            ++Idx;
            continue;
        }
    }

    return;
}

Jafg::Detail::LJxxRecordRegistry& Jafg::Detail::GetGlobalCxxRecordRegistry() noexcept
{
    static LJxxRecordRegistry Instance;
    return Instance;
}

bool Jafg::LJxxClass::DerivesFrom(LJxxClass const& Parent) const noexcept
{
    LJxxClass const* Cursor{this};
    while (Cursor != nullptr)
    {
        if (Cursor == &Parent)
        {
            return true;
        }

        Cursor = Cursor->Parent;
        continue;
    }

    return false;
}

void Jafg::JCxxClass::PullConfig(LPath const& InPath /* = {} */) noexcept
{
    // TODO: Add arg to ignore pulling
    check(Detail::GMutableEngine && "Absence of GMutableEngine if undefined behavior.")

    LPath Path{InPath};
    if (Path == LPath{})
    {
        Path = Finder::GetUserPreferencesFile();
    }

    auto& Class{const_cast<LJxxClass&>(this->GetVirtualTable())};
    LOG_VERBOSE(LogObjectInternal, "[{}{}]: Pulling config.", Path, this->GetNameAsString())
    check(Class.IsConfig())

    Detail::GMutableEngine->Config.PullConfigFile(Path);
    if (auto* Section{Detail::GMutableEngine->Config.GetConfigSection(Path, this->GetNameAsString())})
    {
        for (auto& [Key, Value] : *Section)
        {
            if (auto It{algo::find(Class.GetMutableFieldsDangerous(), Key, &LJxxClassField::Identifier)}; It != Class.GetFields().end())
            {
                LOG_VERBOSE(LogObjectInternal, "[{}{}]: Overriding entry [{}] from [{}] to [{}]."
                    , Path, this->GetNameAsString(), Key, It->Get(*this), Value
                    )
                It->Set(this, Value);
            }
        }
    }

    return;
}

void Jafg::JCxxClass::PushConfig(LPath const& InPath /* = {} */) const noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine if undefined behavior.")

    LPath Path{InPath};
    if (Path == LPath{})
    {
        Path = Finder::GetUserPreferencesFile();
    }

    auto& Class{this->GetVirtualTable()};
    LOG_VERBOSE(LogObjectInternal, "[{}{}]: Pushing config.", Path, this->GetNameAsString())
    check(Class.IsConfig())

    std::unordered_map<LString, LString> Entries;
    Entries.reserve(Class.GetFields().size());
    for (LJxxClassField const& Field : Class.GetFields())
    {
        check(Field.IsModified && Field.Get)
        if (Field.IsModified(*this))
        {
            LOG_VERBOSE(LogObjectInternal, "[{}{}]: Updating entry [{}] to latest since it is modified."
                , Path, this->GetNameAsString(), Field.Identifier)
            Entries[LString{Field.Identifier}] = Field.Get(*this);
        }
        else
        {
            if (Detail::GMutableEngine->Config.RemoveConfigValue(Path, this->GetNameAsString(), LString{Field.Identifier}))
            {
                LOG_VERBOSE(LogObjectInternal, "[{}{}]: Removing entry [{}] since it is not modified anymore."
                    , Path, this->GetNameAsString(), Field.Identifier)
            }
        }
    }
    if (!Entries.empty())
    {
        Detail::GMutableEngine->Config.AddConfigSection(Path, this->GetNameAsString(), Entries);
    }

    return;
}

void Jafg::JCxxClass::MarkAsGarbage(EMarkAsGarbageBehavior Behavior, EJxxRecordTearDownReason Reason)
{
    check(Tasks::IsOnMasterThread())

    check(this->_HasBegunLife())
    check(this->_IsGarbage() == false)
    this->bGarbage = true;

    this->OnGarbage(Reason);

    TUnique<JCxxClass> Self;
    if (Reason == EJxxRecordTearDownReason::OuterTearDown)
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
        std::unreachable();
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

void Jafg::Detail::LCarnifex::KillAllGarbageChildren()
{
    if (this->GarbageChildren.empty())
    {
        return;
    }

    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogCarnifex, "Found {} garbage children. Begin to kill them.", this->GarbageChildren.size())

    while (this->GarbageChildren.empty() == false)
    {
        auto& Child{this->GarbageChildren.back()};
        check( Child->_IsGarbage() )
        this->GarbageChildren.pop_back();
        continue;
    }

    check(this->GarbageChildren.empty())

    return;
}

void Jafg::Detail::LCarnifex::DevourGarbageChildNow(TUnique<JCxxClass> Child)
{
    check(&*Child)

    if (auto It{algo::find(this->GarbageChildren, Child.get(), algo::unique_raw)}; It != this->GarbageChildren.end())
    {
        checkSlow(It->get() == Child.get())
        check(It->get()->_IsGarbage())
        this->GarbageChildren.erase(It);
    }
    else
    {
        if (Child->_IsGarbage() == false)
        {
            LOG_WARNING(LogCarnifex,
                "The provided child is not garbage - but still alive. {}",
                Child->GetVirtualTable().GetFullyQualifiedName()
                )
            Child->MarkAsGarbage(JCxxClass::EMarkAsGarbageBehavior::Ignore, EJxxRecordTearDownReason::Default);
        }
        check(!algo::contains(this->GarbageChildren, Child.get(), algo::unique_raw))
    }

    check(Child.get())
    check(Child->_IsGarbage())
    check(!Child->Outer.IsHiredHere(&*Child))

    return;
}

Jafg::Detail::LCarnifex& Jafg::Detail::GetGlobalCarnifex() noexcept
{
    static LCarnifex Instance;
    return Instance;
}

Jafg::LClassOuter::LClassOuter(LString HumanReadableName, bool bRegisterToEngine /* = true */) noexcept
{
    check(Tasks::IsOnMasterThread())

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
        check(!this->bWasRegisteredToEngine)
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
        E->MarkAsGarbage_v2(EJxxRecordTearDownReason::OuterTearDown);
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
    EJxxRecordTearDownReason Reason /* = EJxxRecordTearDownReason::PluginUnload */
    )
{
    check(Tasks::IsOnMasterThread())
    check(PluginHandle.IsValid())

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
        LOG_VERBOSE(LogClassOuter
            , "Removed {} employees from outer [{}] that were loaded by a foreign plugin."
            , KillCount, this->HumanReadableName
            )
        Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    }

    return KillCount;
}

void Jafg::LClassOuter::RegisterToEngine()
{
    check(Tasks::IsOnMasterThread())

    if (GEngine)
    {
        Detail::GMutableEngine->RegisterClassOuter(this);
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [this]
        {
            check(Detail::GMutableEngine)
            Detail::GMutableEngine->RegisterClassOuter(this);
            return;
        });
    }

    return;
}

void Jafg::LClassOuter::UnregisterFromEngine()
{
    check(Tasks::IsOnMasterThread())
    check(Detail::GMutableEngine)

    Detail::GMutableEngine->UnregisterClassOuter(this);

    return;
}

bool Jafg::IsValidSlow(LClassOuter const* Outer, JCxxClass const* Obj)
{
    STAT_CYCLE_FUNCTION()

    if (Outer == nullptr || Obj == nullptr)
    {
        return false;
    }

    if (GEngine == nullptr)
    {
        LOG_WARNING(LogObjectInternal, "Engine is invalid.")
        return false;
    }

    if (!GEngine->IsClassOuterKnown(Outer))
    {
        return false;
    }

    if (!Outer->IsHiredHere(Obj))
    {
        return false;
    }

    return !Obj->_IsGarbage();
}
