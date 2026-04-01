// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"

namespace Jafg
{

//# Finalize a deferred object that has been created with #NewDeferredObject.
template<typename TCxxObject> requires std::is_base_of_v<JCxxClass, TCxxObject>
FORCEINLINE void MakeCxxObjectFinal(TCxxObject& O) { O.BeginLife(); }

namespace Detail
{
struct StaticClassFn
{
    LCxxClass& operator()(LStringView Name) const
    {
        return GetGlobalCxxRecordRegistry().GetClassByNameAsserted(Name)->StaticClass;
    }
};
} /* ~Namespace Detail */
inline constexpr Detail::StaticClassFn StaticClass{};

//#
//# @return The dynamic-casted object if the object is or derives from TCxxClass, else nullptr.
//# @remark If it is known at compile time with certainty that the object is of the target type, use #StaticCast
//#         as that function does not add any runtime overhead in release builds.
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* DynamicCast(JCxxClass* Obj);
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* DynamicCast(JCxxClass const* Obj);

//#
//# Only checks if the object can be casted if JAFG_DO_CHECKS is true. If the object fails to cast to the
//# targeted type, the application will panic. If JAFG_DO_CHECKS is false, it will assume that the object is
//# of the target type and will do an unsafe cast.
//# Use this only in places where you would also use static_cast.
//#
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCast(JCxxClass* Obj) noexcept;
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCast(JCxxClass const* Obj) noexcept;

//# Same as #StaticCast, but it will not allow nullptr to be returned if #DO_CHECKS is true.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCastChecked(JCxxClass* Obj) noexcept;
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCastChecked(JCxxClass const* Obj) noexcept;
//# Same as #StaticCast, but it will not allow nullptr to be returned.
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCastAsserted(JCxxClass* Obj) noexcept;
template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCastAsserted(JCxxClass const* Obj) noexcept;

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
NODISCARD ENGINE_API bool IsValidFast(LClassOuter const* Outer, JCxxClass const* Obj);

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
NODISCARD ENGINE_API bool IsValidSlow(LClassOuter const* Outer, JCxxClass const* Obj);

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const& GetSingleton() noexcept { return TCxxClass::StaticClass().template GetSingleton<TCxxClass>(); }

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass& GetMutableSingleton() noexcept { return TCxxClass::MutableStaticClass().template GetSingleton<TCxxClass>(); }

ENGINE_API void PullConfigForCxxObject(JCxxClass* Object, LCxxClass* Class);
ENGINE_API void PushConfigFromCxxObject(JCxxClass const& Object, LCxxClass const& Class);

namespace Detail
{

template<typename TCxxClass>
struct TJxxDelete
{
    constexpr TJxxDelete() noexcept = default;
    template<typename TUp, typename = std::enable_if_t<std::is_convertible_v<TUp*, TCxxClass*>>>
    constexpr TJxxDelete(TJxxDelete<TUp> const&) noexcept {}
    void operator()(TCxxClass* Ptr) const
    {
        static_assert(std::is_void_v<TCxxClass> == false, "Can't delete pointer to incomplete type.");
        static_assert(sizeof(TCxxClass) > 0, "Can't delete pointer to incomplete type.");
        check(Ptr)
        Ptr->MarkAsGarbage_v2();

        return;
    }
};

} /* ~Namespace Detail */

//# Unique pointer for jcxx classes.
template<typename TCxxClass, typename Deleter = Detail::TJxxDelete<TCxxClass>>
using TJxxUnique = TUnique<TCxxClass, Deleter>;

///////////////////////////////////////////////////////////////////////////////
// Impl
///////////////////////////////////////////////////////////////////////////////

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* DynamicCast(JCxxClass* Obj)
{
    if (Obj && Obj->GetVirtualTable().DerivesFrom(TCxxClass::StaticClass()))
    {
        return static_cast<TCxxClass*>(Obj);
    }

    return nullptr;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* DynamicCast(JCxxClass const* Obj)
{
    if (Obj && Obj->GetVirtualTable().DerivesFrom(TCxxClass::StaticClass()))
    {
        return static_cast<TCxxClass const*>(Obj);
    }

    return nullptr;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCast(JCxxClass* Obj) noexcept
{
    check(!Obj || Obj->GetVirtualTable().DerivesFrom(TCxxClass::StaticClass()))
    return static_cast<TCxxClass*>(Obj);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCast(JCxxClass const* Obj) noexcept
{
    check(!Obj || Obj->GetVirtualTable().DerivesFrom(TCxxClass::StaticClass()))
    return static_cast<TCxxClass const*>(Obj);
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCastChecked(JCxxClass* Obj) noexcept
{
    auto* Out{StaticCast<TCxxClass>(Obj)};
    check(Out)
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCastChecked(JCxxClass const* Obj) noexcept
{
    auto* Out{StaticCast<TCxxClass>(Obj)};
    check(Out)
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass* StaticCastAsserted(JCxxClass* Obj) noexcept
{
    auto* Out{StaticCast<TCxxClass>(Obj)};
    jassert(Out)
    return Out;
}

template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const* StaticCastAsserted(JCxxClass const* Obj) noexcept
{
    auto* Out{StaticCast<TCxxClass>(Obj)};
    jassert(Out)
    return Out;
}

template<typename TCxxClass /* = JCxxClass */> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass const& LCxxClass::GetSingleton() const noexcept
{
    check(this->IsSingleton())
    return *StaticCastChecked<TCxxClass>(this->Singleton);
}

template<typename TCxxClass /* = JCxxClass */> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass& LCxxClass::GetSingleton() noexcept
{
    check(this->IsSingleton())
    return *StaticCastChecked<TCxxClass>(this->Singleton);
}

} /* ~Namespace Jafg */
