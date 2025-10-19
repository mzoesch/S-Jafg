// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"

namespace Jafg
{

//# Allocate a new j-object.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewObject(LClassOuter* Outer);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewObject(LClassOuter* Outer, LCxxClass const& Class);
FORCEINLINE JCxxClass* NewObject(LClassOuter* Outer, LCxxClass const& Class);
FORCEINLINE JCxxClass* NewObject(LClassOuter* Outer, LString   const& ClassName);

//#
//# Allocate a new j-object. The begin-life method will not be called.
//# You will have to finalize the object later on with #MakeDeferredObjectFinal
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewDeferredObject(LClassOuter* Outer);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewDeferredObject(LClassOuter* Outer, LCxxClass const& Class);
FORCEINLINE JCxxClass* NewDeferredObject(LClassOuter* Outer, LCxxClass const& Class);
FORCEINLINE JCxxClass* NewDeferredObject(LClassOuter* Outer, LString   const& ClassName);

//# Finalize a deferred object that has been created with #NewDeferredObject.
FORCEINLINE void MakeDeferredObjectFinal(JCxxClass* InObject) { check( InObject ) InObject->BeginLife(); }

//#
//# @return The dynamic-casted object if the object is or derives from TCxxClass, else nullptr.
//# @remark If it is known at compile time with certainty that the object is of the target type, use #StaticCast
//#         as that function does not add any runtime overhead in release builds.
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* DynamicCast(JCxxClass* Obj);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* DynamicCast(JCxxClass const* Obj);

//#
//# Only checks if the object can be casted if LAL_DO_CHECKS is true. If the object fails to cast to the
//# targeted type, the application will panic. If LAL_DO_CHECKS is false, it will assume that the object is
//# of the target type and will do an unsafe cast.
//# Use this only in places where you would also use static_cast.
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCast(JCxxClass* Obj);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCast(JCxxClass const* Obj);

//# Same as #StaticCast, but it will not allow nullptr to be returned if #DO_CHECKS is true.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCastChecked(JCxxClass* Obj);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCastChecked(JCxxClass const* Obj);
//# Same as #StaticCast, but it will not allow nullptr to be returned.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCastAsserted(JCxxClass* Obj);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCastAsserted(JCxxClass const* Obj);

//#
//# This function checks whether the #Obj is still valid. It assumes that the #Outer is valid.
//#
//# Valid means:
//#   1. The #Obj is not null.
//#   2. The #Obj is still allocated, based on the #Outer state.
//#   3. The #Obj is not marked as garbage.
//#
//# @remark This function may be used on any thread, but of course, after this function returned the boolean, it
//#         might get immediately invalid.
//#
ENGINE_API bool IsValidFast(LClassOuter const* Outer, JCxxClass const* Obj);

//#
//# A more dedicated function, than the #IsValidFast, to check if the #Obj is still valid.
//# This function is usually only useful if the caller has a reference to an object that is in a different outer, or
//# even outside the Jafg record system (aka. an L class or free function).
//#
//# Valid means:
//#   1. The #Outer is not null.
//#   2. The #Obj is not null.
//#   3. The Engine exists.
//#   4. The Engine is in a defined state (not starting up, not shutting down, etc.).
//#   5. The #Outer is still allocated, based on the current engine state.
//#   6. The #Outer employees the #Obj currently.
//#   7. The #Obj is not marked as garbage.
//#
//# @remark This function may be used on any thread, but of course, after this function returned the boolean, it
//#         might get immediately invalid.
//#
ENGINE_API bool IsValidSlow(LClassOuter const* Outer, JCxxClass const* Obj);

//# @return The CPR.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* GetDefault();
//#
//# @return  The CPR that is mutable.
//# @remarks Mutating any members of the referrer will not affect already instantiated objects but only objects that
//#          are created after the referrer has been mutated.
//#          Generally it is a bad habit to mutate the default package referrer, and therefore this method should be
//#          used sparingly - or for "singleton" like objects.
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* GetMutableDefault();

ENGINE_API void PullConfigForCxxObject(LCxxClass* Obj);
ENGINE_API void PushConfigFromCxxObject(LCxxClass const& Obj);

namespace Private
{

struct LClassOuterMiscellaneousAccessor
{
    UTILITY_STRUCT(LClassOuterMiscellaneousAccessor)

