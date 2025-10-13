// Copyright mzoesch. All rights reserved.

#include "Lal.afx"

#if 0

#include "Engine/ObjectBaseUtility.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"
#include "Memory/MemoryMisc.h"
#include "Engine/ObjectBase.h"
#include "Engine/ObjectClass.h"
#include "Stats/Stats.h"
#include "System/ConfigIo.h"

namespace Jafg
{

void MakeDeferredObjectFinal(JObjectBase* InObject)
{
    InObject->BeginLife();
}

bool IsValidFast(const LObjectContext* InContext, const JObjectBase* InPointer)
{
    STAT_CYCLE_FUNCTION()

    check( InContext )

    if (InPointer == nullptr)
    {
        return false;
    }

    if (InContext->IsHiredHere(InPointer) == false)
    {
        return false;
    }

    return InPointer->IsGarbage() == false;
}

bool IsValidSlow(const LObjectContext* InContextPointer, const JObjectBase* InPointer)
{
    STAT_CYCLE_FUNCTION()

    if (InContextPointer == nullptr || InPointer == nullptr)
    {
        return false;
    }

    if (GEngine == nullptr)
    {
        LOG_WARNING(LogObjectInternal, "Engine is invalid.")
        return false;
    }

    if (GEngine->IsObjectContextKnown(InContextPointer) == false)
    {
        return false;
    }

    if (InContextPointer->IsHiredHere(InPointer) == false)
    {
        return false;
    }

    return InPointer->IsGarbage() == false;
}

void PullConfigFromObject(LObjectClass* InClass)
{
    jassert( InClass && InClass->IsConfig() )
    checkSlow( Tasks::IsOnMasterThread() )

    if (InClass->GetDefaultPackageReferrer()->GetClassFields().empty())
    {
        return;
    }

    const LPath CfgPath = Finder::GetUserPreferencesFile();
    if (Finder::DoesFileExist(CfgPath) == false)
    {
        return;
    }

    for (LClassField& Field : InClass->GetMutableDefaultPackageReferrer()->GetMutableClassFieldsDangerous())
    {
        checkSlow( Field.Identifier.empty() == false )

        if (TOptional<LString> StringValue = ConfigIo::Deserialize(CfgPath, InClass->GetSpacedClassName(), Field.Identifier); StringValue.has_value())
        {
            Field.Set(std::move(*StringValue));
        }

        continue;
    }

    return;
}

void PushConfigFromObject(const LObjectClass* InClass)
{
    jassert( InClass && InClass->IsConfig() )
    checkSlow( Tasks::IsOnMasterThread() )

    if (InClass->GetDefaultPackageReferrer()->GetClassFields().empty())
    {
        return;
    }

    const LPath CfgPath = Finder::GetUserPreferencesFile();
    Finder::EnsureFile(CfgPath);

    TArray<ConfigIo::Entry> Entries;
    for (LClassField& Field : InClass->GetMutableDefaultPackageReferrer()->GetMutableClassFieldsDangerous())
    {
        if (Field.Get.IsBound())
        {
            Entries.emplace_back(InClass->GetSpacedClassName(), LString{Field.Identifier}, Field.Get());
        }

        continue;
    }

    ConfigIo::SerializeBulk(CfgPath, Entries);

    return;
}

} /* ~Namespace Jafg */

void Jafg::Private::CreateSingletonObjectRegistry()
{
    check( GObjectRegistry == nullptr )
    GObjectRegistry = new LObjectRegistry();

    return;
}

void Jafg::Private::KillSingletonObjectRegistry()
{
    if (ensure(GObjectRegistry))
    {
        delete GObjectRegistry;
        GObjectRegistry = nullptr;
    }

    return;
}

TArray<Jafg::Private::LRegistrationQueuePackage>& Jafg::Private::GetRegisterObjectQueue()
{
    static TArray<Jafg::Private::LRegistrationQueuePackage> RegistrationQueue = { };
    return RegistrationQueue;
}

