// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/ObjectBaseTypes.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/ObjectMacros.h"
#include "Serialization/SerializationCore.h"
#include "ObjectBase.generated.h"

namespace Jafg
{

class NextIsObjectBaseClass;

//#
//# Helper struct to initialize the default object referrers.
//#
struct LObjectInitializer final
{
    LObjectInitializer() = delete;
    FORCEINLINE explicit LObjectInitializer(::Jafg::LObjectContext* InOuter) : Outer(InOuter)
    {
        checkSlow( this->Outer )
        return;
    }
    FORCEINLINE LObjectInitializer(const LObjectInitializer&)            = default;
    FORCEINLINE LObjectInitializer(LObjectInitializer&&)                 = default;
    FORCEINLINE LObjectInitializer& operator=(const LObjectInitializer&) = default;
    FORCEINLINE LObjectInitializer& operator=(LObjectInitializer&&)      = default;
    ~LObjectInitializer() = default;

    //#
    //# Outer for a j class inside a module - this outer represents the lifetime of the package referrer
    //# inside a module and is not used as a (default) outer for clients of this class.
    //# @remarks Although a client should generally not life if its package referrer died.
    //#
    LObjectContext* Outer = nullptr;
};

FORCEINLINE LObjectInitializer GetDefaultObjectInitializer()
{
    return LObjectInitializer(GOmniVitaContext);
}

MAKE_DELEGATE_SIGNATURE(LSetClassField, void, const LString& InValue)
MAKE_DELEGATE_SIGNATURE(LGetClassField, LString)
MAKE_DELEGATE_SIGNATURE(LCustomMallocClassField, void)

struct LClassField
{
    FORCEINLINE LClassField(const LStringView InIdentifier, LSetClassField&& InSet, LGetClassField&& InGet, LCustomMallocClassField&& InMalloc)
        : Identifier(InIdentifier), Set(std::move(InSet)), Get(std::move(InGet)), Malloc(std::move(InMalloc)) { }
    PROHIBIT_COPY(LClassField)
    DEFAULT_MOVE(LClassField)

