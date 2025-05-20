// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "ObjectClass.h"
#include "Engine/ObjectContext.h"
#include "Foreign/PluginForward.h"

///////////////////////////////////////////////////////////////////////////////
// Compiler options

//#
//# Whether the C++ compiler should check for pure virtual functions, and if they have been overridden by any derived
//# class. Usually disabled as the program may not run with this option enabled.
//# Abstract classes must still be instantiable to satisfy the object registry that runs at every module startup.
//# Usually, this program panics if it encounters a non-implemented pure virtual method.
//#
#ifndef DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define DO_PURE_VIRTUAL_COMPILER_CHECKS                 0
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

//#
//# Whether to double-check lifetimes of JObjectBase objects. Meaning check if an object has rightfully begun its life,
//# was marked as garbage, ended its life, then was destroyed and freed in the end.
//# These checks add a meaningful non-neglectable overhead to object creation and destruction and should therefore
//# be disabled in shipping builds.
//#
#ifndef DO_DOUBLE_CHECK_LIFETIMES
    #define DO_DOUBLE_CHECK_LIFETIMES                       !IN_SHIPPING
#endif /* !DO_DOUBLE_CHECK_LIFETIMES */

// ~Compiler options
///////////////////////////////////////////////////////////////////////////////

#if DO_PURE_VIRTUAL_COMPILER_CHECKS
    #define PURE_VIRTUAL(...) = 0;
#else /* DO_PURE_VIRTUAL_COMPILER_CHECKS */
    //# Define a RetTy for non-void members if needed.
    #define PURE_VIRTUAL(...) { panic( "Pure virtual function was encountered." ) __VA_ARGS__; }
#endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */

//# A member that was derived but is not callable.
#define NON_CALLABLE_MEMBER(...) { panic( "Non-callable member function was encountered." ) __VA_ARGS__; }

namespace Jafg
{

template <typename InTObj>
class TSubclassOf;
class LCarnifex;
class JObjectBase;
class AActor;
class WNode;
class NextIsObjectBaseClass;
struct LClassField;
struct LObjectInitializer;

namespace Private
{

class LObjectRegistry;
struct TRegistryPackageBase;
struct LRegistrationQueuePackage;
struct LRegistrationCallbackHelper;
struct LObjectMiscellaneousAccessor;

typedef JObjectBase* (*GetContentDefaultFunctor)(void);
typedef void (*OnRegistrationDelegate)(LObjectClass* StaticClass);

} /* ~Namespace Private */

//# A global context that shares the lifetime of the program (not engine!).
ENGINE_API extern LObjectContext* GOmniVitaContext;

//# Allocate a new object of type TObj. */
template <typename TObj>
FORCEINLINE TObj* NewObject();
template <typename TObj>
FORCEINLINE TObj* NewObject(LObjectContext* InContext);
template <typename TObj>
FORCEINLINE TObj* NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass);
FORCEINLINE JObjectBase* NewObject(const LString& InClassName);
FORCEINLINE JObjectBase* NewObject(LObjectContext* InContext, const LString& InClassName);
FORCEINLINE JObjectBase* NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass);

//# Allocate a new object of type TObj. The begin-life method will not be called.
template <typename TObj>
FORCEINLINE TObj* NewDeferredObject();
template <typename TObj>
FORCEINLINE TObj* NewDeferredObject(LObjectContext* InContext);
// Boolean parameters are for internal use only - __DO NOT__ change the default values.
template <typename TObj, bool bAllowActor = /*FALSE REQUIRED*/false, bool bAllowWidget = /*FALSE REQUIRED*/false>
FORCEINLINE TObj* NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass);
FORCEINLINE JObjectBase* NewDeferredObject(const LString& InClassName);
FORCEINLINE JObjectBase* NewDeferredObject(LObjectContext* InContext, const LString& InClassName);
FORCEINLINE JObjectBase* NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass);

//#
//# Call this method to finalize an object that was deferred.
//#
ENGINE_API void MakeDeferredObjectFinal(JObjectBase* InObject);

//#
//# @return The dynamic-casted object if the object is or derives from TObj, else nullptr.
//# @remark This method is fairly slow and should not be used in high proximity in performance-critical control paths.
//#         If it is known at compile time with certainty that the object is of the target type, use CheckedStaticCast
//#         as that function does not add any runtime overhead.
//#
template <typename TObj>
FORCEINLINE TObj* DynamicCast(JObjectBase* InObject);
template <typename TObj>
FORCEINLINE const TObj* DynamicCast(const JObjectBase* InObject);

