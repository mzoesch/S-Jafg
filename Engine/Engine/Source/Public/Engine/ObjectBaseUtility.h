// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "ObjectClass.h"
#include "Engine/ObjectContext.h"

///////////////////////////////////////////////////////////////////////////////
// Compiler options

/**
 * Whether the C++ compiler should check for pure virtual functions, and if they have been overriden by any derived
 * class. Usually disabled as the program may not run with this option enabled.
 * Abstract classes must still be instantiable to satisfy the object registry that runs at every module startup.
 * Usually, this program crashes if it encounters a non-implemented pure virtual method.
 */
#ifndef DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define DO_PURE_VIRTUAL_COMPILER_CHECKS                 0
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

/**
 * Whether to double-check lifetimes of JObjectBase objects. Meaning check if an object has rightfully begun its life,
 * was marked as garbage, ended its life, then was destroyed and freed in the end.
 * These checks add a meaningful non-neglectable overhead to object creation and destruction and should therefore
 * be disabled in shipping builds.
 */
#ifndef DO_DOUBLE_CHECK_LIFETIMES
    #define DO_DOUBLE_CHECK_LIFETIMES                       !IN_SHIPPING
#endif /* !DO_DOUBLE_CHECK_LIFETIMES */

// ~Compiler options
///////////////////////////////////////////////////////////////////////////////

#if DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define PURE_VIRTUAL(...) = 0;
#else /* DO_PURE_VIRTUAL_COMPILER_CHECKS */
    /** Define a RetTy for non-void members if needed. */
    #define PURE_VIRTUAL(...) { panic( "Pure virtual function was encountered." ) ##__VA_ARGS__; }
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

/** A member that was derived but is not callable. */
#define NON_CALLABLE_MEMBER(...) { panic( "Non-callable member function was encountered." ) ##__VA_ARGS__; }

namespace Jafg
{

class AActor;
class WWidgetNode;

namespace Private
{

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
ENGINE_API extern Private::LObjectContext* GOmniVitaContext;

/** Allocate a new object of type TObj. */
template <typename TObj>
FORCEINLINE auto NewObject() -> TObj*;
template <typename TObj>
FORCEINLINE auto NewObject(Private::LObjectContext* InContext) -> TObj*;
template <typename TObj>
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> TObj*;
FORCEINLINE auto NewObject(const LSimpleString& InClassName) -> Private::JObjectBase*;
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LSimpleString& InClassName) -> Private::JObjectBase*;
FORCEINLINE auto NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> Private::JObjectBase*;

/** Allocate a new object of type TObj. The begin-life method will not be called. */
template <typename TObj>
FORCEINLINE auto NewDeferredObject() -> TObj*;
template <typename TObj>
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext) -> TObj*;
/* Boolean parameters are for internal use only - __DO NOT__ change the default values. */
template <typename TObj, bool bAllowActor = /*FALSE REQUIRED*/false, bool bAllowWidget = /*FALSE REQUIRED*/false>
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> TObj*;
FORCEINLINE auto NewDeferredObject(const LSimpleString& InClassName) -> Private::JObjectBase*;
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext, const LSimpleString& InClassName) -> Private::JObjectBase*;
FORCEINLINE auto NewDeferredObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass) -> Private::JObjectBase*;

/**
 * Call this method to finalize an object that was deferred.
 */
ENGINE_API void MakeDeferredObjectFinal(Private::JObjectBase* InObject);

/**
 * @return The dynamic-casted object if the object is or derives from TObj, else nullptr.
 * @remark This method is fairly slow and should not be used in high proximity in performance-critical control paths.
 *         If it is known at compile time with certainty that the object is of the target type, use CheckedStaticCast
 *         as that function does not add any runtime overhead.
 */
template <typename TObj>
FORCEINLINE auto DynamicCast(Private::JObjectBase* InObject) -> TObj*;

/**
 * Only checks if the object can be casted if DO_CHECKS is true. If the object fails to cast to the
 * targeted type, the application will panic. If DO_CHECKS is false, it will assume that the object is
 * of the target type and do an unsafe cast.
 * Only use this method if you are sure that the object is of the targeted type.
 *
 * @tparam bAllowForNullptr Whether to allow for nullptr to be returned if the input object is nullptr.
 * @return The casted object. Will never return nullptr (if bAllowForNullptr is false). But the return value might be
 *         meaningless if DO_CHECKS is false. So you cannot check if this object is valid, e.g., if it is nullptr.
 */
template <typename TObj, bool bAllowForNullptr = false>
FORCEINLINE auto CheckedStaticCast(Private::JObjectBase* InObject) -> TObj*;

