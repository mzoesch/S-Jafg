// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxRecord.h"
#include "Engine/CxxRecordUtility.h"
#include "Engine/CxxClassFlags.h"
#include "Engine/BuildToolMacros.h"
#include "Engine/CxxClassMacros.h"
#include "Engine/CxxClassBaseForward.h"
#include "Engine/CxxRecordTearDownReason.h"
#include "CxxClass.generated.h"

namespace Jafg
{

class LCxxClass;
class NextIsBaseCxxClass;
class LEngine;
class LLocalEgo;
class JCxxClass;
class LCarnifex;
class LCommandLineInterface;

namespace Private
{

class LCxxRecordRegistry;
struct LCxxRecordMiscellaneousAccessor;

//# For internal purposes only. Do not use directly.
template<typename T>
NODISCARD FORCEINLINE T* GetCDRFromCxxClass() noexcept;

} /* ~Namespace Private */

static_assert(Lal::TIsCompleteType_v<NextIsBaseCxxClass> == false);

//#
//# The base class for all objects that share a lifetime among its owner and that are detected automatically
//# by the jafg build tool to allow for dynamic casting, network replication, etc.
//# This class defines the bare minimum for an object to be a jafg object.
//#
//# @note Generally speaking, inheriting from this class directly is not recommended.
//#
PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsBaseCxxClass")
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class JCxxClass
{
    friend LCarnifex;

    LCxxClass* JafgVirtualTable { nullptr };

#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wkeyword-macro"
#endif /* LAL_WITH_CLANG */
#define override
    GENERATED_CLASS_BODY()
#undef override
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

protected:

    //#
    //# The constructor. Only called once. Only called on the default object of a class.
    //# This constructor will always be called even if the object itself is abstract.
    //#
    ENGINE_API  explicit JCxxClass(LCxxObjectInitializer const& CxxObjectInitializer) noexceptcheck;
    FORCEINLINE explicit JCxxClass(JCxxClass const& CDR) noexceptcheck
        : Outer{nullptr} { check( CDR.IsCDR() ) }

public:

    virtual ~JCxxClass() noexceptcheck
    {
        checkCode( JCxxClass::CheckDoubleDestroy(this) )

        //
        // If this check triggers, you might have done one of the following things that are forbidden:
        //   - Manually deleted an object via ~delete or ~delete[].
        //   - Used a smart pointer not from the jafg library, for example, a std::shared_ptr (Which are not
        //     compatible).
        //
        // Create a new object with NewObject<T>. Delete them by either calling #MarkAsGarbage to get them discarded
        // at the next engine butcher cycle or by calling #KillYourSelfNow to get them discarded immediately (comparable
        // with a call to the delete operator).
        //
        check( this->bGarbage )
        check( this->HasEndedLife() )

        checkCode
        (
            if (this->IsCDR())
            {
                check( this->HasBegunLifeDefault() )
                check( this->HasBegunLife() == false )
            }
            else
            {
                check( this->HasBegunLife() )
                check( this->HasBegunLifeDefault() == false )
            }
        )

        return;
    }

    //#
    //# Delegate that is called when the default object of this class is finished loading.
    //# If you need to set variables, use the object constructor to initialize them, but if you need to have access
    //# to the finished loaded default object, use this delegate. E.g., validate config attributes (as they are not
    //# loaded when the constructor is called).
    //#
    virtual void BeginLifeCDR()
    {
        check( this->IsDefault() )
#if DO_DOUBLE_CHECK_LIFETIMES
    jassert( this->bHasBegunLifeDCR == false )
    this->bHasBegunLifeDCR = true;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
    }
#if DO_DOUBLE_CHECK_LIFETIMES
    NODISCARD FORCEINLINE constexpr bool HasBegunLifeDefault() const noexcept { return this->bHasBegunLifeDCR; }
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    //#
    //# The first thing that is being called after this object is being created.
    //# Use it as a deferred constructor that needs runtime information.
    //#
    virtual void BeginLife()
    {
#if DO_DOUBLE_CHECK_LIFETIMES
        jassert( this->bHasBegunLife == false )
        this->bHasBegunLife = true;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
    }
#if DO_DOUBLE_CHECK_LIFETIMES
    NODISCARD FORCEINLINE constexpr bool HasBegunLife() const noexcept { return this->bHasBegunLife; }
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    //#
    //# Called transitively either by the butcher or #KillYourSelfNow at the last moment of this object lifetime.
    //# The destructor will still be called afterward, but this should be the destructor for the common people.
    //#
    virtual void EndLife()
    {
#if DO_DOUBLE_CHECK_LIFETIMES
        jassert( this->bHasExecutedEndLife == false )
        this->bHasExecutedEndLife = true;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
    }
#if DO_DOUBLE_CHECK_LIFETIMES
    NODISCARD FORCEINLINE constexpr bool HasEndedLife() const noexcept { return this->bHasExecutedEndLife; }
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    NODISCARD FORCEINLINE bool   IsVirtualTableValid() const noexcept { return this->JafgVirtualTable != nullptr; }
    FORCEINLINE LCxxClass const* GetVirtualTable() const noexcept { return this->JafgVirtualTable; }
    FORCEINLINE LCxxClass const* GetVirtualTableChecked() const noexcept { check( this->JafgVirtualTable ) return this->JafgVirtualTable; }
    FORCEINLINE LCxxClass      * GetMutableVirtualTable() noexcept { return this->JafgVirtualTable; }
    FORCEINLINE LCxxClass      * GetMutableVirtualTableChecked() noexcept { check( this->JafgVirtualTable ) return this->JafgVirtualTable; }

    NODISCARD FORCEINLINE bool IsCDR() const noexcept { return this->IsDefault(); }
    NODISCARD FORCEINLINE bool IsDefault() const noexcept;

    //#
    //# Marks this object instance as garbage, and it will be killed at the end of this or the next tick depending
    //# on call time inside said tick.
    //#
    ENGINE_API void MarkAsGarbage_v2(ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::Default);
    //# Whether this object is marked as garbage and will be killed very soon. Usually at the very end of a tick.
    NODISCARD FORCEINLINE bool IsGarbage() const noexcept { return this->bGarbage; }
    //#
    //# Will not wait for the global feared engine butcher to massacre all garbage children of this class but will tell
    //# them to kill themselves now. This might have minimal runtime performance issues when called in large quantities
    //# as we cannot use the spare time between ticks, if enforcing it was enabled by the user, for this task.
    //#
    void KillYourSelfNow_v2(ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::Default, bool bMayBeGarbage = false);

    //# Delegate called when this object was marked as garbage. Never called on the default object.
    virtual void OnGarbage(ECxxRecordTearDownReason::Type) { check( this->IsGarbage() && this->IsDefault() == false ) }
    //# Delegate called when this object was marked as garbage. Only called on the default object.
    virtual void OnGarbageDefault(ECxxRecordTearDownReason::Type) { check( this->IsGarbage() && this->IsDefault() ) }

    //#
    //# Gets the context that this object lives in and shares its lifetime with it.
    //# Lifetimes can be abridged by calling either #MarkAsGarbage or #KillYourSelfNow.
    //#
    FORCEINLINE bool IsOuterValid() const noexcept { return this->Outer != nullptr; }
    FORCEINLINE LClassOuter const* GetOuter() const noexcept { return this->Outer; }
    FORCEINLINE LClassOuter      * GetOuter() noexcept { return this->Outer; }

    FORCEINLINE LName          GetName() const noexcept;
    FORCEINLINE LStringView    GetNameAsStringView() const noexcept;
    FORCEINLINE LString const& GetNameAsString() const noexcept;

    FORCEINLINE TArray<LCxxClassField> const& GetFields() const noexcept;
    FORCEINLINE TArray<LCxxClassField>& GetMutableFieldsDangerous() noexcept;

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA() const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA(TObj const** CastedOut) const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA(TObj** CastedOut) noexcept;
    FORCEINLINE bool IsA(LCxxClass const& Class) const noexcept;

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* As() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* As() const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* AsChecked() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* AsChecked() const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* AsAsserted() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* AsAsserted() const noexcept;

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* AsStatic() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* AsStatic() const noexcept;

    ENGINE_API  LEngine* GetEngine() const noexcept;
    FORCEINLINE LEngine* GetEngineChecked() const noexceptcheck { auto* Out{ this->GetEngine() }; check( Out ) return Out; }
    FORCEINLINE LEngine* GetEngineAsserted() const { auto* Out{ this->GetEngine() }; jassert( Out ) return Out; }
    ENGINE_API  LLocalEgo* GetLocalEgo() const;
    FORCEINLINE LLocalEgo* GetLocalEgoChecked() const noexceptcheck { auto* Out{ this->GetLocalEgo() }; check( Out ) return Out; }
    FORCEINLINE LLocalEgo* GetLocalEgoAsserted() const { auto* Out{ this->GetLocalEgo() }; jassert( Out ) return Out; }
    ENGINE_API  LCommandLineInterface* GetCommandLineInterface() const;
    FORCEINLINE LCommandLineInterface* GetCommandLineInterfaceChecked() const noexceptcheck { auto* Out{ this->GetCommandLineInterface() }; check( Out ) return Out; }
    FORCEINLINE LCommandLineInterface* GetCommandLineInterfaceAsserted() const { auto* Out{ this->GetCommandLineInterface() }; jassert( Out ) return Out; }

private:

#if LAL_DO_CHECKS
    ENGINE_API static void CheckDoubleDestroy(void const* Ptr);
#endif /* LAL_DO_CHECKS */

    enum class EMarkAsGarbageBehavior
    {
        Default,
        Ignore,
        DevourNow
    };

    void MarkAsGarbage(EMarkAsGarbageBehavior Behavior, ECxxRecordTearDownReason::Type Reason);
    void OnDefaultGarbageInternal(ECxxRecordTearDownReason::Type Reason);

    bool bGarbage : 1 { false };

#if DO_DOUBLE_CHECK_LIFETIMES
    bool bHasBegunLife : 1 { false };
    bool bHasBegunLifeDCR : 1 { false };
    bool bHasExecutedEndLife : 1 { false };
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    LClassOuter* Outer;
};

} /* ~Namespace Jafg */