//#
//# Only checks if the object can be casted if DO_CHECKS is true. If the object fails to cast to the
//# targeted type, the application will panic. If DO_CHECKS is false, it will assume that the object is
//# of the target type and will do an unsafe cast.
//# Only use this method if you are sure that the object is of the targeted type.
//#
//# @tparam bAllowForNullptr Whether to allow for nullptr to be returned if the input object is nullptr.
//# @return The casted object. Will never return nullptr (if bAllowForNullptr is false). But the return value might be
//#         meaningless if DO_CHECKS is false. So you cannot check if this object is valid, e.g., if it is nullptr.
//#
template <typename TObj, typename U, bool bAllowForNullptr = false>
FORCEINLINE TObj* CheckedStaticCast(U* InObject);
template <typename TObj, typename U, bool bAllowForNullptr = false>
FORCEINLINE const TObj* CheckedStaticCast(const U* InObject);

//#
//# This function checks whether the #InPointer is still valid. It assumes that the #InContext is valid.
//#
//# Valid means:
//#   1. The #InPointer is not null.
//#   2. The #InPointer is still allocated, based of the #InContext state.
//#   3. The object at the #InPointer address is not marked as garbage.
//#
//# @remark This function may be used on any thread but of course after the function returned the pointer, it might get
//#         immediately invalid.
//#
ENGINE_API bool IsValidFast(const LObjectContext* InContext, const JObjectBase* InPointer);

//#
//# A more dedicated function, than the #IsValidFast, to check if the #InPointer is still valid.
//# This function is usually only usefully if the caller has a reference to an object that is not same
//# context has its own.
//#
//# Valid means:
//#   1. The #InContextPointer is not null.
//#   2. The #InPointer is not null.
//#   3. The #InContextPointer is still allocated, based of the current engine state.
//#   3. The context at #InContextPointer employees the #InPointer currently.
//#   4. The object at the #InPointer address is not marked as garbage.
//#
//# @remark This function may be used on any thread but of course after the function returned the pointer, it might get
//#         immediately invalid.
//#
ENGINE_API bool IsValidSlow(const LObjectContext* InContextPointer, const JObjectBase* InPointer);

//# @return The default package referrer.
template <typename TObj>
FORCEINLINE const TObj* GetDefault();
//#
//#  @return  The default package referrer that is mutable.
//#  @remarks Mutating any members of the referrer will not affect already instantiated objects but only objects that are
//#           created after the referrer has been mutated.
//#           Generally it is bad habit to mutate the default package referrer, and therefore this method should be used
//#           sparingly - or for "singleton" like objects.
//#
template <typename TObj>
FORCEINLINE TObj* GetMutableDefault();

ENGINE_API void PullConfigFromObject(LObjectClass* InClass);
ENGINE_API void PushConfigFromObject(const LObjectClass* InClass);
template <typename TObj> FORCEINLINE void PushConfigFromObject() { PushConfigFromObject(TObj::StaticClass()); }
template <typename TObj> FORCEINLINE void PushConfigFromObject(const TObj* InObject) { PushConfigFromObject(InObject->GetVTableSlow()); }

//#
//# Make your own custom default malloc member function for members that were marked as DefaultOnly.
//#
template <typename TMemberField>
FORCEINLINE void OnDefaultOnlyMallocMember(TMemberField* MemberField) UNSUPPORTED_TEMPLATED_SPECIALIZATION(TMemberField)
//#
//# Generic but explicit zeroing of a member field that was marked as DefaultOnly.
//#
template <typename TMemberField>
FORCEINLINE void ExplicitCommonZeroOnDefaultOnlyMallocMember(TMemberField* MemberField);

