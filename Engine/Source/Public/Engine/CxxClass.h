// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxRecord.h"
#include "Engine/CxxRecordUtility.h"
#include "Engine/CxxClassFlags.h"
#include "Engine/BuildToolMacros.h"
#include "Engine/CxxClassMacros.h"
#include "Engine/CxxRecordTearDownReason.h"
#include "CxxClass.generated.h"

namespace Jafg
{

class AActor;
class WNode;
class LCxxClass;
class NextIsBaseCxxClass;
class LEngine;
class LLocalEgo;
class JCxxClass;
class LCarnifex;
class LCommandLineInterface;
struct LBeginClassLifeInfo;
struct LEndClassLifeInfo;

namespace Detail
{

class LCxxRecordRegistry;
struct NewStaticCxxFn;

template<typename TOuter, typename TProj = algo::identity>
struct TCxxDynamicInit final
{
    static_assert(std::is_base_of_v<LClassOuter, std::remove_cvref_t<std::invoke_result_t<TProj, TOuter const&>>>);

    typedef TProj Proj;

    //# The outer in which the new object will sit in.
    TOuter& Outer;
    //# Identifier of the object to create if TCxxClass cannot be resolved at compile-time.
    LCxxClass const& Class;
};

template<typename TOuter, typename TCxxClass, typename TProj = algo::identity>
struct TCxxStaticInitBase final
{
    static_assert(std::is_base_of_v<JCxxClass, TCxxClass>);
    static_assert(std::is_base_of_v<LClassOuter, std::remove_cvref_t<std::invoke_result_t<TProj, TOuter const&>>>);

    typedef TProj Proj;

