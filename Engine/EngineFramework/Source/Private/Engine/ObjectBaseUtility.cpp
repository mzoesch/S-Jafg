// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectBaseUtility.h"

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

void Jafg::Private::LObjectRegistry::LoadPendingPackages()
{
    for (LRegistrationQueuePackage& Package : Private::GetRegisterObjectQueue())
    {
        if (this->DoesPackageWithNameExist(Package.SpacedClassName))
        {
            panic( "Package already exists." )
            continue;
        }

        LRegistryPackage NewPackage;
        NewPackage.SpacedClassName        = Package.SpacedClassName;
        NewPackage.DefaultPackageReferrer = Package.GetContentDefault();

        this->RegisteredObjects.Add(NewPackage);

        LRegistryPackage* NewPackageRef = this->RegisteredObjects.GetLast();

        Package.Callback(NewPackage.DefaultPackageReferrer);

        continue;
    }

    Private::GetRegisterObjectQueue().Empty();

    return;
}

bool Jafg::Private::LObjectRegistry::DoesPackageWithNameExist(const LSimpleString& SpacedClassName) const
{
    return this->GetPackageWithName(SpacedClassName) != nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageWithName(const LSimpleString& SpacedClassName)
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

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageWithName(const LSimpleString& SpacedClassName) const
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

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageWithName(const LSimpleString& SpacedClassName)
{
    if (LRegistryPackage* Package = this->GetPackageWithName(SpacedClassName))
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
}

const Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageWithName(const LSimpleString& SpacedClassName) const
{
    if (const LRegistryPackage* Package = this->GetPackageWithName(SpacedClassName))
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPackageByPointer(const void* ContentDefaultReferrer)
{
    for (LRegistryPackage& Package : this->RegisteredObjects)
    {
        if (Package.DefaultPackageReferrer == ContentDefaultReferrer)
        {
            return &Package;
        }

        continue;
    }

    return nullptr;
}

Jafg::Private::LRegistryPackage* Jafg::Private::LObjectRegistry::GetPanickedPackageByPointer(const void* ContentDefaultReferrer)
{
    if (LRegistryPackage* Package = this->GetPackageByPointer(ContentDefaultReferrer); Package)
    {
        return Package;
    }

    panic( "Failed to find package." )

    return nullptr;
}