//# Primitive types.
template <> FORCEINLINE void OnDefaultOnlyMallocMember<f32>(f32* MemberField)   { *MemberField = static_cast<f32>(0.0f); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<f64>(f64* MemberField)   { *MemberField = static_cast<f64>(0.0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<i8>(i8* MemberField)     { *MemberField = static_cast<i8>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<i16>(i16* MemberField)   { *MemberField = static_cast<i16>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<i32>(i32* MemberField)   { *MemberField = static_cast<i32>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<i64>(i64* MemberField)   { *MemberField = static_cast<i64>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<u8>(u8* MemberField)     { *MemberField = static_cast<u8>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<u16>(u16* MemberField)   { *MemberField = static_cast<u16>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<u32>(u32* MemberField)   { *MemberField = static_cast<u32>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<u64>(u64* MemberField)   { *MemberField = static_cast<u64>(0); }
template <> FORCEINLINE void OnDefaultOnlyMallocMember<bool>(bool* MemberField) { *MemberField = false; }

template <typename TMemberField>
FORCEINLINE void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField);

template <typename InDerived, typename InTraits, typename InAlloc>
FORCEINLINE void OnDefaultOnlyMallocMember(TStringBase<InDerived, InTraits, InAlloc>* MemberField);

template <typename TObj>
FORCEINLINE void OnDefaultOnlyMallocMember(TSubclassOf<TObj>* MemberField) { *MemberField = nullptr; }

namespace Private
{

//#
//# Global application wide singleton object registry.
//#
ENGINE_API extern LObjectRegistry* GObjectRegistry;
//# Referrs to a program global carnifex. This variable is not the owner.
ENGINE_API extern LCarnifex* GCarnifexReferrer;

ENGINE_API void CreateSingletonObjectRegistry(void);
ENGINE_API void KillSingletonObjectRegistry(void);

//# @return All objects that are waiting for registration.
ENGINE_API TArray<LRegistrationQueuePackage>& GetRegisterObjectQueue();

//# Registers a new object type to the global (in this shared translation unit) registry.
template <typename TObj>
FORCEINLINE void RegisterNewObjectType(
    //# Full namespaced name of the target class.
    LString SpacedClassName,
    //# Delegate that returns a clean default object of the target class.
    GetContentDefaultFunctor GetContentDefaultDelegate,
    //# Delegate that is called when the object has been registered.
    OnRegistrationDelegate Callback
);

struct LRegistrationQueuePackage final
{
    //# Full namespaced name of the target class. /
    LString SpacedClassName;
    //# Delegate that returns a clean default object of the target class.
    GetContentDefaultFunctor GetContentDefault;
    //# Delegate that is called when the object has been registered.
    OnRegistrationDelegate Callback;
};

//#
//# Global accessor struct that is permitted to access private member attributes from all derived classes
//# of JObjectBase.
//#
struct LObjectMiscellaneousAccessor final
{
    LObjectMiscellaneousAccessor()  = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LObjectMiscellaneousAccessor)
    ~LObjectMiscellaneousAccessor() = delete;

    template <typename TObj>
    FORCEINLINE static auto NewObject(LObjectContext* Context) -> TObj*;
    FORCEINLINE static auto NewObject(LObjectContext* Context, const LString& ClassName) -> JObjectBase*;

    template <typename TObj>
    FORCEINLINE static auto NewDeferredObject(LObjectContext* Context) -> TObj*;
    FORCEINLINE static auto NewDeferredObject(LObjectContext* Context, const LString& ClassName) -> JObjectBase*;

    ENGINE_API static auto NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;
    ENGINE_API static auto NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass) -> JObjectBase*;

    ENGINE_API static auto DynamicCast(const JObjectBase* InObject, const LObjectClass* InTargetClass) -> bool;
};

//#
//# Temporal private object that holds an already registered package that requires additional tasks to be done after
//# all packages have been registered.
//#
struct LDeferredRegistryPackage final
{
    //# Full namespaced name of the target superclass that has to be resolved at a later time.
    LString SuperName;
    //# The target child that is missing its parent.
    LObjectClass* StaticClass;
};

//#
//# A package that holds a generic derived JObjectBase object.
//# This object is known to the engine as it has been registered and initialized.
//#
struct LRegistryPackage final
{
    //# Pointer to the static class object of the target class.
    Smart::TUnique<LObjectClass> StaticClass;

    FORCEINLINE const LString& GetSpacedClassName() const
    {
        checkSlow( this->StaticClass )
        return this->StaticClass->GetSpacedClassName();
    }
};

//#
//# Singleton registry that holds all content, that is considered default, to an object.
//#
class LObjectRegistry final
{
    friend LRegistrationCallbackHelper;

public:

    LObjectRegistry()  = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LObjectRegistry)
    ~LObjectRegistry() = default;

    //#
    //# Kills all pending packages that might get loaded by a malformed plugin.
    //#
    ENGINE_API void KillPendingPackages();

    //#
    //# Registers all pending packages that are waiting for registration.
    //# Loads them into memory and initializes the default package referrer for them.
    //#
    ENGINE_API void LoadPendingPackages(const LLoadedPluginHandle InHandle);

    //#
    //# Validates all loaded packages by checking for name conflicts and for the existence
    //# of a content default referrer.
    //#
    ENGINE_API void ValidateLoadedPackages();

    ENGINE_API bool DoesPackageWithNameExist(const LString& SpacedClassName) const;
    ENGINE_API auto GetPackageByName(const LString& SpacedClassName) -> LRegistryPackage*;
    ENGINE_API auto GetPackageByName(const LString& SpacedClassName) const -> const LRegistryPackage*;
    ENGINE_API auto GetPackageByNameWeak(const LString& Name) -> LRegistryPackage*;
    ENGINE_API auto GetPackageByNameWeak(const LString& Name) const -> const LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByName(const LString& SpacedClassName) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByName(const LString& SpacedClassName) const -> const LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByNameWeak(const LString& Name) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByNameWeak(const LString& Name) const -> const LRegistryPackage*;

    ENGINE_API auto GetPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByStaticClass(const void* StaticClass) -> LRegistryPackage*;
    ENGINE_API auto GetPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByStaticClass(const void* StaticClass) const -> const LRegistryPackage*;

    ENGINE_API auto GetPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;
    ENGINE_API auto GetPanickedPackageByContentDefault(const void* ContentDefaultReferrer) -> LRegistryPackage*;

    FORCEINLINE auto GetRegisteredObjects() -> TArray<LRegistryPackage>& { return this->RegisteredObjects; }
    //# Gets all registered static class that inherit in any way from InStaticClass.
    ENGINE_API void GetRegisteredObjectsOfClass(const LObjectClass* InStaticClass, TArray<const LObjectClass*>* OutArray) const;

    ENGINE_API i32 RemovePackagesOf(const LLoadedPluginHandle InHandle);

private:

    TArray<LDeferredRegistryPackage> DeferredPackages;
    TArray<LRegistryPackage>         RegisteredObjects;
};

//# Global static helper struct to allow for private member access through derived classes of JObjectBase.
struct LRegistrationCallbackHelper final
{
    LRegistrationCallbackHelper()  = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LRegistrationCallbackHelper)
    ~LRegistrationCallbackHelper() = delete;

    //#
    //# Registers static class information that is required for the object to be registered.
    //#
    //# @tparam TObj        The object type that is being registered.
    //# @param  StaticClass The static class that was assigned to TObj.
    //# @param  Flags       The flags that describe class-specific behavior.
    //# @param  Parent      The namespaced name of the parent class.
    //#
    template <typename TObj = JObjectBase>
    static void DoRegisterContentsForClass(LObjectClass* StaticClass, const EClassFlags::Type Flags, LString&& Parent);
};

} /* ~Namespace Private */

template <typename TObj>
FORCEINLINE TObj* NewObject()
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return NewObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
FORCEINLINE TObj* NewObject(LObjectContext* InContext)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return Private::LObjectMiscellaneousAccessor::NewObject<TObj>(InContext);
}

template <typename TObj>
FORCEINLINE TObj* NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return reinterpret_cast<TObj*>(Private::LObjectMiscellaneousAccessor::NewObject(InContext, InStaticClass));
}

FORCEINLINE JObjectBase* NewObject(const LString& InClassName)
{
    return NewObject(GOmniVitaContext, InClassName);
}

FORCEINLINE JObjectBase* NewObject(LObjectContext* InContext, const LString& InClassName)
{
    return Private::LObjectMiscellaneousAccessor::NewObject(InContext, InClassName);
}

FORCEINLINE JObjectBase* NewObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    return Private::LObjectMiscellaneousAccessor::NewObject(InContext, InStaticClass);
}

template <typename TObj>
FORCEINLINE TObj* NewDeferredObject()
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return NewDeferredObject<TObj>(GOmniVitaContext);
}

template <typename TObj>
FORCEINLINE TObj* NewDeferredObject(LObjectContext* InContext)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject<TObj>(InContext);
}