    LStringView Identifier;
    LSetClassField Set;
    LGetClassField Get;
    LCustomMallocClassField Malloc;
};

//#
//# The base class for all objects that share a lifetime among its owner and that are detected automatically
//# by the jafg build tool to allow for dynamic casting, network replication, etc.
//# This class defines the bare minimum for an object to be a jafg object. If you want to make use of the utility
//# functions that are provided by the jafg framework, use JObject.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsObjectBaseClass")
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JObjectBase
{
    friend LCarnifex;
    friend LObjectContext;

    //# The jafg v table class of this object.
    LObjectClass* VClass = nullptr;

    GENERATED_CLASS_BODY()

protected:

    explicit JObjectBase(const LObjectInitializer& ObjectInitializer);
    virtual ~JObjectBase();

public:

    FORCEINLINE auto GetVTable()        const -> const LObjectClass*  { return this->VClass; }
    FORCEINLINE auto GetMutableVTable() const ->       LObjectClass*  { return this->VClass; }
    FORCEINLINE auto GetVTableSlow()        const -> const LObjectClass*  { return this->VClass ? this->VClass : Private::GObjectRegistry->GetPanickedPackageByContentDefault(this)->StaticClass; }
    FORCEINLINE auto GetMutableVTableSlow() const ->       LObjectClass*  { return this->VClass ? this->VClass : Private::GObjectRegistry->GetPanickedPackageByContentDefault(this)->StaticClass; }
    FORCEINLINE auto IsDefault()       const -> bool  { return this->VClass == nullptr; }
    FORCEINLINE auto GetFullName()     const -> const LSimpleString& { return this->VClass->GetSpacedClassName(); }
    FORCEINLINE auto GetName()         const ->       LName          { return this->VClass->GetName(); }
    FORCEINLINE auto GetFullNameSlow() const -> const LSimpleString& { return this->GetVTableSlow()->GetSpacedClassName(); }
    FORCEINLINE auto GetNameSlow()     const ->       LName          { return this->GetVTableSlow()->GetName(); }

    //#
    //# Gets the context that this object lives in and shares its lifetime with it.
    //# Lifetimes can be abridged by calling either #MarkAsGarbage or #KillYourSelfNow.
    //#
    FORCEINLINE auto GetOuter() const -> LObjectContext* { return this->Outer; }
    FORCEINLINE bool IsOuterValid() const { return this->Outer != nullptr; }

    //#
    //# The first thing that is being called after this object is being created.
    //# Use it as a deferred constructor that needs runtime information not available at module static storage
    //# initialization time.
    //#
    virtual void BeginLife()
    {
#if DO_DOUBLE_CHECK_LIFETIMES
        jassert( this->bHasBegunLife == false )
        this->bHasBegunLife = true;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
        //#
        //# Only the default class referrer may contain fields.
        //#
        check( this->ClassFields.IsData() == false )
    }
#if DO_DOUBLE_CHECK_LIFETIMES
    FORCEINLINE bool HasBegunLife() const { return this->bHasBegunLife; }
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    //#
    //# Marks this object instance as garbage, and it will be killed at the end of this or the next tick depending
    //# on call time inside said tick.
    //#
    void MarkAsGarbage();
    //# Whether this object is marked as garbage and will be killed very soon.
    FORCEINLINE bool IsGarbage() const { return this->bGarbage; }

    //#
    //# Will not wait for the global feared engine butcher to massacre all garbage children of this class but will tell
    //# them to kill themselves now. This might have minimal runtime performance issues when called in large quantities
    //# as we cannot use the spare time between ticks, if enforcing it was enabled by the user, for this task.
    //#
    void KillYourSelfNow(const bool bMayBeGarbage = false);

    //#
    //# Called transitively either by the butcher or #KillYourSelfNow at the last moment of this object lifetime.
    //# The destructor will still be called afterward, but this should be the destructor for the common people.
    //#
    virtual void EndLife() { }

    FORCEINLINE auto GetClassFields() const -> const TdhArray<LClassField>& { return this->ClassFields; }
    FORCEINLINE auto GetMutableClassFieldsDangerous() -> TdhArray<LClassField>& { return this->ClassFields; }

protected:

    //# Delegate called when this object was marked as garbage.
    virtual void OnGarbage() { }

private:

    void MarkAsGarbage(const bool bAddToCarnifex);

    void OnDefaultGarbage();

    LObjectContext* Outer = nullptr;
    bool bGarbage = false;
#if DO_DOUBLE_CHECK_LIFETIMES
    bool bHasBegunLife = false;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    CLASS_FIELD(DefaultOnly)
    TdhArray<LClassField> ClassFields;
};

} /* ~Namespace Jafg */

#include "Engine/SubclassOf.h"
#include "User/Preferences/PreferenceTypes.h"

namespace Jafg
{

template <typename TObj>
FORCEINLINE void Deserialize(Jafg::TSubclassOf<TObj>* Destination, const Jafg::LString& InValue)
{
    checkSlow( Destination )
    const LObjectClass* X = Private::GObjectRegistry->GetPanickedPackageByName(InValue.ToPtr())->StaticClass;
    *Destination = X;
}

} /* ~Namespace Jafg */

template <typename T>
struct std::formatter<::Jafg::TSubclassOf<T>> : std::formatter<const char*>
{
    FORCEINLINE auto format
    (
        const ::Jafg::TSubclassOf<T>& InClass,
        ::std::format_context&        InContext
    ) const -> ::std::format_context::iterator
    {
        if (InClass)
        {
            return std::formatter<const char*>::format(InClass->GetSpacedClassName().ToC(), InContext);
        }
        return ::std::formatter<const char*>::format("", InContext);
    }
};