    //# The outer in which the new object will sit in.
    TOuter& Outer;
};

struct LCxxOuterIdentityProj final
{
    NODISCARD inline constexpr decltype(auto) operator()(auto const& Init) const noexcept
    {
        return Init;
    }
};

} /* ~Namespace Detail */

//# Initialization for an object by runtime info. All classes have to fulfill this.
typedef Detail::TCxxDynamicInit<LClassOuter> LCxxDynamicInit;

//# Initialization for an object known at compile time. You may add any number of arguments to this.
template<typename TCxxClass>
using TCxxStaticInit = Detail::TCxxStaticInitBase<LClassOuter, TCxxClass>;

//# Class initializer.
struct LBeginClassLifeInfo final
{
    LCxxClass const& Class;
};

//# Class de-initializer.
struct LEndClassLifeInfo final
{
    LCxxClass const& Class;
    ECxxRecordTearDownReason::Type Reason;
};

typedef TFunction<void(JCxxClass* Object, LStringView Value)> LSetCxxClassField;
typedef TFunction<LString(JCxxClass const& Object)> LGetCxxClassField;

struct LCxxClassField final
{
    LStringView Identifier;
    LSetCxxClassField Set;
    LGetCxxClassField Get;
};

//#
//# The base class for all objects that share a lifetime among its owner and that are detected automatically
//# by the jafg build tool to allow for dynamic casting, network replication, etc.
//# This class defines the bare minimum for an object to be a jafg object.
//#
static_assert(Jafg::TIsCompleteType_v<NextIsBaseCxxClass> == false);
PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsBaseCxxClass")
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API JCxxClass
{
    GENERATED_CLASS_BODY()

public:

    typedef Detail::LCxxOuterIdentityProj LDynamicInitProj;

private:

    friend LCarnifex;
    LCxxClass const& JafgVirtualTable;

protected:

    //#
    //# Jxx object can be created with two different ctors. Both should be declared as protected to avoid
    //# accidentally creation of them:
    //#
    //# 1. Dynamic initialized:
    //#     - All dynamic ctors must only take one argument that is: l-cref to LCxxDynamicInit.
    //# 2. Static initialized:
    //#     - All static ctors must take as their first argument: l-cref to TCxxStaticInit<T> where T is the leaf class.
    //#     - They make take any number of additional arguments after that.
    //#
    //# Static ctors are called if the client knows at compile time which class to instantiate. Comparable to "new Class()".
    //# While dyn ctors are called by the jxx record system of Jafg; this allows for dynamic hot-swapping of newly
    //# created classes at runtime through TSubclassOf<T>, user configs or plugin injections. A client knows only the
    //# interface to the class but does not know the actual underlying class.
    //#
    //# To create a new object see Jafg::NewDeferredObject and Jafg::NewObject below.
    //#
    inline explicit JCxxClass(LCxxDynamicInit const& Init) noexcept;
    template<typename TCxxClass>
    inline explicit JCxxClass(TCxxStaticInit<TCxxClass> const& Init) noexcept;

public:

    virtual ~JCxxClass() noexceptcheck
    {
        //
        // A class must begin their life before getting destroyed.
        //
        check(this->_HasBegunLife())

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
        check(this->bGarbage)

        return;
    }

    //#
    //# Delegate that is guaranteed to be called before the first instantiation of a class object. Usually right after
    //# a plugin has finished loading.
    //#
    static void BeginClassLife(LBeginClassLifeInfo const& Info)
    {
        checkCode(JCxxClass::_check_BeginClassLife(Info))
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
        static std::unordered_set<void const*> Lifetimes;
        jassert(Lifetimes.contains(&Info.Class) == false)
        Lifetimes.emplace(&Info.Class);
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */
    }

    //#
    //# Delegate that is guaranteed to be called while no class objects of this type are allocated. Usually right
    //# before the plugin unloading process or engine termination.
    //#
    static void EndClassLife(LEndClassLifeInfo const& Info)
    {
        checkCode(JCxxClass::_check_EndClassLife(Info))
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
        static std::unordered_set<void const*> Lifetimes;
        jassert(Lifetimes.contains(&Info.Class) == false)
        Lifetimes.emplace(&Info.Class);
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */
    }

    //#
    //# The first thing that is being called after this object is being created.
    //# Config and networked related fields will be initialized.
    //#
    //# Use it as a deferred constructor that needs runtime information.
    //#
    inline virtual void BeginLife();
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
    NODISCARD FORCEINLINE constexpr bool _HasBegunLife() const noexcept { return this->bHasBegunLife; }
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */

    FORCEINLINE LCxxClass const& GetVirtualTable() const noexcept { return this->JafgVirtualTable; }
    FORCEINLINE LCxxClass const* GetVirtualTableAsPointer() const noexcept { return &this->JafgVirtualTable; }

    //#
    //# Marks this object instance as garbage, and it will be killed at the end of this or the next tick depending
    //# on call time inside said tick.
    //#
    ENGINE_API void MarkAsGarbage_v2(ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::Default);
    //# Whether this object is marked as garbage and will be killed very soon. Usually at the very end of a tick.
    NODISCARD FORCEINLINE bool _IsGarbage() const noexcept { return this->bGarbage; }
    //#
    //# Will not wait for the global feared engine butcher to massacre all garbage children of this class but will tell
    //# them to kill themselves now.
    //#
    //# It is generally bad practice to call this randomly. As Jafg usually tries to assure that a handle to any Jafg
    //# class is at least valid for the whole tick the handle was acquired and does not point to dangling memory.
    //#
    //# @see TClassStorage<T>
    //#
    void KillYourSelfNow_v2(ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::Default, bool bMayBeGarbage = false);

    //#
    //# Delegate called when this object was marked as garbage.
    //# Use this for immediate reaction to be killed. Otherwise, use the dctor that will usually be called at the end
    //# of the tick this delegate was called.
    //#
    inline virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason);

    //#
    //# Gets the context that this object lives in and shares its lifetime with it.
    //# Lifetimes can be abridged by calling either #MarkAsGarbage or #KillYourSelfNow.
    //#
    FORCEINLINE LClassOuter      & GetOuter() noexcept { return this->Outer; }
    FORCEINLINE LClassOuter const& GetOuter() const noexcept { return this->Outer; }

    FORCEINLINE LName          GetName() const noexcept;
    FORCEINLINE LStringView    GetNameAsStringView() const noexcept;
    FORCEINLINE LString const& GetNameAsString() const noexcept;

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
    FORCEINLINE TObj& AsStatic() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const& AsStatic() const noexcept;

    ENGINE_API LEngine const& GetEngine() const noexcept;
    ENGINE_API LEngine& GetMutableEngine() noexcept;
    ENGINE_API LLocalEgo const& GetLocalEgo() const noexcept;
    ENGINE_API LLocalEgo& GetMutableLocalEgo() noexcept;
    ENGINE_API LCommandLineInterface const& GetCommandLineInterface() const noexcept;
    ENGINE_API LCommandLineInterface& GetMutableCommandLineInterface() noexcept;

    //# If you want a custom path, the override this and call super with your path.
    ENGINE_API virtual void PullConfig(LPath const& InPath = {}) noexcept;
    ENGINE_API virtual void PushConfig(LPath const& InPath = {}) const noexcept;

private:

    LClassOuter& Outer;

    enum struct EMarkAsGarbageBehavior
    {
        Default,
        Ignore,
        DevourNow
    };