template <typename TObj, bool bAllowActor /* = false */, bool bAllowWidget /* = false */>
FORCEINLINE TObj* NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    if constexpr (bAllowActor == false)
    {
        static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    }
    if constexpr (bAllowWidget == false)
    {
        static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    }

    return CheckedStaticCast<TObj>(NewDeferredObject(InContext, InStaticClass));
}

FORCEINLINE JObjectBase* NewDeferredObject(const LString& InClassName)
{
    return NewDeferredObject(GOmniVitaContext, InClassName);
}

FORCEINLINE JObjectBase* NewDeferredObject(LObjectContext* InContext, const LString& InClassName)
{
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InClassName);
}

FORCEINLINE JObjectBase* NewDeferredObject(LObjectContext* InContext, const LObjectClass* InStaticClass)
{
    return Private::LObjectMiscellaneousAccessor::NewDeferredObject(InContext, InStaticClass);
}

template <typename TObj>
FORCEINLINE TObj* Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* Context)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return reinterpret_cast<TObj*>(LObjectMiscellaneousAccessor::NewObject(Context, TObj::StaticClass()));
}

FORCEINLINE JObjectBase* Private::LObjectMiscellaneousAccessor::NewObject(LObjectContext* Context, const LString& ClassName)
{
    return LObjectMiscellaneousAccessor::NewObject(Context, GObjectRegistry->GetPanickedPackageByName(ClassName)->StaticClass);
}