#include "Engine/CxxClassStatic.h"

namespace Jafg
{

NODISCARD FORCEINLINE bool JCxxClass::IsDefault() const noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetVirtualTable()->GetCDR() == this;
}

NODISCARD LName JCxxClass::GetName() const noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetVirtualTable()->GetName();
}

NODISCARD LStringView JCxxClass::GetNameAsStringView() const noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetVirtualTable()->GetFullyQualifiedName();
}

NODISCARD LString const& JCxxClass::GetNameAsString() const noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetVirtualTable()->GetFullyQualifiedName();
}

NODISCARD TArray<LCxxClassField> const& JCxxClass::GetFields() const noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetVirtualTable()->GetFields();
}

NODISCARD TArray<LCxxClassField>& JCxxClass::GetMutableFieldsDangerous() noexcept
{
    check( this->IsVirtualTableValid() )
    return this->GetMutableVirtualTable()->GetMutableFieldsDangerous();
}

} /* ~Namespace Jafg */

#include "Engine/ClassOuter.h"
#include "Engine/SubclassOf.h"
#include "User/Preferences/PreferenceTypes.h"
#include "Engine/CxxClassUtlity.h"
#include "Engine/ClassStorage.h"

template <typename T>
struct std::formatter<::Jafg::TSubclassOf<T>> : std::formatter<LStringView>
{
    FORCEINLINE auto format
    (
        const Jafg::TSubclassOf<T>& InClass,
        std::format_context&        InContext
    ) const -> std::format_context::iterator
    {
        if (InClass)
        {
            return std::formatter<LStringView>::format(InClass->GetNameAsStringView(), InContext);
        }

        return ::std::formatter<LStringView>::format({}, InContext);
    }
};

