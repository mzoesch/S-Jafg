// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/ObjectBaseTypes.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/ObjectMacros.h"
#include "ObjectBase.generated.h"

namespace Jafg
{

class LCarnifex;

/** Helper struct to initialize the default object referrers. */
struct LObjectInitializer final
{
    LObjectInitializer() = delete;
    FORCEINLINE explicit LObjectInitializer(::Jafg::Private::LObjectContext* InOuter) : Outer(InOuter)
    {
        checkSlow( this->Outer )
        return;
    }
    FORCEINLINE LObjectInitializer(const LObjectInitializer&)            = default;
    FORCEINLINE LObjectInitializer(LObjectInitializer&&)                 = default;
    FORCEINLINE LObjectInitializer& operator=(const LObjectInitializer&) = default;
    FORCEINLINE LObjectInitializer& operator=(LObjectInitializer&&)      = default;
    ~LObjectInitializer() = default;

    /**
     * Outer for a j class inside a module - this outer represents the lifetime of the package referrer
     * inside a module and is not used as a (default) outer for clients of this class.
     * @remakrs Although a client should generally not life if its package referrer died.
     */
    Private::LObjectContext* Outer = nullptr;
};

FORCEINLINE auto GetDefaultObjectInitializer() -> LObjectInitializer
{
    return LObjectInitializer(GOmniVitaContext);
}

namespace Private
{

/**
 * The base class for all objects that share a lifetime among its owner and that are detected automatically
 * by the jafg build tool to allow for dynamic casting, network replication, etc.
 */
PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsObjectBaseClass")
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINEFRAMEWORK_API JObjectBase
{
    friend LCarnifex;
    friend LObjectContext;

    /** The jafg v table class of this object. */
    LObjectClass* VClass = nullptr;

    GENERATED_CLASS_BODY()

protected:

    explicit JObjectBase(const LObjectInitializer& ObjectInitializer);
    virtual ~JObjectBase();

public:

    FORCEINLINE auto GetVTable()        const -> const LObjectClass*  { return this->VClass; }
    FORCEINLINE auto GetMutableVTable() const ->       LObjectClass*  { return this->VClass; }
    FORCEINLINE auto GetFullName()      const -> const LSimpleString& { return this->VClass->GetSpacedClassName(); }

    /**
     * Gets the context that this object lives in and shares its lifetime with it.
     * Lifetimes can be abridged by calling either #MarkAsGarbage or #KillYourSelfNow.
     */
    FORCEINLINE virtual auto GetContext() const -> ::Jafg::Private::LObjectContext* { return this->Outer; }
    FORCEINLINE virtual auto HasContext() const -> bool { return this->Outer != nullptr; }

    /**
     * The first thing that is being called after this object is being created.
     * Use it as a deferred constructor that needs runtime information not available at module static storage
     * initialization time.
     */
    virtual void BeginLife()
    {
#if DO_DOUBLE_CHECK_LIFETIMES
        jassert( this->bHasBegunLife == false )
        this->bHasBegunLife = true;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
    }

    /**
     * Marks this object instance as garbage, and it will be killed at the end of this or the next tick depending
     * on call time inside said tick.
     */
    void MarkAsGarbage();
    /** Whether this object is marked as garbage and will be killed very soon. */
    FORCEINLINE bool IsGarbage() const { return this->bGarbage; }

    /**
     * Will not wait for the global feared engine butcher to massacre all garbage children of this class but will tell
     * them to kill themselves now. This might have minimal runtime performance issues when called in large quantities
     * as we cannot use the spare time between ticks, if enforcing it was enabled by the user, for this task.
     */
    void KillYourSelfNow(const bool bMayBeGarbage = false);

    /**
     * Called transitively either by the butcher or #KillYourSelfNow at the last moment of this object lifetime.
     * The destructor will still be called afterward, but this should be the destructor for the common people.
     */
    virtual void EndLife() { }

protected:

    /** Delegate called when this object was marked as garbage. */
    virtual void OnGarbage() { }

private:

    void MarkAsGarbage(const bool bAddToCarnifex);

    bool                             bGarbage = false;
    ::Jafg::Private::LObjectContext* Outer    = nullptr;

#if DO_DOUBLE_CHECK_LIFETIMES
    bool bHasBegunLife = false;
#endif /* DO_DOUBLE_CHECK_LIFETIMES */
};

} /* ~Namespace Private */

} /* ~Namespace Jafg */