template <typename TObj>
FORCEINLINE TObj* DynamicCast(JObjectBase* InObject)
{
    if (InObject && Private::LObjectMiscellaneousAccessor::DynamicCast(InObject, TObj::StaticClass()))
    {
        return static_cast<TObj*>(InObject);
    }

    return nullptr;
}

template <typename TObj>
FORCEINLINE const TObj* DynamicCast(const JObjectBase* InObject)
{
    return DynamicCast<TObj>(const_cast<JObjectBase*>(InObject));
}

template <typename TObj, typename U, bool bAllowForNullptr /* = false */>
FORCEINLINE TObj* CheckedStaticCast(U* InObject)
{
    static_assert(std::is_base_of_v<JObjectBase, U>);

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
    return static_cast<TObj*>(InObject);
#endif /* !DO_CHECKS */
}

template <typename TObj, typename U, bool bAllowForNullptr /* = false */>
FORCEINLINE const TObj* CheckedStaticCast(const U* InObject)
{
    return CheckedStaticCast<TObj, U, bAllowForNullptr>(const_cast<U*>(InObject));
}

template <typename TObj>
FORCEINLINE const TObj* GetDefault()
{
    return reinterpret_cast<const TObj*>(TObj::StaticClass()->GetDefaultPackageReferrer());
}

template <typename TObj>
FORCEINLINE TObj* GetMutableDefault()
{
    return reinterpret_cast<TObj*>(TObj::StaticClass()->GetMutableDefaultPackageReferrer());
}

template <typename TObj>
FORCEINLINE TObj* Private::LObjectMiscellaneousAccessor::NewDeferredObject(LObjectContext* Context)
{
    static_assert(std::is_base_of_v<AActor, TObj> == false, "AActor now allowed. Use SpawnActor<T> instead.");
    static_assert(std::is_base_of_v<WNode, TObj> == false, "AActor now allowed. Use ConstructWidget<T> instead.");
    return reinterpret_cast<TObj*>(LObjectMiscellaneousAccessor::NewDeferredObject(Context, TObj::StaticClass()));
}

template <typename TObj>
FORCEINLINE void Private::LRegistrationCallbackHelper::DoRegisterContentsForClass(LObjectClass* StaticClass, const EClassFlags::Type Flags, LString&& Parent)
{
    static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");

    jassert( StaticClass )
    jassert( StaticClass->DefaultPackageReferrer )

    TObj::StaticClassReferrer  = StaticClass;
    StaticClass->TotalByteSize = sizeof(TObj);
    StaticClass->Flags         = Flags;

    check( TObj::StaticClass() )

    GObjectRegistry->DeferredPackages.Emplace
    (
        std::move(Parent),
        const_cast<LObjectClass*>(TObj::StaticClass())
    );

    return;
}

FORCEINLINE JObjectBase* Private::LObjectMiscellaneousAccessor::NewDeferredObject(LObjectContext* Context, const LString& ClassName)
{
    return LObjectMiscellaneousAccessor::NewDeferredObject(Context, GObjectRegistry->GetPanickedPackageByName(ClassName)->StaticClass);
}

template <typename TMemberField>
FORCEINLINE void ExplicitCommonZeroOnDefaultOnlyMallocMember(TMemberField* MemberField)
{
    static_assert( sizeof(TMemberField) == 0, "This member field type has not been specialized for default-only malloc." );
    ::memset(MemberField, 0, sizeof(TMemberField));
    return;
}

template <typename TMemberField>
FORCEINLINE void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField)
{
    MemberField->Impl.Data  = nullptr;
    MemberField->Impl.Slack = nullptr;
    MemberField->Impl.End   = nullptr;

    return;
}

template <typename InDerived, typename InTraits, typename InAlloc>
FORCEINLINE void OnDefaultOnlyMallocMember(TStringBase<InDerived, InTraits, InAlloc>* MemberField)
{
    ExplicitCommonZeroOnDefaultOnlyMallocMember(MemberField);
}

template <typename TObj>
FORCEINLINE void Private::RegisterNewObjectType(
    LString SpacedClassName,
    GetContentDefaultFunctor GetContentDefaultDelegate,
    OnRegistrationDelegate Callback
)
{
    static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");

    Private::GetRegisterObjectQueue().Emplace(
        std::forward<LString>(SpacedClassName),
        GetContentDefaultDelegate,
        Callback
    );

    return;
}

} /* ~Namespace Jafg */