Jafg::JObjectBase* Jafg::Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    JObjectBase* Out = LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InStaticClass);
    Out->BeginLife();
    return Out;
}

Jafg::JObjectBase* Jafg::Private::LObjectMiscellaneousAccessor::NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    check( InContext     )
    check( InStaticClass )

    if (InStaticClass->IsAbstract())
    {
        panicMsgf( "Tried to instantiate abstract class [{}].", InStaticClass->GetSpacedClassName() )
        return nullptr;
    }

    void* Out = ::malloc(InStaticClass->GetTotalByteSize());
    check( Out )

    // ReSharper disable once CppReinterpretCastFromVoidPtr
    JObjectBase* Reinterpreted = reinterpret_cast<JObjectBase*>(Out);
    check( Reinterpreted == Out )

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* LAL_WITH_CLANG */
    ::memcpy(Out, InStaticClass->GetDefaultPackageReferrer(), InStaticClass->GetTotalByteSize());  // NOLINT(bugprone-undefined-memory-manipulation, clang-diagnostic-dynamic-class-memaccess)
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

    checkCode
    (
        checkMsgf
        (
            /* Offset of VTable ptr is POINTER_BYTE_SIZE bytes */
            ::Jafg::OffsetOf(&JObjectBase::VClass) == POINTER_BYTE_SIZE,
            "Offset is [{}].", ::Jafg::OffsetOf(&JObjectBase::VClass)
        )

        void** ActualJafgVTableLocation    = reinterpret_cast<void**>(&Reinterpreted->VClass);
        /* POINTER_BYTE_SIZE Bytes offset because of compiler generated v table pointer. */
        void** PredictedJafgVTableLocation = reinterpret_cast<void**>(reinterpret_cast<::size_t>(Out) + POINTER_BYTE_SIZE);  // NOLINT(performance-no-int-to-ptr)
        check( ActualJafgVTableLocation == PredictedJafgVTableLocation )
    )

    Reinterpreted->VClass = const_cast<LObjectClass*>(InStaticClass);
    Reinterpreted->Outer  = InContext;

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
#endif /* LAL_WITH_GCC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wnontrivial-memcall"
#endif /* LAL_WITH_CLANG */
    ::memset(&Reinterpreted->ClassFields, 0, sizeof(Reinterpreted->ClassFields));  // NOLINT(bugprone-undefined-memory-manipulation)
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* LAL_WITH_GCC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

    check( Reinterpreted->ClassFields.size() == 0 && Reinterpreted->ClassFields.data() == nullptr )

    for (const LClassField& Field : InStaticClass->GetDefaultPackageReferrer()->GetClassFields())
    {
        if (Field.Malloc)
        {
            Field.Malloc(Reinterpreted);
        }
    }

    InContext->Employees.emplace_back(Reinterpreted);

    return Reinterpreted;
}

bool Jafg::Private::LObjectMiscellaneousAccessor::DynamicCast(const JObjectBase* InObject, const LObjectClass* InTargetClass)
{
    return InObject->GetVTableChecked()->DerivesFrom(InTargetClass);
}

void Jafg::Private::LObjectRegistry::KillPendingPackages()
{
    if (Private::GetRegisterObjectQueue().empty())
    {
        return;
    }

    check( Tasks::IsOnMasterThread() )

    LOG_VERBOSE(LogObjectPackager, "Killing [{}] pending packages.", this->RegisteredObjects.size())
    algo::orphan(&this->RegisteredObjects);

    if (this->DeferredPackages.empty() == false)
    {
        LOG_ERROR(LogObjectPackager, "Found [{}] deferred packages after killing pending packages.", this->DeferredPackages.size())
        algo::orphan(&this->DeferredPackages);
    }

    return;
}