    void MarkAsGarbage(EMarkAsGarbageBehavior Behavior, ECxxRecordTearDownReason::Type Reason);
#if JAFG_DO_CHECKS
    ENGINE_API static void _check_BeginClassLife(LBeginClassLifeInfo const& Info);
    ENGINE_API static void _check_EndClassLife(LEndClassLifeInfo const& Info);
#endif /* JAFG_DO_CHECKS */

    bool bGarbage:1{};
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
    bool bHasBegunLife:1{};
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */
};

} /* ~Namespace Jafg */

#ifndef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_SUBCLASS_DEFINITION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_SUBCLASS_DEFINITION is not defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_SUBCLASS_DEFINITION */
#undef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_SUBCLASS_DEFINITION
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_SUBCLASS_DEFINITION( \
    MyClassName,                                                                \
    MyClassSpaces,                                                              \
    SuperClassName,                                                             \
    ConstructionHelperLine,                                                     \
    ... /*OptionalAPI*/                                                         \
    )                                                                           \
    inline static ::TSubclassOf<MyClassName> StaticSubclass() noexcept          \
    {                                                                           \
        return TSubclassOf<MyClassName>(MyClassName::StaticClass());            \
    }

#include "Engine/CxxClassStatic.h"

namespace Jafg
{

void JCxxClass::BeginLife()
{
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
    jassert(this->bHasBegunLife == false)
    this->bHasBegunLife = true;
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */

    if (this->GetVirtualTable().IsConfig())
    {
        this->PullConfig();
    }

    return;
}

inline void JCxxClass::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    check(this->_IsGarbage())

    if (this->GetVirtualTable().IsConfig())
    {
        this->PushConfig();
    }

    return;
}

NODISCARD LName JCxxClass::GetName() const noexcept
{
    return this->GetVirtualTable().GetName();
}

NODISCARD LStringView JCxxClass::GetNameAsStringView() const noexcept
{
    return this->GetVirtualTable().GetFullyQualifiedName();
}

NODISCARD LString const& JCxxClass::GetNameAsString() const noexcept
{
    return this->GetVirtualTable().GetFullyQualifiedName();
}

} /* ~Namespace Jafg */

#include "Engine/ClassOuter.h"
#include "Engine/SubclassOf.h"
#include "User/Preferences/PreferenceTypes.h"
#include "Engine/CxxClassUtlity.h"
#include "Engine/ClassStorage.h"

template<typename T>
struct std::formatter<TSubclassOf<T>> : std::formatter<LString>
{
    FORCEINLINE std::format_context::iterator format(const TSubclassOf<T>& InClass, std::format_context&  InContext) const
    {
        if (InClass)
        {
            return std::formatter<LString>::format(InClass->GetNameAsString(), InContext);
        }
        return std::formatter<LString>::format(Jafg::SprintF("{}?", T::StaticClass().GetNameAsString()), InContext);
    }
};

namespace Jafg
{

//# Cast the result of a new object to a specific compile-time class.
template<typename TCxxClass> requires TIsCompleteType_v<TCxxClass> && std::is_base_of_v<JCxxClass, TCxxClass>
struct CastTo final{};

namespace Detail
{

template<typename TCxxClass> requires TIsCompleteType_v<TCxxClass> && std::is_base_of_v<JCxxClass, TCxxClass>
struct TNewStaticCxxType final{};

struct NewStaticCxxFn
{
    template<typename TCxxClass, typename... TArgs>
    inline static constexpr bool is_constructible_v{requires(TArgs&&... Args){new TCxxClass{std::forward<TArgs>(Args)...};}};

    template<typename TCxxClass, typename... TArgs> requires std::is_base_of_v<JCxxClass, TCxxClass>
        && NewStaticCxxFn::is_constructible_v<TCxxClass, TArgs&&...>
    TCxxClass* operator()(TNewStaticCxxType<TCxxClass>, TArgs&&... Args) const noexcept(std::is_nothrow_constructible_v<TCxxClass, TArgs...>)
    {
        return new TCxxClass(std::forward<TArgs>(Args)...);
    }
};
inline constexpr NewStaticCxxFn NewStaticCxx{};

template<typename TCxxClass> requires TIsCompleteType_v<TCxxClass> && std::is_base_of_v<JCxxClass, TCxxClass>
struct TDeferredObjectExec
{
    inline constexpr TDeferredObjectExec() noexcept = delete;
    inline constexpr TDeferredObjectExec(TCxxClass& InClass) noexcept : Class(InClass) {}
    inline constexpr TDeferredObjectExec(TDeferredObjectExec&& O) noexcept : Class(O.Class), bReleased{O.bReleased}
    {
        O.bReleased = true;
    }
    template<typename UCxxClass> requires TIsCompleteType_v<UCxxClass> && std::is_base_of_v<JCxxClass, UCxxClass>
        && (std::is_base_of_v<TCxxClass, UCxxClass> || std::is_base_of_v<UCxxClass, TCxxClass>)
    inline constexpr TDeferredObjectExec(TDeferredObjectExec<UCxxClass>&& O) noexcept :
        Class{static_cast<TCxxClass&>(O.Class)}, bReleased{O.bReleased}
    {
        O.bReleased = true;
    }

