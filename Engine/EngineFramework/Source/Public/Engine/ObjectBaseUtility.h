// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg::Private
{

class JObjectBase;
class LObjectRegistry;
struct TRegistryPackageBase;
struct LRegistrationQueuePackage;

typedef JObjectBase* (*GetContentDefaultFunctor)(void);
typedef void (*OnRegistrationDelegate)(JObjectBase* DefaultPackageReferrer);

/**
 * Global application wide singleton object registry.
 */
ENGINEFRAMEWORK_API extern LObjectRegistry* GObjectRegistry;

ENGINEFRAMEWORK_API void CreateSingletonObjectRegistry(void);
ENGINEFRAMEWORK_API void KillSingletonObjectRegistry(void);

/** @return All objects that are waiting for registration. */
ENGINEFRAMEWORK_API auto GetRegisterObjectQueue() -> TdhArray<LRegistrationQueuePackage>&;

/** Registers a new object type to the global (in this shared translation unit) registry. */
template <typename TObj>
FORCEINLINE auto RegisterNewObjectType(
    /** Full namespaced name of the target class. */
    LSimpleString            SpacedClassName,
    /** Delegate that returns a clean default object of the target class. */
    GetContentDefaultFunctor GetContentDefaultDelegate,
    /** Delegate that is called when the object has been registered. */
    OnRegistrationDelegate   Callback
) -> void;

struct LRegistrationQueuePackage final
{
    /** Full namespaced name of the target class. */
    LSimpleString            SpacedClassName;
    /** Delegate that returns a clean default object of the target class. */
    GetContentDefaultFunctor GetContentDefault;
    /** Delegate that is called when the object has been registered. */
    OnRegistrationDelegate   Callback;
};

/**
 * A package that holds a generic derived JObjectBase object.
 * This object is known to the engine as it has been registered and initialized.
 */
struct LRegistryPackage final
{
    /** Full namespaced name of the target class. */
    LSimpleString SpacedClassName;
    /** Pointer to the default content object of the target class. */
    JObjectBase*  DefaultPackageReferrer;
};

/** Singleton registry that holds all content, that is considered default, to an object. */
class LObjectRegistry final
{
public:

    LObjectRegistry()  = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LObjectRegistry)
    ~LObjectRegistry() = default;

    /**
     * Registers all pending packages that are waiting for registration.
     * Loads them into memory and initializes the default package referrer for them.
     */
    ENGINEFRAMEWORK_API void LoadPendingPackages(void);

    ENGINEFRAMEWORK_API auto DoesPackageWithNameExist(const LSimpleString& SpacedClassName) const -> bool;
    ENGINEFRAMEWORK_API auto GetPackageWithName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPackageWithName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageWithName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageWithName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;

    ENGINEFRAMEWORK_API auto GetPackageByPointer(const void* ContentDefaultReferrer) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByPointer(const void* ContentDefaultReferrer) -> LRegistryPackage*;

    FORCEINLINE auto GetRegisteredObjects() -> TdhArray<LRegistryPackage>& { return this->RegisteredObjects; }

private:

    TdhArray<LRegistryPackage> RegisteredObjects;
};

template <typename TObj>
void RegisterNewObjectType(
    LSimpleString            SpacedClassName,
    GetContentDefaultFunctor GetContentDefaultDelegate,
    OnRegistrationDelegate   Callback
)
{
    static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");

    Private::GetRegisterObjectQueue().Emplace(
        std::forward<LSimpleString>(SpacedClassName),
        GetContentDefaultDelegate,
        Callback
    );

    return;

}

/** Global static helper struct to allow for private member access through derived classes of JObjectBase. */
struct LRegistrationCallbackHelper final
{
    LRegistrationCallbackHelper() = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LRegistrationCallbackHelper)
    ~LRegistrationCallbackHelper() = delete;

    template <typename TObj = JObjectBase>
    static void DoRegisterContentsForClass(TObj* DefaultPackageReferrer)
    {
        jassert( DefaultPackageReferrer != nullptr )
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");

        TObj::ClassName           = GObjectRegistry->GetPanickedPackageByPointer(DefaultPackageReferrer)->SpacedClassName;
        TObj::StaticClassReferrer = DefaultPackageReferrer;

        check( TObj::StaticClassReferrer )

        LOG_DEBUG(LogTemporal, "Finished registering class [{}].", TObj::StaticClassName())

        return;
    }
};

} /* ~Namespace Jafg::Private */