void Jafg::Private::LObjectRegistry::LoadPendingPackages(const LLoadedPluginHandle InHandle)
{
    if (Private::GetRegisterObjectQueue().empty())
    {
        return;
    }

    check( Tasks::IsOnMasterThread() )

    const auto CurrentPackages { this->RegisteredObjects.size() };
    const auto CurrentQueue { Private::GetRegisterObjectQueue().size() };

    LOG_VERBOSE(LogObjectPackager, "Loading [{}] pending packages.", Private::GetRegisterObjectQueue().size())

    for (LRegistrationQueuePackage& Package : Private::GetRegisterObjectQueue())
    {
        if (this->DoesPackageWithNameExist(Package.SpacedClassName))
        {
            panicMsgf( "Package [{}] already exists.", Package.SpacedClassName )
            continue;
        }

        LRegistryPackage NewPackage;
        NewPackage.StaticClass                         = std::make_unique<LObjectClass>();
        NewPackage.StaticClass->SpacedClassName        = Package.SpacedClassName;
        NewPackage.StaticClass->ClassName              = MAKE_NAME(NewPackage.StaticClass->SpacedClassName);
        NewPackage.StaticClass->PluginHandle           = InHandle;
        NewPackage.StaticClass->DefaultPackageReferrer = Package.GetContentDefault();
        check( NewPackage.StaticClass->SpacedClassName.empty() == false )
        check( NewPackage.StaticClass->DefaultPackageReferrer != nullptr )

        this->RegisteredObjects.emplace_back(std::move(NewPackage));

        Package.Callback(this->RegisteredObjects.back().StaticClass.get());

        continue;
    }

    check( Private::GetRegisterObjectQueue().size() == CurrentQueue )
    algo::orphan(&Private::GetRegisterObjectQueue());

    for (auto& [SuperName, StaticClass] : this->DeferredPackages)
    {
        if (SuperName == "NextIsObjectBaseClass")
        {
            /*
             * The root package.
             */
            continue;
        }

        LRegistryPackage* ParentPackage = GetPanickedPackageByNameWeak(SuperName);
        if (algo::contains(ParentPackage->StaticClass->GetChildren(), StaticClass))
        {
            panicMsgf
            (
                "Tried resolving deferred package [{}] that already has registered its child.",
                StaticClass->GetSpacedClassName()
            )
            continue;
        }

        ParentPackage->StaticClass->GetChildren().emplace_back(StaticClass);
        StaticClass->Parent = ParentPackage->StaticClass.get();

        continue;
    }
    algo::orphan(&this->DeferredPackages);

    for (auto& [UniqueStaticClass] : this->RegisteredObjects)
    {
        auto* StaticClass = UniqueStaticClass.get();

        if (StaticClass->IsConfig())
        {
            PullConfigFromObject(StaticClass);
        }

        StaticClass->DefaultPackageReferrer->BeginLifeDefault();

        LOG_TRACE
        (
            LogObjectPackager,
            "Finished loading package for [{} ({}b)].",
            StaticClass->GetSpacedClassName(),
            StaticClass->GetTotalByteSize()
        )

        continue;
    }

    this->ValidateLoadedPackages();

    return;
}