    inline constexpr TDeferredObjectExec& operator=(TDeferredObjectExec&&) noexcept = delete;
    PROHIBIT_COPY(TDeferredObjectExec)

    inline ~TDeferredObjectExec()
    {
        if (this->bReleased == false)
        {
            MakeCxxObjectFinal(Class);
        }
    }

    inline constexpr TCxxClass* release() noexcept
    {
        check(this->bReleased == false)
        this->bReleased = true;
        return &this->Class;
    }

    inline TCxxClass& operator*() noexcept { return this->Class; }
    inline TCxxClass const& operator*() const noexcept { return this->Class; }
    inline TCxxClass* operator->() noexcept { return &this->Class; }
    inline TCxxClass const* operator->() const noexcept { return &this->Class; }

    inline explicit operator TCxxClass*() noexcept { return &this->Class; }
    inline explicit operator TCxxClass const*() const noexcept { return &this->Class; }

    inline TCxxClass* operator&() noexcept { return &this->Class; }
    inline TCxxClass* operator&() const noexcept { return &this->Class; }

    TCxxClass& Class;
    bool bReleased{};
};

template<typename TDynInit, template<typename> typename TStatInit, template<typename> typename TResult, typename TRootNode, typename... TForbiddenNodes>
struct NewDeferredObjectFn
{
    //# Whether the typename #TCxxClass is allowed to be used as a node in this struct to create a new deferred jxx-object.
    template<typename TCxxClass>
    inline static constexpr bool AllowedTreeNode{CAllowedTreeNode<TCxxClass, TRootNode, TForbiddenNodes...>};

    TResult<TRootNode> operator()(TDynInit const& Init) const
    {
        check(Tasks::IsOnMasterThread())
#if JAFG_DO_CHECKS
        if (Init.Class.IsAbstract())
        {
            panicMsgf("Tried to instantiate abstract class [{}].", Init.Class.GetFullyQualifiedName())
        }
#endif /* JAFG_DO_CHECKS */
        auto* Result{Init.Class.GetMallocCxxFn()({.Outer=std::invoke(typename TDynInit::Proj{}, Init.Outer),.Class=Init.Class})};
        check(Result && Result->_HasBegunLife() == false)
        return TResult<TRootNode>{*static_cast<TRootNode*>(Result)};
    }

    template<typename TCxxClass> requires AllowedTreeNode<TCxxClass>
    TResult<TCxxClass> operator()(CastTo<TCxxClass>, TDynInit const& Init) const
    {
        return TResult<TCxxClass>{std::move((*this)(Init))};
    }

    template<typename TCxxClass, typename... TArgs> requires
           AllowedTreeNode<TCxxClass>
        && NewStaticCxxFn::is_constructible_v<TCxxClass, TStatInit<TCxxClass> const&, TArgs&&...>
    TResult<TCxxClass> operator()(TStatInit<TCxxClass> const& Init, TArgs&&... Args) const
    {
        check(Tasks::IsOnMasterThread())
#if JAFG_DO_CHECKS
        if (TCxxClass::StaticClass().IsAbstract())
        {
            panicMsgf("Tried to instantiate abstract class [{}].", TCxxClass::StaticClass().GetFullyQualifiedName())
        }
#endif /* JAFG_DO_CHECKS */
        auto* Result{NewStaticCxx(TNewStaticCxxType<TCxxClass>{}, Init, std::forward<TArgs>(Args)...)};
        check(Result && &Result->GetVirtualTable() == &TCxxClass::StaticClass() && Result->_HasBegunLife() == false)
        return TResult<TCxxClass>{*Result};
    }
};

} /* ~Namespace Detail */

//#
//# Create a new deferred jxx-object; the #BeginLife method will not be called.
//# Use this function and not new/delete, etc.
//# @note Jxx-objects may not be allocated on the stack.
//#
inline constexpr Detail::NewDeferredObjectFn<LCxxDynamicInit, TCxxStaticInit, Detail::TDeferredObjectExec, JCxxClass, AActor, WNode> NewDeferredObject{};

namespace Detail
{

template<typename TDeferrer, typename TDynInit, template<typename> typename TStatInit, typename TRootNode, typename... TForbiddenNodes>
struct NewObjectFn
{
    //# Whether the typename #TCxxClass is allowed to be used as a node in this struct to create a new jxx-object.
    template<typename TCxxClass>
    inline static constexpr bool AllowedTreeNode{CAllowedTreeNode<TCxxClass, TRootNode, TForbiddenNodes...>};