/** @return The default package referrer. */
template <typename TObj>
FORCEINLINE auto GetDefault() -> const TObj*;
/**
 * @return  The default package referrer that is mutable.
 * @remarks Mutating any members of the referrer will not affect already instantiated objects but only objects that are
 *          created after the referrer has been mutated.
 *          Generally it is bad habit to mutate the default package referrer, and therefore this method should be used
 *          sparingly - or for "singleton" objects.
 */
template <typename TObj>
FORCEINLINE auto GetMutableDefault() -> TObj*;

namespace Private
{

/**
 * Global application wide singleton object registry.
 */
ENGINE_API extern LObjectRegistry* GObjectRegistry;
/** Referrs to a program global carnifex. This variable is not the owner. */
ENGINE_API extern LCarnifex**      GCarnifexReferrer;

ENGINE_API void CreateSingletonObjectRegistry(void);
ENGINE_API void KillSingletonObjectRegistry(void);

/** @return All objects that are waiting for registration. */
ENGINE_API auto GetRegisterObjectQueue() -> TdhArray<LRegistrationQueuePackage>&;

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

    ENGINE_API static auto NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;
    ENGINE_API static auto NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;

    ENGINE_API static auto DynamicCast(const JObjectBase* InObject, const LObjectClass* InTargetClass) -> bool;
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
    ENGINE_API void LoadPendingPackages(void);
    /**
     * Validates all loaded packages by checking for name conflicts and for the existence
     * of a content default referrer.
     */
    ENGINE_API void ValidateLoadedPackages(void);

    ENGINE_API auto DoesPackageWithNameExist(const LSimpleString& SpacedClassName) const -> bool;
    ENGINE_API auto GetPackageByName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINE_API auto GetPackageByName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByName(const LSimpleString& SpacedClassName) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByName(const LSimpleString& SpacedClassName) const -> const LRegistryPackage*;

    ENGINE_API auto GetPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINE_API auto GetPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;

    ENGINE_API auto GetPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;

    FORCEINLINE         auto GetRegisteredObjects() -> TdhArray<LRegistryPackage>& { return this->RegisteredObjects; }
    /** Gets all registered static class that inherit in any way from InStaticClass. */
    ENGINE_API auto GetRegisteredObjectsOfClass(const LObjectClass* InStaticClass, TdhArray<const LObjectClass*>& OutArray) const -> void;

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
     * @tparam TObj        The object type that is being registered.
     * @param  StaticClass The static class that was assigned to TObj.
     * @param  Flags       The flags that describe class-specific behavior.
     * @param  Parent      The namespaced name of the parent class.
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
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return NewObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
TObj* NewObject(Private::LObjectContext* InContext)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return Private::LObjectMiscellaneousAccessor::NewObject<TObj>(InContext);
}

template <typename TObj>
TObj* NewObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
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
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return NewDeferredObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
TObj* NewDeferredObject(Private::LObjectContext* InContext)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject<TObj>(InContext);
}

template <typename TObj, bool bAllowActor /* = false */, bool bAllowWidget /* = false */>
TObj* NewDeferredObject(Private::LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    if constexpr (bAllowActor == false)
    {
        static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    }
    if constexpr (bAllowWidget == false)
    {
        static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    }

    return reinterpret_cast<TObj*>(NewDeferredObject(InContext, InStaticClass));
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
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
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

template <typename TObj, bool bAllowForNullptr /* = false */>
TObj* CheckedStaticCast(Private::JObjectBase* InObject)
{
#if DO_CHECKS
    if constexpr (bAllowForNullptr)
    {
        if (InObject == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        jassert( InObject )
    }

    if (TObj* Out = DynamicCast<TObj>(InObject); Out)
    {
        return Out;
    }

    panicMsgf( "Failed to cast object to [{}].", TObj::StaticClass()->GetSpacedClassName() )

    return nullptr;
#else /* DO_CHECKS */
    return reinterpret_cast<TObj*>(InObject);
#endif /* !DO_CHECKS */
}

template <typename TObj>
const TObj* GetDefault()
{
    return reinterpret_cast<const TObj*>(TObj::StaticClass()->GetDefaultPackageReferrer());
}

template <typename TObj>
TObj* GetMutableDefault()
{
    return reinterpret_cast<TObj*>(TObj::StaticClass()->GetMutableDefaultPackageReferrer());
}

template <typename TObj>
TObj* Private::LObjectMiscellaneousAccessor::NewDeferredObject(Private::LObjectContext* Context)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WWidgetNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return reinterpret_cast<TObj*>(LObjectMiscellaneousAccessor::NewDeferredObject(Context, TObj::StaticClass()));
}

Private::JObjectBase* Private::LObjectMiscellaneousAccessor::NewDeferredObject(Private::LObjectContext* Context, const LSimpleString& ClassName)
{
    return LObjectMiscellaneousAccessor::NewDeferredObject(Context, GObjectRegistry->GetPanickedPackageByName(ClassName)->StaticClass);
}

} /* ~Namespace Jafg */
