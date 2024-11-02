// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "ObjectClass.h"
#include "Engine/ObjectContext.h"


///////////////////////////////////////////////////////////////////////////////
// Compiler options

/**
 * Whether the c++ compiler should check for pure virtual functions, and if
 * they have been overriden by any derived class. Usually disabled as the
 * program may not run with this option enabled.
 * Usually, this program crashes if it encounters a non-implemented pure
 * virtual method.
 */
#define DO_PURE_VIRTUAL_COMPILER_CHECKS                 0

// ~Compiler options
///////////////////////////////////////////////////////////////////////////////

#if DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define PURE_VIRTUAL(...) = 0;
#else /* DO_PURE_VIRTUAL_COMPILER_CHECKS */
    /** Define a RetTy for non-void members if needed. */
    #define PURE_VIRTUAL(RetTy, ...) { panic( "Pure virtual function was encountered." ) RetTy; ##__VA_ARGS__; }
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

/** A member that was derived but is not callable. */
#define NON_CALLABLE_MEMBER(RetTy, ...) { panic( "Non-callable member function was encountered." ) RetTy; ##__VA_ARGS__; }

namespace Jafg
{

namespace Private
{

class JObjectBase;
class JObjectBase;
class LObjectRegistry;
struct TRegistryPackageBase;
struct LRegistrationQueuePackage;
struct LRegistrationCallbackHelper;
struct LObjectMiscellaneousAccessor;

typedef JObjectBase* (*GetContentDefaultFunctor)(void);
typedef void (*OnRegistrationDelegate)(LObjectClass* StaticClass);

} /* ~Namespace Private */

/** A global context that shares the lifetime of the program (not engine!). */
ENGINEFRAMEWORK_API extern Private::LObjectContext* GOmniVitaContext;

/** Allocate a new object of type TObj. */
template <typename TObj>
FORCEINLINE auto NewObject() -> TObj*;
/** Allocate a new object of type TObj within a given context. */
template <typename TObj>
FORCEINLINE auto NewObject(Private::LObjectContext* InContext) -> TObj*;
/** Allocate a new object of type TObj within a given context and with a given static class that might be more specialist. */
template <typename TObj>
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> TObj*;
/** Allocate a new object with its given class name. */
FORCEINLINE auto NewObject(const LSimpleString& InClassName) -> Private::JObjectBase*;
/** Allocate a new object with its given class name within a given context. */
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LSimpleString& InClassName) -> Private::JObjectBase*;
/** Allocate a new object with its given static class and a given context. */
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> Private::JObjectBase*;

/** Allocate a new object of type TObj. The begin-life method will not be called. */
template <typename TObj>
FORCEINLINE auto NewDeferredObject() -> TObj*;
/** Allocate a new object of type TObj within a given context. The begin-life method will not be called. */
template <typename TObj>
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext) -> TObj*;
/** Allocate a new object with its given class name. The begin-life method will not be called. */
FORCEINLINE auto NewDeferredObject(const LSimpleString& InClassName) -> Private::JObjectBase*;
/** Allocate a new object with its given class name within a given context. The begin-life method will not be called. */
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext, const LSimpleString& InClassName) -> Private::JObjectBase*;
/** Allocate a new object with its given static class and a given context. The begin-life method will not be called. */
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> Private::JObjectBase*;

/** @return The dynamic-casted object if the object is or derives from TObj, else nullptr. */
template <typename TObj>
FORCEINLINE auto DynamicCast(Private::JObjectBase* InObject) -> TObj*;

/** @return The default package referrer. */
template <typename TObj>
FORCEINLINE auto GetDefault() -> const TObj*;

namespace Private
{

/**
 * Global application wide singleton object registry.
 */
ENGINEFRAMEWORK_API extern LObjectRegistry* GObjectRegistry;
/** Referrs to a program global carnifex. This variable is not the owner. */
ENGINEFRAMEWORK_API extern LCarnifex**      GCarnifexReferrer;

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
 * Global accessor struct that is permitted to access private member attributes from all derived classes
 * of JObjectBase.
 */
struct LObjectMiscellaneousAccessor final
{
    LObjectMiscellaneousAccessor()  = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LObjectMiscellaneousAccessor)
    ~LObjectMiscellaneousAccessor() = delete;

    template <typename TObj>
    FORCEINLINE static auto NewObject(Private::LObjectContext* Context) -> TObj*;
    FORCEINLINE static auto NewObject(Private::LObjectContext* Context, const LSimpleString& ClassName) -> JObjectBase*;

    template <typename TObj>
    FORCEINLINE static auto NewDeferredObject(Private::LObjectContext* Context) -> TObj*;
    FORCEINLINE static auto NewDeferredObject(Private::LObjectContext* Context, const LSimpleString& ClassName) -> JObjectBase*;

    ENGINEFRAMEWORK_API static auto NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;
    ENGINEFRAMEWORK_API static auto NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;

    ENGINEFRAMEWORK_API static auto DynamicCast(const JObjectBase* InObject, const LObjectClass* InTargetClass) -> bool;
};

/**
 * Temporal private object that holds an already registered package that requires additional tasks to be done after
 * all packages have been registered.
 */
struct LDeferredRegistryPackage final
{
    /** Full namespaced name of the target superclass that has to be resolved at a later time. */
    LSimpleString   SuperName;
    /** The target child that is missing its parent. */
    LObjectClass*   StaticClass;
};

/**
 * A package that holds a generic derived JObjectBase object.
 * This object is known to the engine as it has been registered and initialized.
 */
struct LRegistryPackage final
{
    /** Full namespaced name of the target class. */
    LSimpleString SpacedClassName;
    /** Pointer to the static class object of the target class. */
    LObjectClass* StaticClass;
};

