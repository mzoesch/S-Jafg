// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/ObjectBaseUtility.h"
#include "Memory/MemoryMisc.h"
#include "Engine/ObjectBase.h"
#include "Engine/ObjectClass.h"

namespace Jafg
{

ENGINE_API Private::LObjectContext* GOmniVitaContext = nullptr;

namespace Private
{

ENGINE_API LObjectRegistry* GObjectRegistry   = nullptr;
ENGINE_API LCarnifex**      GCarnifexReferrer = nullptr;

} /* ~Namespace Private */

void MakeDeferredObjectFinal(Private::JObjectBase* InObject)
{
    InObject->BeginLife();
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

Jafg::TdhArray<Jafg::Private::LRegistrationQueuePackage>& Jafg::Private::GetRegisterObjectQueue()
{
    static Jafg::TdhArray<Jafg::Private::LRegistrationQueuePackage> RegistrationQueue = { };
    return RegistrationQueue;
}

Jafg::Private::JObjectBase* Jafg::Private::LObjectMiscellaneousAccessor::NewObject(
    LObjectContext*     InContext,
    const LObjectClass* InStaticClass
)
{
    JObjectBase* Out = LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InStaticClass);
    Out->BeginLife();
    return Out;
}

Jafg::Private::JObjectBase* Jafg::Private::LObjectMiscellaneousAccessor::NewDeferredObject(
    LObjectContext*     InContext,
    const LObjectClass* InStaticClass
)
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

#if PLATFORM_WASM
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* PLATFORM_WASM */
    ::memcpy(Out, InStaticClass->GetDefaultPackageReferrer(), InStaticClass->GetTotalByteSize());  // NOLINT(bugprone-undefined-memory-manipulation)
#if PLATFORM_WASM
    #pragma GCC diagnostic pop
#endif /* PLATFORM_WASM */

    checkCode(
        checkMsgf(
            /* Offset of VTable ptr is POINTER_BYTE_SIZE bytes */
            ::Jafg::OffsetOf(&JObjectBase::VClass) == POINTER_BYTE_SIZE,
            "Offset is [{}].", ::Jafg::OffsetOf(&JObjectBase::VClass)
        )

        void** ActualJafgVTableLocation    = reinterpret_cast<void**>(&Reinterpreted->VClass);
        /* POINTER_BYTE_SIZE Bytes offset because of compiler generated v table pointer. */
        void** PredictedJafgVTableLocation = reinterpret_cast<void**>(reinterpret_cast<::size_t>(Out) + POINTER_BYTE_SIZE);
        check( ActualJafgVTableLocation == PredictedJafgVTableLocation )
    )

    Reinterpreted->VClass = const_cast<LObjectClass*>(InStaticClass);
    Reinterpreted->Outer  = InContext;

    InContext->Employees.Emplace(Reinterpreted);

    return Reinterpreted;
}

bool Jafg::Private::LObjectMiscellaneousAccessor::DynamicCast(const JObjectBase* InObject, const LObjectClass* InTargetClass)
{
    return InObject->GetVTable()->DerivesFrom(InTargetClass);
}

void Jafg::Private::LObjectRegistry::LoadPendingPackages()
{
    if (Private::GetRegisterObjectQueue().IsEmpty())
    {
        return;
    }

    const int32 CurrentPackages = this->RegisteredObjects.GetSize();

    LOG_VERBOSE(LogObjectPackager, "Loading [{}] pending packages.", Private::GetRegisterObjectQueue().GetSize())

    for (LRegistrationQueuePackage& Package : Private::GetRegisterObjectQueue())
    {
        if (this->DoesPackageWithNameExist(Package.SpacedClassName))
        {
            panicMsgf( "Package [{}] already exists.", Package.SpacedClassName )
            continue;
        }

        LRegistryPackage NewPackage;
        NewPackage.StaticClass                         = new LObjectClass();
        NewPackage.StaticClass->SpacedClassName        = Package.SpacedClassName;
        NewPackage.StaticClass->DefaultPackageReferrer = Package.GetContentDefault();
        check( NewPackage.StaticClass->SpacedClassName.IsEmpty() == false )
        check( NewPackage.StaticClass->DefaultPackageReferrer != nullptr )

        this->RegisteredObjects.Add(std::forward<LRegistryPackage>(NewPackage));

        Package.Callback(NewPackage.StaticClass);

        continue;
    }

    Private::GetRegisterObjectQueue().Empty();

    for (auto& [SuperName, StaticClass] : this->DeferredPackages)
    {
        if (SuperName == "NextIsObjectBaseClass")
        {
            /*
             * The root package.
             */
            continue;
        }

        const LRegistryPackage* ParentPackage = GetPanickedPackageByNameWeak(SuperName);
        if (ParentPackage->StaticClass->GetChildren().Contains(StaticClass))
        {
            panicMsgf(
                "Tried resolving deferred package [{}] that already has registered its child.",
                StaticClass->GetSpacedClassName()
            )
            continue;
        }

        ParentPackage->StaticClass->GetChildren().Add(StaticClass);
        StaticClass->Parent = ParentPackage->StaticClass;

        continue;
    }

    this->DeferredPackages.Empty();

    for (int32 i = CurrentPackages; i < this->RegisteredObjects.GetSize(); ++i)
    {
        auto& [StaticClass] = this->RegisteredObjects[i];

        LOG_TRACE(
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
        if (Package.GetSpacedClassName().IsEmpty())
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

bool Jafg::Private::LObjectRegistry::DoesPackageWithNameExist(const LSimpleString& SpacedClassName) const
{
    return this->GetPackageByName(SpacedClassName) != nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByName(const LSimpleString& SpacedClassName)
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

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByName(const LSimpleString& SpacedClassName) const
{
    return const_cast<LObjectRegistry*>(this)->GetPackageByName(SpacedClassName);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByNameWeak(const LSimpleString& Name)
{
    if (LRegistryPackage* Package = this->GetPackageByName(Name))
    {
        return Package;
    }

    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.GetSpacedClassName().Contains(Name))
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByNameWeak(const LSimpleString& Name) const
{
    return const_cast<LObjectRegistry*>(this)->GetPackageByNameWeak(Name);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LSimpleString& SpacedClassName)
{
    if (LRegistryPackage* Package = this->GetPackageByName(SpacedClassName))
    {
        return Package;
    }

    panicMsgf( "Failed to find package [{}].", SpacedClassName )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LSimpleString& SpacedClassName) const
{
    return const_cast<LObjectRegistry*>(this)->GetPanickedPackageByName(SpacedClassName);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByNameWeak(const LSimpleString& Name)
{
    if (LRegistryPackage* Package = this->GetPackageByNameWeak(Name))
    {
        return Package;
    }

    panicMsgf( "Failed to find package [{}].", Name )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByNameWeak(const LSimpleString& Name) const
{
    return const_cast<LObjectRegistry*>(this)->GetPanickedPackageByNameWeak(Name);
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByStaticClass(const void* StaticClass)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass == StaticClass)
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
        if (Package.StaticClass == StaticClass)
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
        if (Package.StaticClass == StaticClass)
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
        if (Package.StaticClass == StaticClass)
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
    const LObjectClass*            InStaticClass,
    TdhArray<const LObjectClass*>& OutArray
) const
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.StaticClass->DerivesFrom(InStaticClass))
        {
            OutArray.Add(Package.StaticClass);
        }

        continue;
    }

    return;
}