namespace Jafg
{

template<typename T> requires std::is_base_of_v<JCxxClass, T>
NODISCARD FORCEINLINE T const* LCxxClass::GetCDR() const noexcept
{
    return StaticCast<T>(this->GetCDR());
}

template<typename T> requires std::is_base_of_v<JCxxClass, T>
NODISCARD FORCEINLINE T* LCxxClass::GetMutableCDR() noexcept
{
    return StaticCast<T>(this->GetMutableCDR());
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE bool JCxxClass::IsA() const noexcept
{
    return this->GetVirtualTableChecked()->DerivesFrom(*TObj::StaticClass());
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE bool JCxxClass::IsA(TObj const** CastedOut) const noexcept
{
    if (TObj const* Out{ DynamicCast<TObj>(this) })
    {
        if (CastedOut)
        {
            *CastedOut = Out;
        }

        return true;
    }

    return false;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE bool JCxxClass::IsA(TObj** CastedOut) noexcept
{
    if (TObj* Out{ DynamicCast<TObj>(this) })
    {
        if (CastedOut)
        {
            *CastedOut = Out;
        }

        return true;
    }

    return false;
}

FORCEINLINE bool JCxxClass::IsA(LCxxClass const& Class) const noexcept
{
    return this->GetVirtualTableChecked()->DerivesFrom(Class);
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj* JCxxClass::As() noexcept
{
    return DynamicCast<TObj>(this);
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::As() const noexcept
{
    return DynamicCast<TObj>(this);
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj* JCxxClass::AsChecked() noexcept
{
    TObj* Out{ this->As<TObj>() };
    check( Out )
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::AsChecked() const noexcept
{
    TObj const* Out{ this->As<TObj>() };
    check( Out )
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj* JCxxClass::AsAsserted() noexcept
{
    TObj* Out{ this->As<TObj>() };
    jassert( Out )
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::AsAsserted() const noexcept
{
    TObj const* Out{ this->As<TObj>() };
    jassert( Out )
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj* JCxxClass::AsStatic() noexcept
{
    return StaticCast<TObj>(this);
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::AsStatic() const noexcept
{
    return StaticCast<TObj>(this);
}

namespace Private
{

template<typename T>
NODISCARD FORCEINLINE T* GetCDRFromCxxClass() noexcept
{
    return T::MutableStaticClass()->template GetMutableCDR<T>();
}

NODISCARD FORCEINLINE LString const& LRegistryClassPackage::GetFullyQualifiedName() const noexcept
{
    return this->StaticClass.GetFullyQualifiedName();
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetPackageByCDR(void const* CDR) const noexcept
{
    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [CDR](auto const& E)
    {
        if (E->IsClass() == false)
        {
            return false;
        }

        return static_cast<void const*>(E->AsClass().StaticClass.GetCDR()) == CDR;
    }) })
    {
        return Out->get()->IsClass() ? &Out->get()->AsClass() : nullptr;
    }

    return nullptr;
}

FORCEINLINE JCxxClass* LCxxRecordMiscellaneousAccessor::MallocClass(JCxxClass const& Class) { return Class._MallocClone(); }
FORCEINLINE void LCxxRecordMiscellaneousAccessor::ChangeOuter(JCxxClass* Obj, LClassOuter* NewOuter) noexcept
{
    check( Obj && NewOuter )
    check( Obj->Outer == nullptr )
    Obj->Outer = NewOuter;

    return;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg */

namespace Serialization
{

template<typename TClass> NODISCARD FORCEINLINE constexpr LString ToString(Jafg::TSubclassOf<TClass> const& Field) noexcept
{
    if (Field.HasClass())
    {
        return Field->GetFullyQualifiedName();
    }

    return TClass::StaticClass()->GetFullyQualifiedName();
}

template<typename TClass> FORCEINLINE constexpr void FromString(Jafg::TSubclassOf<TClass>* Dst, LString const& Value) noexcept
{
    check( Dst )

    auto* Package{ Jafg::Private::GetGlobalCxxRecordRegistry().GetPackageByNameAsserted(Value) };
    jassert( Package->IsClass() )
    auto const& ClassPackage{ Package->AsClass() };

    Dst->Assign(ClassPackage.StaticClass);

    return;
}

} /* ~Namespace Serialization */

static_assert(Lal::TIsCompleteType_v<Jafg::NextIsBaseCxxClass> == false);