/** Singleton registry that holds all content, that is considered default, to an object. */
class LObjectRegistry final
{
    friend LRegistrationCallbackHelper;

public:

    LObjectRegistry()  = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LObjectRegistry)
    ~LObjectRegistry() = default;

    /**
     * Registers all pending packages that are waiting for registration.
     * Loads them into memory and initializes the default package referrer for them.
     */
    ENGINEFRAMEWORK_API void LoadPendingPackages(void);
    /**
     * Validates all loaded packages by checking for name conflicts and for the existence
     * of a content default referrer.
     */
    ENGINEFRAMEWORK_API void ValidateLoadedPackages(void);

    ENGINEFRAMEWORK_API auto DoesPackageWithNameExist(const LSimpleString& SpacedClassName) const -> bool;
    ENGINEFRAMEWORK_API auto GetPackageByName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPackageByName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;

    ENGINEFRAMEWORK_API auto GetPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;

    ENGINEFRAMEWORK_API auto GetPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;
    ENGINEFRAMEWORK_API auto GetPanickedPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;

    FORCEINLINE         auto GetRegisteredObjects() -> TdhArray<LRegistryPackage>& { return this->RegisteredObjects; }
    /** Gets all registered static class that inherit in any way from InStaticClass. */
    ENGINEFRAMEWORK_API auto GetRegisteredObjectsOfClass(const LObjectClass* InStaticClass, TdhArray<const LObjectClass*>& OutArray) const -> void;

private:

    TdhArray<LDeferredRegistryPackage> DeferredPackages;
    TdhArray<LRegistryPackage>         RegisteredObjects;
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
    LRegistrationCallbackHelper()  = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LRegistrationCallbackHelper)
    ~LRegistrationCallbackHelper() = delete;

    /**
     * Registers static class information that is required for the object to be registered.
     *
     * @tparam TObj       The object type that is being registered.
     * @param StaticClass The static class that was assigned to TObj.
     * @param Flags       The flags that describe class-specific behavior.
     * @param Parent      The namespaced name of the parent class.
     */
    template <typename TObj = JObjectBase>
    static void DoRegisterContentsForClass(
        LObjectClass*           StaticClass,
        const EClassFlags::Type Flags,
        LSimpleString&&         Parent
    )
    {
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");

        jassert( StaticClass )
        jassert( StaticClass->DefaultPackageReferrer )

        TObj::StaticClassReferrer  = StaticClass;
        StaticClass->TotalByteSize = sizeof(TObj);
        StaticClass->Flags         = Flags;

        check( TObj::StaticClass() )

        GObjectRegistry->DeferredPackages.Emplace(
            std::move(Parent),
            const_cast<LObjectClass*>(TObj::StaticClass())
        );

        return;
    }
};

} /* ~Namespace Private */

template <typename TObj>
TObj* NewObject()
{
    return NewObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
TObj* NewObject(Private::LObjectContext* InContext)
{
    return Private::LObjectMiscellaneousAccessor::NewObject<TObj>(InContext);
}

template <typename TObj>
TObj* NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    return reinterpret_cast<TObj*>(Private::LObjectMiscellaneousAccessor::NewObject(InContext, InStaticClass));
}

Private::JObjectBase* NewObject(const LSimpleString& InClassName)
{
    return NewObject(GOmniVitaContext, InClassName);
}

Private::JObjectBase* NewObject(Private::LObjectContext* InContext, const LSimpleString& InClassName)
{
    return Private::LObjectMiscellaneousAccessor::NewObject(InContext, InClassName);
}

Private::JObjectBase* NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    return Private::LObjectMiscellaneousAccessor::NewObject(InContext, InStaticClass);
}

template <typename TObj>
TObj* NewDeferredObject()
{
    return NewDeferredObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
TObj* NewDeferredObject(Private::LObjectContext* InContext)
{
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject<TObj>(InContext);
}

Private::JObjectBase* NewDeferredObject(const LSimpleString& InClassName)
{
    return NewDeferredObject(GOmniVitaContext, InClassName);
}

Private::JObjectBase* NewDeferredObject(Private::LObjectContext* InContext, const LSimpleString& InClassName)
{
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InClassName);
}

Private::JObjectBase* NewDeferredObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InStaticClass);
}

template <typename TObj>
TObj* Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* Context)
{
    return reinterpret_cast<TObj*>(LObjectMiscellaneousAccessor::NewObject(Context, TObj::StaticClass()));
}

Private::JObjectBase* Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* Context, const LSimpleString& ClassName)
{
    return LObjectMiscellaneousAccessor::NewObject(Context, GObjectRegistry->GetPanickedPackageByName(ClassName)->StaticClass);
}

template <typename TObj>
TObj* DynamicCast(Private::JObjectBase* InObject)
{
    if (Private::LObjectMiscellaneousAccessor::DynamicCast(InObject, TObj::StaticClass()))
    {
        return reinterpret_cast<TObj*>(InObject);
    }

    return nullptr;
}

template <typename TObj>
const TObj* GetDefault()
{
    return reinterpret_cast<const TObj*>(TObj::StaticClass()->GetDefaultPackageReferrer());
}

template <typename TObj>
TObj* Private::LObjectMiscellaneousAccessor::NewDeferredObject(Private::LObjectContext* Context)
{
    return reinterpret_cast<TObj*>(LObjectMiscellaneousAccessor::NewDeferredObject(Context, TObj::StaticClass()));
}

Private::JObjectBase* Private::LObjectMiscellaneousAccessor::NewDeferredObject(Private::LObjectContext* Context, const LSimpleString& ClassName)
{
    return LObjectMiscellaneousAccessor::NewDeferredObject(Context, GObjectRegistry->GetPanickedPackageByName(ClassName)->StaticClass);
}

} /* ~Namespace Jafg */
