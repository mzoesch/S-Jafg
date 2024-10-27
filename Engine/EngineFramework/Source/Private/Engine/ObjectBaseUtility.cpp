// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectBaseUtility.h"

#include "Engine/ObjectBase.h"
#include "Engine/ObjectClass.h"

namespace Jafg::Private
{

ENGINEFRAMEWORK_API LObjectRegistry* GObjectRegistry = nullptr;

} /* ~Namespace Jafg::Private */

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

Jafg::Private::JObjectBase* Jafg::Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* Context, const LObjectClass* StaticClass)
{
    check( StaticClass )

    void* Out = ::malloc(StaticClass->GetTotalByteSize());
    check( Out )
    ::memcpy(Out, StaticClass->GetDefaultPackageReferrer(), StaticClass->GetTotalByteSize());  // NOLINT(bugprone-undefined-memory-manipulation)

    // ReSharper disable once CppReinterpretCastFromVoidPtr
    reinterpret_cast<JObjectBase*>(Out)->Outer = Context ? Context : nullptr;
    // ReSharper disable once CppReinterpretCastFromVoidPtr
    reinterpret_cast<JObjectBase*>(Out)->BeginLife();

    // ReSharper disable once CppReinterpretCastFromVoidPtr
    return reinterpret_cast<JObjectBase*>(Out);
}

void Jafg::Private::LObjectRegistry::LoadPendingPackages()
{
    for (LRegistrationQueuePackage& Package : Private::GetRegisterObjectQueue())
    {
        if (this->DoesPackageWithNameExist(Package.SpacedClassName))
        {
            panicMsgf( "Package [{}] already exists.", Package.SpacedClassName )
            continue;
        }

        LRegistryPackage NewPackage;
        NewPackage.SpacedClassName                     = Package.SpacedClassName;
        NewPackage.StaticClass                         = new LObjectClass();
        NewPackage.StaticClass->DefaultPackageReferrer = Package.GetContentDefault();
        check( NewPackage.StaticClass->DefaultPackageReferrer != nullptr )

        this->RegisteredObjects.Add(std::forward<LRegistryPackage>(NewPackage));

        Package.Callback(NewPackage.StaticClass);

        continue;
    }

    Private::GetRegisterObjectQueue().Empty();

    this->ValidateLoadedPackages();

    return;
}

void Jafg::Private::LObjectRegistry::ValidateLoadedPackages()
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.SpacedClassName.IsEmpty())
        {
            panic( "Found loaded package with empty name." )
            continue;
        }

        if (Package.StaticClass == nullptr)
        {
            panicMsgf( "Found loaded package [{}] with no static class.", Package.SpacedClassName )
            continue;
        }

        if (Package.StaticClass->DefaultPackageReferrer == nullptr)
        {
            panicMsgf( "Found loaded package [{}] with no default referrer.", Package.SpacedClassName )
            continue;
        }

        if (Package.StaticClass->TotalByteSize == INDEX_NONE)
        {
            panicMsgf( "Found loaded package [{}] with no byte size.", Package.SpacedClassName )
            continue;
        }

        for (const LRegistryPackage& OtherPackage : this->RegisteredObjects)
        {
            if (&Package == &OtherPackage)
            {
                continue;
            }

            if (Package.SpacedClassName == OtherPackage.SpacedClassName)
            {
                panicMsgf( "Found duplicate package names [%s].", Package.SpacedClassName )
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
        if (Package.SpacedClassName == SpacedClassName)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByName(const LSimpleString& SpacedClassName) const
{
    for (const LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.SpacedClassName == SpacedClassName)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LSimpleString& SpacedClassName)
{
    if (LRegistryPackage* Package = this->GetPackageByName(SpacedClassName))
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByName(const LSimpleString& SpacedClassName) const
{
    if (const LRegistryPackage* Package = this->GetPackageByName(SpacedClassName))
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
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