    ENGINE_API static JCxxClass* NewDeferredObjectImpl(LClassOuter* Outer, LCxxClass const& Class);
};

} /* ~Namespace Private */


///////////////////////////////////////////////////////////////////////////////
// Impl
///////////////////////////////////////////////////////////////////////////////

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewObject(LClassOuter* Outer)
{
    return StaticCastChecked<TCxxClass>(NewObject(Outer, *TCxxClass::StaticClass()));
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewObject(LClassOuter* Outer, LCxxClass const& Class)
{
    return StaticCastChecked<TCxxClass>(NewObject(Outer, Class));
}

FORCEINLINE JCxxClass* NewObject(LClassOuter* Outer, LCxxClass const& Class)
{
    auto* Out{ NewDeferredObject(Outer, Class) };
    MakeDeferredObjectFinal(Out);
    return Out;
}

FORCEINLINE JCxxClass* NewObject(LClassOuter* Outer, LString const& ClassName)
{
    auto const* Package{ Private::GetGlobalCxxRecordRegistry().GetClassByNameWeakChecked(ClassName) };
    return NewObject(Outer, Package->StaticClass);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewDeferredObject(LClassOuter* Outer)
{
    check( TCxxClass::StaticClass() )
    return StaticCastChecked<TCxxClass>(NewDeferredObject(Outer, *TCxxClass::StaticClass()));
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* NewDeferredObject(LClassOuter* Outer, LCxxClass const& Class)
{
    return StaticCastChecked<TCxxClass>(NewDeferredObject(Outer, Class));
}

FORCEINLINE JCxxClass* NewDeferredObject(LClassOuter* Outer, LCxxClass const& Class)
{
    return Private::LClassOuterMiscellaneousAccessor::NewDeferredObjectImpl(Outer, Class);
}

FORCEINLINE JCxxClass* NewDeferredObject(LClassOuter* Outer, LString   const& ClassName)
{
    auto const* Package{ Private::GetGlobalCxxRecordRegistry().GetClassByNameWeakChecked(ClassName) };
    return NewDeferredObject(Outer, Package->StaticClass);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* DynamicCast(JCxxClass* Obj)
{
    if (Obj && Obj->GetVirtualTable().DerivesFrom(*TCxxClass::StaticClass()))
    {
        return static_cast<TCxxClass*>(Obj);
    }

    return nullptr;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* DynamicCast(JCxxClass const* Obj)
{
    if (Obj && Obj->GetVirtualTable().DerivesFrom(*TCxxClass::StaticClass()))
    {
        return static_cast<TCxxClass const*>(Obj);
    }

    return nullptr;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCast(JCxxClass* Obj)
{
    check( !Obj || Obj->GetVirtualTable().DerivesFrom(*TCxxClass::StaticClass()) )
    return static_cast<TCxxClass*>(Obj);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCast(JCxxClass const* Obj)
{
    check( !Obj || Obj->GetVirtualTable().DerivesFrom(*TCxxClass::StaticClass()) )
    return static_cast<TCxxClass const*>(Obj);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCastChecked(JCxxClass* Obj)
{
    auto* Out{ StaticCast<TCxxClass>(Obj) };
    check( Out )
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCastChecked(JCxxClass const* Obj)
{
    auto* Out{ StaticCast<TCxxClass>(Obj) };
    check( Out )
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* StaticCastAsserted(JCxxClass* Obj)
{
    auto* Out{ StaticCast<TCxxClass>(Obj) };
    jassert( Out )
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* StaticCastAsserted(JCxxClass const* Obj)
{
    auto* Out{ StaticCast<TCxxClass>(Obj) };
    jassert( Out )
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass const* GetDefault()
{
    return TCxxClass::StaticClass()->template GetCDR<TCxxClass>();
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
FORCEINLINE TCxxClass* GetMutableDefault()
{
    return TCxxClass::MutableStaticClass()->template GetMutableCDR<TCxxClass>();
}

} /* ~Namespace Jafg */

#include "Engine/CdrMemberManipulation.h"