    TRootNode* operator()(TDynInit const& Init) const
    {
         return &this->Deferrer(Init);
    }

    template<typename TCxxClass> requires AllowedTreeNode<TCxxClass>
    TCxxClass* operator()(CastTo<TCxxClass>, TDynInit const& Init) const
    {
        return StaticCastChecked<TCxxClass>((*this)(Init));
    }

    template<typename TCxxClass, typename... TArgs> requires
           AllowedTreeNode<TCxxClass>
        && NewStaticCxxFn::is_constructible_v<TCxxClass, TStatInit<TCxxClass> const&, TArgs&&...>
    TCxxClass* operator()(TStatInit<TCxxClass> const& Init, TArgs&&... Args) const
    {
        return &this->Deferrer(Init, std::forward<TArgs>(Args)...);
    }

    TDeferrer& Deferrer;
};

} /* ~Namespace Detail */

//#
//# Create a new jxx-object.
//# Use this function and not new/delete, etc.
//# @note Jxx-objects may not be allocated on the stack.
//#
inline constexpr Detail::NewObjectFn<decltype(NewDeferredObject), LCxxDynamicInit, TCxxStaticInit, JCxxClass, AActor, WNode> NewObject{NewDeferredObject};

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE bool JCxxClass::IsA() const noexcept
{
    return this->GetVirtualTable().DerivesFrom(TObj::StaticClass());
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE bool JCxxClass::IsA(TObj const** CastedOut) const noexcept
{
    if (TObj const* Out{DynamicCast<TObj>(this)})
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
    if (TObj* Out{DynamicCast<TObj>(this)})
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
    return this->GetVirtualTable().DerivesFrom(Class);
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
    TObj* Out{this->As<TObj>()};
    check(Out)
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::AsChecked() const noexcept
{
    TObj const* Out{this->As<TObj>()};
    check(Out)
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj* JCxxClass::AsAsserted() noexcept
{
    TObj* Out{this->As<TObj>()};
    jassert(Out)
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const* JCxxClass::AsAsserted() const noexcept
{
    TObj const* Out{this->As<TObj>()};
    jassert(Out)
    return Out;
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj& JCxxClass::AsStatic() noexcept
{
    check(this)
    return *StaticCast<TObj>(this);
}

template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
FORCEINLINE TObj const& JCxxClass::AsStatic() const noexcept
{
    check(this)
    return *StaticCast<TObj>(this);
}

} /* ~Namespace Jafg */

template<typename TCxxClass, typename TArchive> requires std::is_base_of_v<Jafg::JCxxClass, TCxxClass>
    && Serde::IsTextOArchive_v<TArchive>
struct Serde::TSerializer<TSubclassOf<TCxxClass>, TArchive>
{
    void operator()(TArchive& Ar, TSubclassOf<TCxxClass> const& Field) const
    {
        if (Field.HasClass())
        {
            Ar.Stream << Field->GetFullyQualifiedName();
        }
        else
        {
            Ar.Stream << TCxxClass::StaticClass().GetFullyQualifiedName();
        }
    }
};
template<typename TCxxClass, typename TArchive> requires std::is_base_of_v<Jafg::JCxxClass, TCxxClass>
    && Serde::IsTextIArchive_v<TArchive>
struct Serde::TDeserializer<TSubclassOf<TCxxClass>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TSubclassOf<TCxxClass>& Field) const
    {
        if (auto* Package{Jafg::Detail::GetGlobalCxxRecordRegistry().GetPackageByName(Ar.Stream)})
        {
            if (Package->IsClass())
            {
                Field.Assign(Package->AsClass().StaticClass);
            }
            else
            {
                return {
                    .Errc = std::errc::invalid_argument,
                    .Error = Jafg::SprintF("Package [{}] is not a class. Found [{}].",
                        Package->GetFullyQualifiedName(), Jafg::LexToString(Package->GetType())
                        )
                    };
            }
        }
        else
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = Jafg::SprintF("No such package: [{}].", Ar.Stream)
                };
        }
        return {};
    }
};