void Jafg::Private::LObjectRegistry::ValidateLoadedPackages()
{
    bool bRootFound = false;
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.GetSpacedClassName().empty())
        {
            panic( "Found loaded package with empty name." )
            continue;
        }

        if (Package.StaticClass == nullptr)
        {
            panicMsgf( "Found loaded package [{}] with no static class.", Package.GetSpacedClassName() )
            continue;
        }

        if (Package.StaticClass->DefaultPackageReferrer == nullptr)
        {
            panicMsgf( "Found loaded package [{}] with no default referrer.", Package.GetSpacedClassName() )
            continue;
        }

        if (Package.StaticClass->GetParent() == nullptr)
        {
            if (bRootFound)
            {
                panicMsgf( "Found loaded package [{}] with no parent or found multiple root objects.", Package.GetSpacedClassName() )
                continue;
            }

            bRootFound = true;
            continue;
        }

        if (Package.StaticClass->TotalByteSize == INDEX_NONE)
        {
            panicMsgf( "Found loaded package [{}] with no byte size.", Package.GetSpacedClassName() )
            continue;
        }

        for (const LRegistryPackage& OtherPackage : this->RegisteredObjects)
        {
            if (&Package == &OtherPackage)
            {
                continue;
            }

            if (Package.GetSpacedClassName() == OtherPackage.GetSpacedClassName())
            {
                panicMsgf( "Found duplicate package names [%s].", Package.GetSpacedClassName() )
                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

bool Jafg::Private::LObjectRegistry::DoesPackageWithNameExist(const LString& SpacedClassName) const
{
    return this->GetPackageByName(SpacedClassName) != nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByName(const LString& SpacedClassName)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.GetSpacedClassName() == SpacedClassName)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByName(const LString& SpacedClassName) const
{
    return const_cast<LObjectRegistry*>(this)->GetPackageByName(SpacedClassName);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByNameWeak(const LString& Name)
{
    if (LRegistryPackage* Package = this->GetPackageByName(Name))
    {
        return Package;
    }

    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.GetSpacedClassName().ends_with(Name))
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByNameWeak(const LString& Name) const
{
    return const_cast<LObjectRegistry*>(this)->GetPackageByNameWeak(Name);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LString& SpacedClassName)
{
    if (LRegistryPackage* Package = this->GetPackageByName(SpacedClassName))
    {
        return Package;
    }

    panicMsgf( "Failed to find package [{}].", SpacedClassName )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LString& SpacedClassName) const
{
    return const_cast<LObjectRegistry*>(this)->GetPanickedPackageByName(SpacedClassName);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByNameWeak(const LString& Name)
{
    if (LRegistryPackage* Package = this->GetPackageByNameWeak(Name))
    {
        return Package;
    }

    panicMsgf( "Failed to find package [{}].", Name )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByNameWeak(const LString& Name) const
{
    return const_cast<LObjectRegistry*>(this)->GetPanickedPackageByNameWeak(Name);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByStaticClass(const void* StaticClass)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass.get() == StaticClass)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByStaticClass(const void* StaticClass)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass.get() == StaticClass)
        {
            return &Package;
        }

        continue;
    }

    panic( "Failed to find package by static class." )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByStaticClass(const void* StaticClass) const
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass.get() == StaticClass)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByStaticClass(const void* StaticClass) const
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass.get() == StaticClass)
        {
            return &Package;
        }

        continue;
    }

    panic( "Failed to find package by static class." )

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByContentDefault(const void* ContentDefaultReferrer)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass->GetDefaultPackageReferrer() == ContentDefaultReferrer)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByContentDefault(const void* ContentDefaultReferrer)
{
    if (LRegistryPackage* Package = this->GetPackageByContentDefault(ContentDefaultReferrer); Package)
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
}

void Jafg::Private::LObjectRegistry::GetRegisteredObjectsOfClass(
    const LObjectClass*          InStaticClass,
    TArray<const LObjectClass*>* OutArray
) const
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass->DerivesFrom(InStaticClass))
        {
            OutArray->emplace_back(Package.StaticClass.get());
        }

        continue;
    }

    return;
}

i32 Jafg::Private::LObjectRegistry::RemovePackagesOf(const LLoadedPluginHandle InHandle)
{
    check( Tasks::IsOnMasterThread() )

    i32 Removed { 0 };
    for (TArray<LRegistryPackage>::size_type Idx { 0 }; Idx < this->RegisteredObjects.size();)
    {
        LRegistryPackage& Package = this->RegisteredObjects[Idx];

        if (Package.StaticClass->GetPluginHandle() != InHandle)
        {
            ++Idx;
            continue;
        }

        algo::erase_once_checked(&Package.StaticClass->Parent->GetChildren(), Package.StaticClass.get());
        for (LObjectClass* Child: Package.StaticClass->GetChildren())
        {
            check( Package.StaticClass.get() == Child->Parent  )
            Child->Parent = nullptr;

            continue;
        }

        ++Removed;
        this->RegisteredObjects.erase(this->RegisteredObjects.begin() + Idx);

        continue;
    }

    return Removed;
}


#endif
