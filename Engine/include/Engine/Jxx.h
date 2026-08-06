// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Engine/EngineCompileTimeConstants.h"
#include "Core/Tag.h"
#include "Core/TagExt.h"
#include "Foreign/PluginForward.h"
#include "Engine/JxxClassMacros.h"
#include "Engine/EngineGetters.h"
#include "Jxx.generated.h"
#include "Jxx.h"

//# Pragmas for the Jafg Build Tool.
#define PRAGMA_FOR_JAFG_BUILD_TOOL(Pragma)

///////////////////////////////////////////////////////////////////////////////
// Compiler options

//#
//# Whether to double-check lifetimes of JCxxClass objects. Meaning check if an object has rightfully begun its life,
//# was marked as garbage, ended its life, then was destroyed and freed in the end.
//# These checks add a meaningful non-neglectable overhead to object creation and destruction and should therefore
//# be disabled in shipping builds.
//#
#ifndef JAFG_DO_DOUBLE_CHECK_LIFETIMES
    #define JAFG_DO_DOUBLE_CHECK_LIFETIMES                              !JAFG_IN_SHIPPING
#endif /* !JAFG_DO_DOUBLE_CHECK_LIFETIMES */

// ~Compiler options
///////////////////////////////////////////////////////////////////////////////

#ifndef NON_CALLABLE_MEMBER
    //# A member that was derived but is not callable.
    #define NON_CALLABLE_MEMBER(...)        { panic("Non-callable member function was encountered.") __VA_ARGS__; }
#endif /* !NON_CALLABLE_MEMBER */

namespace Jafg
{

class LJxxRecord;
class LJxxClass;
class JCxxClass;
class LClassOuter;
class NextIsBaseJxxClass;
class AActor;
class WNode;
class LWorld;
class LViewport;
struct LEditorNodeCreateInfo;

namespace Detail
{

class LCarnifex;
struct LReflectedTag;
struct LRegistryPackage;
struct LRegistryClassPackage;
struct LBeginStylingFnResult;
template<typename TCxxClass>
struct TJxxDelete;

} /* ~Namespace Detail */

} /* ~Namespace Jafg */

//# Unique pointer for jcxx classes.
template<typename TCxxClass, typename Deleter = Jafg::Detail::TJxxDelete<TCxxClass>>
using TJxxUnique = TUnique<TCxxClass, Deleter>;

template<typename TObj> requires algo::is_base_of_weak_v<Jafg::JCxxClass, TObj>
class TSubclassOf;

namespace Jafg
{

namespace Detail
{

//#
//# The tag registry is for tags that are replicated over the network from authorities to the clients.
//# They are ment for a hyperfast lookups, RPCs, etc. You are not meant to interact with them directly.
//#
typedef LMasterThreadTagRegistry<LReflectedTag> LReflectedTagRegistry;
//# Valid at the start of the static storage initialization phase from the runtime until the very end.
ENGINE_API LReflectedTagRegistry& GetJxxTagRegistry() noexcept;

struct LReflectedTag final : public TTag<u16>
{
    typedef TTag<u16> Super;
    using Super::Super;

    FORCEINLINE LString ToString() const noexcept
    {
        return GetJxxTagRegistry().GetReprSafe(*this);
    }
};

struct LNodeFactoryBase
{
    friend LBeginStylingFnResult;

    typedef WNode TFactoredNode;

    constexpr LNodeFactoryBase() noexcept = delete;
    constexpr LNodeFactoryBase(WNode& InNode) noexcept : Node{InNode} {}
    PROHIBIT_COPY(LNodeFactoryBase)
    LNodeFactoryBase(LNodeFactoryBase&& O) noexcept
        : Node{O.Node}
    , Siblings{std::move(O.Siblings)}
#if JAFG_DO_CHECKS
    , _bReleased{O._bReleased}
    , _bDecommissioned{O._bDecommissioned}
#endif /* JAFG_DO_CHECKS */
    {
        check(O.Siblings.empty())
        checkCode(O._bReleased = true)
        checkCode(O._bDecommissioned = true)
    }
    LNodeFactoryBase& operator=(LNodeFactoryBase&& Rhs) noexcept = delete;
    ~LNodeFactoryBase()
    {
        /* A factory does not have to be decommissioned in order to be destroyed. */
        check(this->_bReleased && this->Siblings.empty())
    }

#if JAFG_DO_CHECKS
    FORCEINLINE constexpr void _Release() noexcept { check(this->_bReleased == false) this->_bReleased = true; }
    FORCEINLINE constexpr bool _IsReleased() const noexcept { return this->_bReleased; }
    FORCEINLINE constexpr void _Decommission() noexcept { check(this->_bDecommissioned == false) this->_bDecommissioned = true; }
    FORCEINLINE constexpr bool _IsDecommissioned() const noexcept { return this->_bDecommissioned; }
#endif /* JAFG_DO_CHECKS */

    FORCEINLINE auto& GetRawNode() noexcept { check(this->_IsDecommissioned() == false) return this->Node; }
    FORCEINLINE auto const& GetRawNode() const noexcept { check(this->_IsDecommissioned() == false) return this->Node; }

    FORCEINLINE auto& GetMutableSiblings() noexcept { check(this->_IsDecommissioned() == false) return this->Siblings; }
    FORCEINLINE auto const& GetSiblings() const noexcept { check(this->_IsDecommissioned() == false) return this->Siblings; }

    //# Hatch the node out of their factory to the real life.
    NODISCARD inline decltype(auto) Unique(this auto&& Self) noexcept;
    //# Hatch the leading node out of their factory; end the whole chain; and extent xs with potential siblings of the leader.
    NODISCARD inline decltype(auto) UniqueXs(this auto&& Self, TArray<TJxxUnique<WNode>>* Xs) noexcept;
    template<typename T> requires std::is_base_of_v<WNode, T>
    decltype(auto) SaveTo(this auto&& Self, T** Out) noexcept;
    inline decltype(auto) operator+(this auto&& Self, LNodeFactoryBase&& F) noexcept;

    template<typename TFunc>
    decltype(auto) Delegate(this auto&& Self, TFunc&& Func) noexcept
    requires std::is_invocable_r_v<void, TFunc, std::remove_cvref_t<decltype(Self)>&>
    {
        if constexpr (algo::bool_testable<TFunc>) if (!Func)
        {
            return std::forward<decltype(Self)>(Self);
        }
        Func(static_cast<std::remove_cvref_t<decltype(Self)>&>(Self));
        return std::forward<decltype(Self)>(Self);
    }

private:

    WNode& Node;
    TArray<WNode*> Siblings;
#if JAFG_DO_CHECKS
    bool _bReleased{};
    bool _bDecommissioned{};
#endif /* JAFG_DO_CHECKS */
};

//# A package for a reflected record.
struct LRegistryPackage
{
    enum struct EType
    {
        None,
        Class,
    };

    constexpr LRegistryPackage(EType InType) noexcept : PackageType{InType} { check(this->IsTypeValid()) }
    virtual ~LRegistryPackage() noexcept = default;

    NODISCARD FORCEINLINE virtual LString const& GetFullyQualifiedName() const noexcept = 0;

    FORCEINLINE constexpr auto GetType() const noexcept { return this->PackageType; }
    FORCEINLINE constexpr bool IsTypeValid() const noexcept { return this->PackageType != EType::None; }
    FORCEINLINE constexpr bool IsClass() const noexcept { return this->PackageType == EType::Class; }

    NODISCARD FORCEINLINE LRegistryClassPackage const& AsClass() const noexcept;
    NODISCARD FORCEINLINE LRegistryClassPackage& AsClass() noexcept;

    EType PackageType;
    LLoadedPluginHandle Origin;
};
inline constexpr LStringView LexToString(LRegistryPackage::EType Type) noexcept
{
    switch (Type)
    {
    case LRegistryPackage::EType::Class: { return "Class"; }
    default: { return "<error>"; }
    }
}

//# Holds all registered CxxRecords for all plugins.
class LJxxRecordRegistry final
{
public:

    typedef TArray<TUnique<LRegistryPackage>> LPackages;

    constexpr LJxxRecordRegistry() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LJxxRecordRegistry)
    ~LJxxRecordRegistry() noexcept = default;

    NODISCARD FORCEINLINE constexpr bool AreNewPendingPackagesAllowed() const noexcept { return this->bAllowNewPendingPackages; }

    ENGINE_API void AddNewPendingClassPackage(TUnique<LRegistryClassPackage> Package);

    //#
    //# Registers all pending packages that are waiting for registration.
    //# Loads them into memory and initializes static classes.
    //#
    ENGINE_API void LoadPendingPackages(const LLoadedPluginHandle Handle);

    //# Kills all pending packages that might get loaded by a malformed plugin.
    ENGINE_API void KillPendingPackages();

    //# Whether to allow now new pending packages to be loaded.
    ENGINE_API void SetAllowNewPendingPackages(const bool bAllow) noexcept;

    //# Removes all packages loaded by the provided plugin handle.
    ENGINE_API std::size_t RemovePackagesOf(const LLoadedPluginHandle Handle);

    //# Prematurely free all packages and do not wait for undefined static storage for said task.
    ENGINE_API void TearDown();

    FORCEINLINE LPackages const& GetPendingPackages() const noexcept { return this->PendingPackages; }
    FORCEINLINE LPackages& GetMutablePendingPackages() noexcept { return this->PendingPackages; }

    FORCEINLINE LPackages const& GetRegisteredPackages() const noexcept { return this->RegisteredPackages; }
    FORCEINLINE LPackages& GetMutableRegisteredPackages() noexcept { return this->RegisteredPackages; }

    FORCEINLINE bool DoesPackageWithNameExist(LStringView FullyQualifiedName) const noexcept { return this->GetPackageByName(FullyQualifiedName) != nullptr; }

    FORCEINLINE LRegistryPackage const* GetPackageByName(LStringView FullyQualifiedName) const noexcept
    {
        if (auto const* Out{algo::find_pointer(this->RegisteredPackages, FullyQualifiedName, [](auto const& E){ return E->GetFullyQualifiedName(); })})
        {
            return &**Out;
        }
        return nullptr;
    }
    FORCEINLINE LRegistryPackage const* GetPackageByNameChecked(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; check( Out ) return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameAsserted(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; jassert( Out ) return Out; }

    FORCEINLINE LRegistryClassPackage const* GetClassByName(LStringView FullyQualifiedName) const noexcept
    {
        if (auto const* Out{algo::find_pointer_if(this->RegisteredPackages, [&FullyQualifiedName](auto const& E)
        {
            return E->IsClass() && E->GetFullyQualifiedName() == FullyQualifiedName;
        })})
        {
            return &Out->get()->AsClass();
        }
        return nullptr;
    }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameChecked(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; check( Out ) return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameAsserted(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; jassert( Out ) return Out; }

    // @return All derived classes of TCxxClass inclusive TCxxClass.
    template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TArray<LJxxClass const*> GetClassesByBase() const noexcept { return this->GetClassesByBase(TCxxClass::StaticClass()); }
    NODISCARD FORCEINLINE TArray<LJxxClass const*> GetClassesByBase(LJxxClass const& Base) const noexcept;

    //# For internal purposes only. Do not use.
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeak(LStringView Name) const noexcept
    {
        if (LRegistryPackage const* Out{ this->GetPackageByName(Name) })
        {
            return Out;
        }

        //# TODO: Sketchy solution. Prefix :: avoid any namespace confusions, etc... namespace hints?
        if (auto const* Out{algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E)
        {
            return E->GetFullyQualifiedName().ends_with(Name);
        })})
        {
            return &**Out;
        }

        return nullptr;
    }
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeakChecked(LStringView Name) const noexcept { auto const* Out{ this->_GetPackageByNameWeak(Name) }; check( Out ) return Out; }
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeakAsserted(LStringView Name) const noexcept { auto const* Out{ this->_GetPackageByNameWeak(Name) }; jassert( Out ) return Out; }
    //# For internal purposes only. Do not use.
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeak(LStringView Name) const noexcept
    {
        if (LRegistryClassPackage const* Out{this->GetClassByName(Name)})
        {
            return Out;
        }

        if (auto const* Out{algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E)
        {
            return E->IsClass() && E->GetFullyQualifiedName().ends_with(Name);
        })})
        {
            return &Out->get()->AsClass();
        }

        return nullptr;
    }
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeakChecked(LStringView Name) const noexcept { auto const* Out{ this->_GetClassByNameWeak(Name) }; check( Out ) return Out; }
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeakAsserted(LStringView Name) const noexcept { auto const* Out{ this->_GetClassByNameWeak(Name) }; jassert( Out ) return Out; }

private:

    void RemoveSingletonsOf(std::optional<LLoadedPluginHandle> Handle);
    LClassOuter* SingletonOuter{};

    LPackages PendingPackages;
    LPackages RegisteredPackages;

    //# Default to true for core static storage packages.
    bool bAllowNewPendingPackages{ true };
};
ENGINE_API LJxxRecordRegistry& GetGlobalCxxRecordRegistry() noexcept;

//# A package for a reflected class.
struct LRegistryClassPackage final: LRegistryPackage
{
    LRegistryClassPackage() noexcept = delete;
    LRegistryClassPackage(LJxxClass& InStaticClass) noexcept : LRegistryPackage{EType::Class}, StaticClass{InStaticClass} {}
    NODISCARD FORCEINLINE virtual LString const& GetFullyQualifiedName() const noexcept override;
    LJxxClass& StaticClass;
};

NODISCARD FORCEINLINE LRegistryClassPackage const& LRegistryPackage::AsClass() const noexcept
{
    check(this->IsClass())
    return *static_cast<LRegistryClassPackage const*>(this);
}

NODISCARD FORCEINLINE LRegistryClassPackage& LRegistryPackage::AsClass() noexcept
{
    check(this->IsClass())
    return *static_cast<LRegistryClassPackage*>(this);
}

} /* ~Namespace Detail */

//# Categorical flags for fields of a JCxxClass.
enum struct EJxxFieldBits
{
    Identity = 0 << 0,
    //# This field is serialized.
    Config = 1 << 0,
    //# This field is visible in the editor.
    EditorVisible  = 1 << 1,
    //# This field is visible and editable in the editor. Implies #EditorVisible.
    EditorEditable = 1 << 2,
    //# Skip serialization. #Config and #Transient are mutually exclusive.
    Transient = 1 << 3,
};
ENUM_STRUCT_FLAGS(EJxxFieldBits, EJxxFieldFlags)

namespace JxxFieldBits
{

NODISCARD FORCEINLINE constexpr bool IsSerde(EJxxFieldFlags Flags) noexcept
{
    return !!(Flags & EJxxFieldBits::Config);
}

NODISCARD FORCEINLINE constexpr bool IsEditorVisible(EJxxFieldFlags Flags) noexcept
{
    return (Flags & EJxxFieldBits::EditorVisible) || (Flags & EJxxFieldBits::EditorEditable);
}

NODISCARD FORCEINLINE constexpr bool IsEditorEditable(EJxxFieldFlags Flags) noexcept
{
    return !!(Flags & EJxxFieldBits::EditorEditable);
}

NODISCARD FORCEINLINE constexpr bool IsFastCloneable(EJxxFieldFlags Flags) noexcept
{
    return !(Flags & EJxxFieldBits::Transient);
}

} /* ~Namespace JxxFieldBits */

template<typename... TFlags>
FORCEINLINE constexpr EJxxFieldFlags CombineJxxFieldFlags(TFlags&&... Flags) noexcept
{
    static_assert((std::is_same_v<TFlags, EJxxFieldBits> &&...));
    if constexpr (sizeof...(Flags) > 0)
    {
        return (EJxxFieldFlags{EJxxFieldBits::Identity} | (Flags |...));
    }
    return EJxxFieldBits::Identity;
}

typedef TFunction2<void(JCxxClass* Object, LStringView Value)> LSetCxxClassField;
typedef TFunction2<LString(JCxxClass const& Object)> LGetCxxClassField;
typedef TFunction2<bool(JCxxClass const& Object)> LIsModifiedCxxClassField;
typedef TFunction2<void(JCxxClass const& Origin, JCxxClass* Target)> LFastCloneCxxClassField;
#if JAFG_WITH_EDITOR
    typedef TFunction2<Detail::LNodeFactoryBase(LViewport& Viewport, JCxxClass& Object, TFunction2<void()>& UpdateValue)> LEditorFieldFactory;
#endif /* JAFG_WITH_EDITOR */
//# A reflected field for a class.
struct LJxxClassField final
{
    //# Always valid.
    LStringView Identifier;
    //# Always valid.
    EJxxFieldFlags Flags;
    //# Valid if #JxxFieldBits::IsSerde else nullptr.
    LSetCxxClassField Set;
    //# Valid if #JxxFieldBits::IsSerde else nullptr.
    mutable LGetCxxClassField Get;
    //# Valid if #JxxFieldBits::IsSerde else nullptr.
    mutable LIsModifiedCxxClassField IsModified;
    //# Valid if #JxxFieldBits::IsFastCloneable else nullptr.
    mutable LFastCloneCxxClassField FastClone;
#if JAFG_WITH_EDITOR
    //# Valid if #JxxFieldBits::IsEditorVisible else nullptr.
    mutable LEditorFieldFactory EditorFactory;
#endif /* JAFG_WITH_EDITOR */
};

template<typename T>
struct TEditorNodeCreateInfo final
{
    //# The owning viewport of the new editor node.
    LViewport& Viewport;
    //# The owner that owns the data-field. This field is optional and may be null.
    JCxxClass* Owner{};
    //# The field that is made accessible.
    T& Field;
    //# Optional default value.
    std::optional<T> Default;
    //# Optional update function.
    TFunction2<void()>& UpdateValue;
};

enum struct EJxxRecordTearDownReason
{
    //# Any normal reason.
    Default,
    //# The record is implicitly torn down because its outer is torn down.
    OuterTearDown,
    //# The record is implicitly torn down because its plugin is unloaded.
    PluginUnload,
    //# The record is implicitly torn down because the engine is shutting down.
    EngineShutdown,
};

//# A reflected record.
class LJxxRecord
{
public:

    constexpr LJxxRecord() noexcept = delete;
    constexpr LJxxRecord(LString InFullyQualifiedName) noexcept
        : FullyQualifiedName(std::move(InFullyQualifiedName))
    {
        check(!this->FullyQualifiedName.empty())
        this->Tag = Detail::GetJxxTagRegistry().RegisterOrGet(this->FullyQualifiedName);
        check(this->Tag.IsSet())
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LJxxRecord)
    ~LJxxRecord() noexcept = default;

    FORCEINLINE constexpr Detail::LReflectedTag GetTag() const noexcept { return this->Tag; }
    FORCEINLINE constexpr LString const& GetFullyQualifiedName() const noexcept { return this->FullyQualifiedName; }

    FORCEINLINE constexpr bool IsPluginHandleValid() const noexcept { return this->PluginHandle.IsValid(); }
    FORCEINLINE constexpr LLoadedPluginHandle GetPluginHandle() const noexcept { return this->PluginHandle; }

private:

    Detail::LReflectedTag Tag;
    LString FullyQualifiedName;
    LLoadedPluginHandle PluginHandle;
};

//# High-level categorization of jxx classes.
enum struct EJxxClassBits
{
    //# No flags are set. This is the default value.
    Identity        = 0 << 0,
    //# The class is abstract and can therefore not be instantiated.
    Abstract        = 1 << 0,
    //# Fields with the CLASS_FIELD macro are serialized both ways.
    Config          = 1 << 1,
    //# The class is a singleton and can only be accessed through the #GetSingleton and #GetMutableSingleton functions.
    Singleton       = 1 << 2,
};
ENUM_STRUCT_FLAGS(EJxxClassBits, EJxxClassFlags)

template<typename... TFlags>
FORCEINLINE constexpr EJxxClassFlags CombineJxxClassFlags(TFlags&&... Flags) noexcept
{
    static_assert((std::is_same_v<TFlags, EJxxClassBits> &&...));
    if constexpr (sizeof...(Flags) > 0)
    {
        return (EJxxClassFlags{EJxxClassBits::Identity} | (Flags |...));
    }
    return EJxxClassBits::Identity;
}

//# Class initializer.
struct LBeginClassLifeInfo final
{
    LJxxClass const& Class;
};
//# Class de-initializer.
struct LEndClassLifeInfo final
{
    LJxxClass const& Class;
    EJxxRecordTearDownReason Reason{ EJxxRecordTearDownReason::Default };
};

namespace Detail
{

template<typename TOuter, typename TProj = algo::identity>
struct TCxxDynamicInit final
{
    static_assert(std::is_base_of_v<LClassOuter, std::remove_cvref_t<std::invoke_result_t<TProj, TOuter const&>>>);

    typedef TProj Proj;

    //# The outer in which the new object will sit in.
    TOuter& Outer;
    //# Identifier of the object to create if TCxxClass cannot be resolved at compile-time.
    LJxxClass const& Class;
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

namespace Detail
{

typedef JCxxClass*(*MallocCxxFn)(LCxxDynamicInit const&);
typedef void(*BeginClassLifeFn)(LBeginClassLifeInfo const&);
typedef void(*EndClassLifeFn)(LEndClassLifeInfo const&);

} /* ~Namespace Detail */

//# A reflected class.
class LJxxClass final : public LJxxRecord
{
    friend Detail::LJxxRecordRegistry;

public:

    LJxxClass() = delete;
    LJxxClass(
          LString InFullyQualifiedName
        , Detail::MallocCxxFn InMallocCxx
        , Detail::BeginClassLifeFn InBeginClassLife
        , Detail::EndClassLifeFn InEndClassLife
        , LStringView InParentName
        , EJxxClassFlags InFlags
        , TArray<LJxxClassField>&& InFields
        ) noexcept
        : LJxxRecord{std::move(InFullyQualifiedName)}
        , MallocCxx(InMallocCxx)
        , BeginClassLife(InBeginClassLife)
        , EndClassLife(InEndClassLife)
        , ParentName(std::move(InParentName))
        , Flags(InFlags)
        , Fields(std::move(InFields))
    {
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LJxxClass)
    ~LJxxClass() noexcept { check(!this->Singleton) }

    NODISCARD FORCEINLINE auto const& GetMallocCxxFn() const noexcept { return this->MallocCxx; }
    NODISCARD FORCEINLINE auto const& GetBeginClassLifeFn() const noexcept { return this->BeginClassLife; }
    NODISCARD FORCEINLINE auto const& GetEndClassLifeFn() const noexcept { return this->EndClassLife; }

    NODISCARD FORCEINLINE bool IsValid() const noexcept { return this->Parent != nullptr || this->GetFullyQualifiedName() == "::Jafg::JCxxClass"sv; }

    NODISCARD FORCEINLINE bool IsRoot() const noexcept { return this->Parent == nullptr && this->GetFullyQualifiedName() == "::Jafg::JCxxClass"sv; }
    NODISCARD FORCEINLINE bool IsParentValid() const noexcept { return this->Parent != nullptr; }
    NODISCARD FORCEINLINE auto GetParent()         noexcept -> LJxxClass*                { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetParent()   const noexcept -> LJxxClass const*          { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetParentUnsafe()         noexcept -> LJxxClass*          { return this->Parent; }
    NODISCARD FORCEINLINE auto GetParentUnsafe()   const noexcept -> LJxxClass const*    { return this->Parent; }
    NODISCARD FORCEINLINE auto GetChildren()       noexcept -> TArray<LJxxClass*>&       { return this->Children; }
    NODISCARD FORCEINLINE auto GetChildren() const noexcept -> TArray<LJxxClass*> const& { return this->Children; }

    template<typename T> requires std::is_base_of_v<JCxxClass, T>
    NODISCARD FORCEINLINE bool DerivesFrom() const noexcept { return this->DerivesFrom(T::StaticClass()); }
    NODISCARD ENGINE_API  bool DerivesFrom(LJxxClass const& Parent) const noexcept;

    NODISCARD FORCEINLINE auto GetFlags()       const noexcept { return this->Flags; }
    NODISCARD FORCEINLINE bool HasAnyFlags()    const noexcept { return  this->Flags != EJxxClassBits::Identity;                               }
    NODISCARD FORCEINLINE bool IsAbstract()     const noexcept { return (this->Flags  & EJxxClassBits::Abstract)  != EJxxClassBits::Identity; }
    NODISCARD FORCEINLINE bool IsNotAbstract()  const noexcept { return (this->Flags  & EJxxClassBits::Abstract)  == EJxxClassBits::Identity; }
    NODISCARD FORCEINLINE bool IsConfig()       const noexcept { return (this->Flags  & EJxxClassBits::Config)    != EJxxClassBits::Identity; }
    NODISCARD FORCEINLINE bool IsNotConfig()    const noexcept { return (this->Flags  & EJxxClassBits::Config)    == EJxxClassBits::Identity; }
    NODISCARD FORCEINLINE bool IsSingleton()    const noexcept { return (this->Flags  & EJxxClassBits::Singleton) != EJxxClassBits::Identity; }
    NODISCARD FORCEINLINE bool IsNotSingleton() const noexcept { return (this->Flags  & EJxxClassBits::Singleton) == EJxxClassBits::Identity; }

    //# Only returns the fields that this class declares. This does not include fields that this class inherits.
    NODISCARD FORCEINLINE TArray<LJxxClassField> const& GetFieldsOfThisClassOnly() const noexcept { return this->Fields; }
    NODISCARD FORCEINLINE TArray<LJxxClassField>& GetMutableFieldsOfThisClassOnlyDangerous() noexcept { return this->Fields; }

    //# Returns all fields from top to this class with all fields that this class inherits and declares if any.
    NODISCARD FORCEINLINE auto FieldIter() const noexcept;
    NODISCARD FORCEINLINE auto MutableFieldIter() noexcept;

    template<typename TCxxClass = JCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TCxxClass const& GetSingleton() const noexcept;
    template<typename TCxxClass = JCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TCxxClass& GetSingleton() noexcept;

private:

    Detail::MallocCxxFn MallocCxx{};
    Detail::BeginClassLifeFn BeginClassLife{};
    Detail::EndClassLifeFn EndClassLife{};

    LStringView ParentName;
    LJxxClass* Parent{};
    TArray<LJxxClass*> Children;

    EJxxClassFlags Flags;

    TArray<LJxxClassField> Fields;

    JCxxClass* Singleton{};
};

namespace Detail
{

//# Iterator that traverses from top most root to the leaf over all inherited fields of said leaf.
template<typename T> requires std::is_same_v<std::remove_const_t<T>, LJxxClass>
struct TJxxClassFieldIterator
{
    inline static constexpr bool is_const{std::is_const_v<T>};

    struct Iterator
    {
        friend TJxxClassFieldIterator;

        FORCEINLINE Iterator(TArray<T*> const& Chain, std::size_t ClassIndex, std::size_t FieldIndex) noexcept
            : Chain{Chain}
            , ClassIndex{ClassIndex}
            , FieldIndex{FieldIndex}
        {
        }
        FORCEINLINE Iterator(Iterator const& It) noexcept
            : Chain{It.Chain}
            , ClassIndex{It.ClassIndex}
            , FieldIndex{It.FieldIndex}
        {
        }

        NODISCARD FORCEINLINE auto& operator*() noexcept
        {
            if constexpr (is_const)
            {
                return this->Chain[this->ClassIndex]->GetFieldsOfThisClassOnly()[this->FieldIndex];
            }
            else
            {
                return this->Chain[this->ClassIndex]->GetMutableFieldsOfThisClassOnlyDangerous()[this->FieldIndex];
            }
        }
        NODISCARD FORCEINLINE auto* operator->() noexcept
        {
            if constexpr (is_const)
            {
                return &this->Chain[this->ClassIndex]->GetFieldsOfThisClassOnly()[this->FieldIndex];
            }
            else
            {
                return &this->Chain[this->ClassIndex]->GetMutableFieldsOfThisClassOnlyDangerous()[this->FieldIndex];
            }
        }

        //# Iterator must point to a valid field.
        NODISCARD FORCEINLINE T& GetClass() noexcept
        {
            check(algo::valid_index(this->Chain, this->ClassIndex))
            return *this->Chain[this->ClassIndex];
        }

        FORCEINLINE Iterator& operator++() noexcept
        {
            ++this->FieldIndex;

            while (this->ClassIndex < this->Chain.size())
            {
                if (this->FieldIndex < this->Chain[this->ClassIndex]->GetFieldsOfThisClassOnly().size())
                {
                    break;
                }

                ++this->ClassIndex;
                this->FieldIndex = 0;
                continue;
            }

            return *this;
        }

        NODISCARD FORCEINLINE constexpr bool operator==(Iterator const& Rhs) const noexcept
        {
            check(&this->Chain == &Rhs.Chain)
            return this->ClassIndex == Rhs.ClassIndex && this->FieldIndex == Rhs.FieldIndex;
        }

    private:

        TArray<T*> const& Chain;
        std::size_t ClassIndex{0uz};
        std::size_t FieldIndex{0uz};
    };

    NODISCARD FORCEINLINE TJxxClassFieldIterator(T& Leaf) noexcept
    {
        TArray<T*> Transient;
        for (auto* Parent{&Leaf}; Parent != nullptr; Parent = Parent->GetParentUnsafe())
        {
            Transient.emplace_back(Parent);
        }
        this->Chain.reserve(Transient.size());
        this->Chain.assign(Transient.rbegin(), Transient.rend());
    }

    NODISCARD FORCEINLINE Iterator begin() const noexcept
    {
        Iterator It{this->Chain, 0uz, 0uz};
        while (It.ClassIndex < this->Chain.size() && this->Chain[It.ClassIndex]->GetFieldsOfThisClassOnly().empty())
        {
            ++It.ClassIndex;
        }
        return It;
    }

    NODISCARD FORCEINLINE Iterator end() const noexcept
    {
        return Iterator{this->Chain, this->Chain.size(), 0};
    }

private:

    TArray<T*> Chain;
};

NODISCARD FORCEINLINE TArray<LJxxClass const*> LJxxRecordRegistry::GetClassesByBase(LJxxClass const& Base) const noexcept
{
    TArray<LJxxClass const*> Out;

    for (auto& E : this->RegisteredPackages)
    {
        if (E->IsClass() && E->AsClass().StaticClass.DerivesFrom(Base))
        {
            Out.push_back(&E->AsClass().StaticClass);
        }

        continue;
    }

    return Out;
}

NODISCARD FORCEINLINE LString const& LRegistryClassPackage::GetFullyQualifiedName() const noexcept
{
    return this->StaticClass.GetFullyQualifiedName();
}

} /* ~Namespace Detail */

FORCEINLINE auto LJxxClass::FieldIter() const noexcept
{
    return Detail::TJxxClassFieldIterator{*this};
}

FORCEINLINE auto LJxxClass::MutableFieldIter() noexcept
{
    return Detail::TJxxClassFieldIterator{*this};
}

//# Cast the result of a new object to a specific compile-time class.
template<typename TCxxClass> requires algo::is_base_of_weak_v<JCxxClass, TCxxClass>
struct CastTo final{};

namespace Detail
{

template<typename TCxxClass> requires algo::is_base_of_weak_v<JCxxClass, TCxxClass>
struct TNewStaticCxxType final{};

struct NewStaticCxxFn
{
    template<typename TCxxClass, typename... TArgs>
    static constexpr bool is_constructible_v{requires(TArgs&&... Args){new TCxxClass{std::forward<TArgs>(Args)...};}};

    template<typename TCxxClass, typename... TArgs> requires std::is_base_of_v<JCxxClass, TCxxClass>
        && NewStaticCxxFn::is_constructible_v<TCxxClass, TArgs&&...>
    TCxxClass* operator()(TNewStaticCxxType<TCxxClass>, TArgs&&... Args) const noexcept(std::is_nothrow_constructible_v<TCxxClass, TArgs...>)
    {
        return new TCxxClass(std::forward<TArgs>(Args)...);
    }
};
inline constexpr NewStaticCxxFn NewStaticCxx{};

template<typename TCxxClass> requires algo::is_base_of_weak_v<JCxxClass, TCxxClass>
struct TDeferredObjectExec
{
    constexpr TDeferredObjectExec() noexcept = delete;
    constexpr explicit TDeferredObjectExec(TCxxClass& InClass) noexcept : Class{InClass} { check(!this->Class._HasBegunLife()) }
    constexpr TDeferredObjectExec(TDeferredObjectExec&& O) noexcept : Class{O.Class}, bReleased{std::exchange(O.bReleased, true)} {}
    template<typename UCxxClass> requires std::is_base_of_v<JCxxClass, UCxxClass>
        && (std::is_base_of_v<TCxxClass, UCxxClass> || std::is_base_of_v<UCxxClass, TCxxClass>)
    constexpr TDeferredObjectExec(TDeferredObjectExec<UCxxClass>&& O) noexcept :
        Class{static_cast<TCxxClass&>(O.Class)}, bReleased{std::exchange(O.bReleased, true)} {}

    constexpr TDeferredObjectExec& operator=(TDeferredObjectExec&&) noexcept = delete;
    PROHIBIT_COPY(TDeferredObjectExec)

    ~TDeferredObjectExec()
    {
        if (!this->bReleased)
        {
            this->finalize();
        }
    }

    constexpr TCxxClass* finalize() noexcept
    {
        check(!this->bReleased)
        this->bReleased = true;
        MakeCxxObjectFinal(this->Class);
        return &this->Class;
    }

    constexpr TCxxClass* release() noexcept
    {
        check(this->bReleased == false)
        this->bReleased = true;
        return &this->Class;
    }

    NODISCARD constexpr TCxxClass& get() noexcept { return this->Class; }
    NODISCARD constexpr TCxxClass const& get() const noexcept { return this->Class; }

    NODISCARD constexpr TCxxClass& operator*() noexcept { return this->Class; }
    NODISCARD constexpr TCxxClass const& operator*() const noexcept { return this->Class; }
    NODISCARD constexpr TCxxClass* operator->() noexcept { return &this->Class; }
    NODISCARD constexpr TCxxClass const* operator->() const noexcept { return &this->Class; }

    NODISCARD constexpr explicit operator TCxxClass*() noexcept { return &this->Class; }
    NODISCARD constexpr explicit operator TCxxClass const*() const noexcept { return &this->Class; }

    NODISCARD constexpr TCxxClass* operator&() noexcept { return &this->Class; }
    NODISCARD constexpr TCxxClass* operator&() const noexcept { return &this->Class; }

    TCxxClass& Class;
    bool bReleased{};
};

template<typename TDynInit, template<typename> typename TStatInit, template<typename> typename TResult, typename TRootNode, typename... TForbiddenNodes>
struct NewDeferredObjectFn
{
    //# Whether the typename #TCxxClass is allowed to be used as a node in this struct to create a new deferred jxx-object.
    template<typename TCxxClass>
    static constexpr bool AllowedTreeNode{algo::allowed_tree_node<TCxxClass, TRootNode, TForbiddenNodes...>};

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
        return TResult<TCxxClass>{(*this)(Init)};
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
    static constexpr bool AllowedTreeNode{algo::allowed_tree_node<TCxxClass, TRootNode, TForbiddenNodes...>};

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
//# @note You can also create unique objects with #NewUniqueObject.
//#
inline constexpr Detail::NewObjectFn<decltype(NewDeferredObject), LCxxDynamicInit, TCxxStaticInit, JCxxClass, AActor, WNode> NewObject{NewDeferredObject};

//#
//# The base class for all objects that share a lifetime among its owner and that are detected automatically
//# by the jafg build tool to allow for dynamic casting, network replication, etc.
//# This class defines the bare minimum for an object to be a jafg object.
//#
static_assert(!algo::detail::is_complete_type_v<NextIsBaseJxxClass>);
PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsBaseJxxClass")
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API JCxxClass : public LEngineGetters
{
    GENERATED_CLASS_BODY()

public:

    typedef Detail::LCxxOuterIdentityProj LDynamicInitProj;

private:

    friend Detail::LCarnifex;
    LJxxClass const& JafgVirtualTable;

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
    virtual void BeginLife()
    {
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
        jassert(this->bHasBegunLife == false)
        this->bHasBegunLife = true;
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */
        if (this->GetVirtualTable().IsConfig())
        {
            this->PullConfig();
        }
    }
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
    NODISCARD FORCEINLINE constexpr bool _HasBegunLife() const noexcept { return this->bHasBegunLife; }
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */

    NODISCARD FORCEINLINE constexpr LJxxClass const& GetVirtualTable() const noexcept { return this->JafgVirtualTable; }

    //#
    //# Marks this object instance as garbage, and it will be killed at the end of this or the next tick depending
    //# on call time inside said tick.
    //#
    FORCEINLINE void MarkAsGarbage_v2(EJxxRecordTearDownReason Reason = EJxxRecordTearDownReason::Default)
    {
        check(Tasks::IsOnMasterThread())
        if (this->bGarbage)
        {
            LOG_WARNING(LogObjectInternal, "Class [{}] was already marked as garbage.", this->GetNameAsString())
            return;
        }
        this->MarkAsGarbage(EMarkAsGarbageBehavior::Default, Reason);
    }
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
    FORCEINLINE void KillYourSelfNow_v2(EJxxRecordTearDownReason Reason = EJxxRecordTearDownReason::Default, bool bMayBeGarbage = false)
    {
        checkCode(if (bMayBeGarbage == false) { check(!this->_IsGarbage()) })
        if (this->_IsGarbage())
        {
            LOG_ERROR(LogObjectInternal, "Class [{}] was already marked as garbage. Failed to devour.", this->GetNameAsString())
        }
        else
        {
            this->MarkAsGarbage(EMarkAsGarbageBehavior::DevourNow, Reason);
        }
    }

    //#
    //# Delegate called when this object was marked as garbage.
    //# Use this for immediate reaction to be killed. Otherwise, use the dctor that will usually be called at the end
    //# of the tick this delegate was called.
    //#
    virtual void OnGarbage(EJxxRecordTearDownReason Reason)
    {
        check(this->_IsGarbage())
        if (this->GetVirtualTable().IsConfig())
        {
            this->PushConfig();
        }
    }

    //#
    //# Gets the context that this object lives in and shares its lifetime with it.
    //# Lifetimes can be abridged by calling either #MarkAsGarbage or #KillYourSelfNow.
    //#
    FORCEINLINE LClassOuter      & GetOuter() noexcept { return this->Outer; }
    FORCEINLINE LClassOuter const& GetOuter() const noexcept { return this->Outer; }

    NODISCARD FORCEINLINE Detail::LReflectedTag GetTag() const noexcept { return this->GetVirtualTable().GetTag(); }
    NODISCARD FORCEINLINE LString const& GetNameAsString() const noexcept { return this->GetVirtualTable().GetFullyQualifiedName(); }
    NODISCARD FORCEINLINE LStringView GetNameAsStringView() const noexcept { return this->GetNameAsString(); }

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA() const noexcept { return this->GetVirtualTable().DerivesFrom(TObj::StaticClass()); }
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA(TObj const** CastedOut) const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA(TObj** CastedOut) noexcept;
    FORCEINLINE bool IsA(LJxxClass const& Class) const noexcept { return this->GetVirtualTable().DerivesFrom(Class); }
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE bool IsA(TSubclassOf<TObj> Class) const noexcept;

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* As() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* As() const noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* AsChecked() noexcept
    {
        TObj* Out{this->As<TObj>()};
        check(Out)
        return Out;
    }
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* AsChecked() const noexcept
    {
        TObj const* Out{this->As<TObj>()};
        check(Out)
        return Out;
    }
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj* AsAsserted() noexcept
    {
        TObj* Out{this->As<TObj>()};
        jassert(Out)
        return Out;
    }
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const* AsAsserted() const noexcept
    {
        TObj const* Out{this->As<TObj>()};
        jassert(Out)
        return Out;
    }

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj& AsStatic() noexcept;
    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TObj const& AsStatic() const noexcept;

    //# If you want a custom path, the override this and call super with your path.
    ENGINE_API virtual void PullConfig(LPath const& InPath = {}) noexcept;
    ENGINE_API virtual void PushConfig(LPath const& InPath = {}) const noexcept;

protected:

    NODISCARD FORCEINLINE LClassOuter& GetMutableOuter() const noexcept { return this->Outer; }

private:

    LClassOuter& Outer;

    enum struct EMarkAsGarbageBehavior
    {
        Default,
        Ignore,
        DevourNow
    };

    void MarkAsGarbage(EMarkAsGarbageBehavior Behavior, EJxxRecordTearDownReason Reason);
#if JAFG_DO_CHECKS
    ENGINE_API static void _check_BeginClassLife(LBeginClassLifeInfo const& Info);
    ENGINE_API static void _check_EndClassLife(LEndClassLifeInfo const& Info);
#endif /* JAFG_DO_CHECKS */

    bool bGarbage:1{};
#if JAFG_DO_DOUBLE_CHECK_LIFETIMES
    bool bHasBegunLife:1{};
#endif /* JAFG_DO_DOUBLE_CHECK_LIFETIMES */
};

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
    static ::TSubclassOf<MyClassName> StaticSubclass() noexcept          \
    {                                                                           \
        return TSubclassOf<MyClassName>(MyClassName::StaticClass());            \
    }

//# Object context used to determine the context and lifetimes of reflected classes.
class LClassOuter
{
public:

    LClassOuter() noexcept : HumanReadableName("<anonymous>") { }
    ENGINE_API explicit LClassOuter(LString HumanReadableName, bool bRegisterToEngine = true) noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LClassOuter)
    ENGINE_API virtual ~LClassOuter() noexcept;

    ENGINE_API void TearDown() noexcept;

    FORCEINLINE constexpr bool IsUserDataValid() const noexcept { return this->UserData != nullptr; }
    FORCEINLINE constexpr void SetUserData(void* InUserData) noexcept { this->UserData = InUserData;     }
    FORCEINLINE constexpr void* GetUserData() noexcept { return this->UserData; }

    FORCEINLINE void SetHumanReadableName(LStringView const& S) noexcept { this->HumanReadableName = S; }
    FORCEINLINE LString const& GetHumanReadableName() const noexcept { return this->HumanReadableName; }

    //# For internal use only. DO NOT USE.
    FORCEINLINE void _AddEmployee(JCxxClass* Employee) noexcept
    {
        check(this->IsHiredHere(Employee) == false)
        this->Employees.emplace_back(Employee);
    }

    FORCEINLINE bool IsHiredHere(JCxxClass const* Employee) const noexcept { return algo::contains(this->Employees, Employee, &TUnique<JCxxClass>::get); }
    FORCEINLINE TArray<TUnique<JCxxClass>> const& GetEmployees() const noexcept { return this->Employees; }

    //# @return How many employees were hired and killed in this outer.
    std::size_t KillEmployeesFromForeignPlugin(
          LLoadedPluginHandle PluginHandle
        , EJxxRecordTearDownReason Reason = EJxxRecordTearDownReason::PluginUnload
        );

    virtual bool IsWorld() const noexcept { return false; }
    LWorld& AsWorld() noexcept;
    LWorld const& AsWorld() const noexcept;

    FORCEINLINE TUnique<JCxxClass> Poach(JCxxClass* Employee) noexceptcheck
    {
        check(Employee)
        auto It{algo::find(this->Employees, Employee, algo::unique_raw)};
        check( It != this->Employees.end() )

        TUnique Out{std::move(*It)};
        check(*It == nullptr)
        check(Out.get() == Employee)
        this->Employees.erase(It);

        return Out;
    }

    FORCEINLINE TUnique<JCxxClass> PoachToNull(JCxxClass* Employee) noexceptcheck
    {
        check( Employee )
        auto It{algo::find(this->Employees, Employee, algo::unique_raw)};
        check( It != this->Employees.end() )

        TUnique Out{std::move(*It)};
        check(*It == nullptr)
        check(Out.get() == Employee)

        return Out;
    }

protected:

    virtual void OnTearDown() { }

private:

    void RegisterToEngine();
    void UnregisterFromEngine();

    void* UserData{};
    LString HumanReadableName;
    TArray<TUnique<JCxxClass>> Employees;
    bool bWasRegisteredToEngine{};
};

inline JCxxClass::JCxxClass(LCxxDynamicInit const& Init) noexcept
    : JafgVirtualTable{Init.Class}, Outer{Init.Outer}
{
    this->Outer._AddEmployee(this);
}

template<typename TCxxClass>
inline JCxxClass::JCxxClass(TCxxStaticInit<TCxxClass> const& Init) noexcept
    : JafgVirtualTable{TCxxClass::StaticClass()}, Outer{Init.Outer}
{
    this->Outer._AddEmployee(this);
}

namespace Detail
{

//#
//# A carnifex is an object used to mascara all garbage children.
//# At the very end of every tick, it will look for them and kill them.
//#
class LCarnifex final
{
public:

    constexpr LCarnifex() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCarnifex)
    ~LCarnifex() = default;

    FORCEINLINE void AddGarbageChild(TUnique<JCxxClass> Child) noexcept
    {
        check(Child.get())
        this->GarbageChildren.emplace_back(std::move(Child));
    }

    ENGINE_API void KillAllGarbageChildren();
    ENGINE_API void DevourGarbageChildNow(TUnique<JCxxClass> Child);

    FORCEINLINE auto const& GetGarbageChildren() const noexcept { return this->GarbageChildren; }

private:

    TArray<TUnique<JCxxClass>> GarbageChildren;
};
ENGINE_API LCarnifex& GetGlobalCarnifex() noexcept;

} /* ~Namespace  Detail */

//# Finalize a deferred object that has been created with #NewDeferredObject.
template<typename TCxxObject> requires std::is_base_of_v<JCxxClass, TCxxObject>
FORCEINLINE void MakeCxxObjectFinal(TCxxObject& O) { O.BeginLife(); }

namespace Detail
{
struct StaticClassFn
{
    LJxxClass& operator()(LStringView Name) const
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

//#
//# Only checks if the object can be casted if JAFG_DO_CHECKS is true. If the object fails to cast to the
//# targeted type, the application will panic. If JAFG_DO_CHECKS is false, it will assume that the object is
//# of the target type and will do an unsafe cast.
//# Use this only in places where you would also use static_cast.
//#
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
//# Same as #StaticCast, but it will not allow nullptr to be returned if #DO_CHECKS is true.
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
//# Same as #StaticCast, but it will not allow nullptr to be returned.
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
NODISCARD FORCEINLINE TCxxClass const& LJxxClass::GetSingleton() const noexcept
{
    check(this->IsSingleton())
    return *StaticCastChecked<TCxxClass>(this->Singleton);
}

template<typename TCxxClass /* = JCxxClass */> requires std::is_base_of_v<JCxxClass, TCxxClass>
NODISCARD FORCEINLINE TCxxClass& LJxxClass::GetSingleton() noexcept
{
    check(this->IsSingleton())
    return *StaticCastChecked<TCxxClass>(this->Singleton);
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
NODISCARD FORCEINLINE bool IsValidFast(LClassOuter const& Outer, JCxxClass const* Obj) noexcept
{
    if (Obj == nullptr) { return false; }
    if (!Outer.IsHiredHere(Obj)) { return false; }
    return !Obj->_IsGarbage();
}

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
    }
};

} /* ~Namespace Detail */

} /* ~Namespace Jafg */

//#
//# This struct is meant as a wrapper class for storing object pointers for a longer time.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
template<typename TObj> requires algo::is_base_of_weak_v<Jafg::JCxxClass, TObj>
struct TClassStorage final
{
    FORCEINLINE constexpr TClassStorage() noexcept : Outer{nullptr}, Pointer{nullptr} { checkCode(this->CheckValidState()) }
    FORCEINLINE constexpr TClassStorage(std::nullptr_t) noexcept : Outer{nullptr}, Pointer{nullptr} { checkCode(this->CheckValidState()) }
    FORCEINLINE constexpr TClassStorage(TObj* InPointer) noexcept : Outer{nullptr}, Pointer{InPointer}
    {
        if (InPointer)
        {
            this->Outer = &InPointer->GetOuter();
        }
        checkCode(this->CheckValidState())
    }
    FORCEINLINE constexpr TClassStorage(TClassStorage const& Other) noexcept = default;
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage const& Rhs) noexcept = default;
    FORCEINLINE constexpr TClassStorage(TClassStorage&& Other) noexcept
    {
        this->Outer = Other.Outer;
        this->Pointer = Other.Pointer;
        Other.Outer = nullptr;
        Other.Pointer = nullptr;
        checkCode(this->CheckValidState(); Other.CheckValidState())
    }
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            this->Outer = Rhs.Outer;
            this->Pointer = Rhs.Pointer;
            Rhs.Outer = nullptr;
            Rhs.Pointer = nullptr;
            checkCode(this->CheckValidState(); Rhs.CheckValidState())
        }
        return *this;
    }

    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage(TClassStorage<UObj> const& Other) noexcept
    {
        this->Outer = Other.Outer;
        this->Pointer = Other.Pointer;
        checkCode(this->CheckValidState())
    }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage<UObj> const& Rhs) noexcept
    {
        this->Outer = Rhs.Outer;
        this->Pointer = Rhs.Pointer;
        checkCode(this->CheckValidState())
        return *this;
    }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage(TClassStorage<UObj>&& InOther) noexcept
    {
        this->Outer = InOther.Outer;
        this->Pointer = InOther.Pointer;
        InOther.Outer = nullptr;
        InOther.Pointer = nullptr;
        checkCode(this->CheckValidState() && InOther.CheckValidState())
    }
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr TClassStorage& operator=(TClassStorage<UObj>&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            this->Outer = Rhs.Outer;
            this->Pointer = Rhs.Pointer;
            Rhs.Outer = nullptr;
            Rhs.Pointer = nullptr;
            checkCode(this->CheckValidState() && Rhs.CheckValidState())
        }
        return *this;
    }

    FORCEINLINE constexpr ~TClassStorage() noexcept = default;

    //# Nulls the underlying object out.
    FORCEINLINE constexpr void Reset() noexcept { *this = {}; }

    FORCEINLINE constexpr bool operator==(std::nullptr_t) const noexcept { return this->Pointer == nullptr; }
    FORCEINLINE constexpr bool operator==(TObj* InOther) const noexcept { return this->Pointer == InOther; }
    FORCEINLINE constexpr bool operator==(TClassStorage const& InOther) const noexcept = default;
    template<typename UObj> requires(!std::is_same_v<TObj, UObj> && std::is_base_of_v<TObj, UObj>)
    FORCEINLINE constexpr bool operator==(TClassStorage<UObj> const& Rhs) const noexcept { return this->Outer == Rhs.Outer && this->Pointer == Rhs.Pointer; }

    //# Checks whether the underlying object pointer is null. Equivalent to if (Pointer) { ... }
    FORCEINLINE constexpr explicit operator bool() const noexcept { return !!this->Pointer; }
    FORCEINLINE constexpr bool operator!() const noexcept { return !this->Pointer; }

    FORCEINLINE bool IsValidFast() const noexcept { return this->Outer && Jafg::IsValidFast(*this->Outer, this->Pointer); }
    FORCEINLINE bool IsValidSlow() const noexcept { return Jafg::IsValidSlow(this->Outer, this->Pointer); }

    FORCEINLINE TObj* get() noexcept { check(!this->Pointer || this->IsValidFast()) return this->Pointer; }
    FORCEINLINE TObj const* get() const noexcept { check(!this->Pointer || this->IsValidFast()) return this->Pointer; }
    FORCEINLINE TObj* operator->() noexcept { return this->get(); }
    FORCEINLINE TObj const* operator->() const noexcept { return this->get(); }
    FORCEINLINE TObj& operator*() noexcept { return *this->get(); }
    FORCEINLINE TObj const& operator*() const noexcept { return *this->get(); }

    FORCEINLINE Jafg::LClassOuter* get_outer() noexcept { return this->Outer; }
    FORCEINLINE Jafg::LClassOuter const* get_outer() const noexcept { return this->Outer; }
    FORCEINLINE TObj* get_unsafe() noexcept { return this->Pointer; }
    FORCEINLINE TObj const* get_unsafe() const noexcept { return this->Pointer; }

private:

#if JAFG_DO_CHECKS
    FORCEINLINE constexpr void CheckValidState() const noexcept
    {
        if (this->Pointer)
        {
            jassert(this->Outer)
        }
        else
        {
            jassert(this->Outer == nullptr)
        }
    }
#endif /* JAFG_DO_CHECKS */

    Jafg::LClassOuter* Outer;
    TObj* Pointer;
};

template<typename TObj> requires algo::is_base_of_weak_v<Jafg::JCxxClass, TObj>
class TSubclassOf final
{
    template<typename UObj> requires algo::is_base_of_weak_v<Jafg::JCxxClass, UObj>
    friend class TSubclassOf;

public:

    constexpr TSubclassOf() noexcept : Class{nullptr} { }
    constexpr TSubclassOf(std::nullptr_t) noexcept : Class{nullptr} { }

    TSubclassOf(EDefaultInit) noexcept : Class{&TObj::StaticClass()} { check(this->HasClass() && this->IsValidType()) }

    TSubclassOf(TSubclassOf const& Other) noexcept = default;
    TSubclassOf& operator=(TSubclassOf const& Rhs) noexcept = default;

    TSubclassOf(Jafg::LJxxClass const* InClass) noexcept : Class{InClass} { check(this->IsValidType()) }
    TSubclassOf(Jafg::LJxxClass const& InClass) noexcept : Class{&InClass} { check(this->IsValidType()) }
    TSubclassOf& operator=(Jafg::LJxxClass const* InClass) noexcept { return this->Assign(InClass); }
    TSubclassOf& operator=(Jafg::LJxxClass const& InClass) noexcept { return this->Assign(InClass); }

    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf(TSubclassOf<UObj> const& Other) noexcept : Class{Other.Class} { check(this->IsValidType()) }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf& operator=(TSubclassOf<UObj> const& Other) noexcept { return this->Assign(Other.Class); }

    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf(UObj const* What) noexcept : Class{What->GetVirtualTable()} { check(this->IsValidType()) }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf& operator=(UObj const* What) noexcept { return this->Assign(What->GetVirtualTable()); }

    constexpr ~TSubclassOf() = default;

    FORCEINLINE bool HasClass() const noexcept { return this->Class != nullptr; }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    FORCEINLINE void SetClass() noexcept { this->Assign(UObj::StaticClass()); }
    FORCEINLINE void SetClass(std::nullptr_t) noexcept { this->Assign(nullptr); }

    FORCEINLINE TSubclassOf& Assign(Jafg::LJxxClass const* InClass) noexcept
    {
        this->Class = InClass;
        check(this->IsValidType())
        return *this;
    }
    FORCEINLINE TSubclassOf& Assign(Jafg::LJxxClass const& InClass) noexcept
    {
        this->Class = &InClass;
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE Jafg::LJxxClass const& GetClassOrDefault() const noexcept
    {
        if (this->HasClass())
        {
            return *this->Class;
        }
        return TObj::StaticClass();
    }

    FORCEINLINE Jafg::LJxxClass const* GetClass() const noexcept { return this->Class; }
    FORCEINLINE operator Jafg::LJxxClass const*() const noexcept { return this->Class; }

    FORCEINLINE operator Jafg::LJxxClass const&() const noexcept { check(this->HasClass()) return *this->Class; }

    FORCEINLINE Jafg::LJxxClass const* operator->() const noexcept { check(this->HasClass()) return this->Class; }
    FORCEINLINE Jafg::LJxxClass const* operator*() const noexcept { if (this->HasClass()) { return this->Class; } return nullptr; }

    FORCEINLINE bool IsValidType() const noexcept
    {
        if (this->Class)
        {
            if (this->Class->DerivesFrom<TObj>())
            {
                return true;
            }
            if (this->Class->IsRoot() || this->Class->IsParentValid())
            {
                return false;
            }
            LOG_WARNING(LogObjectInternal,
                "[{}] is used in TSubclassOf<{}> but not yet initialized. Assuming valid parent.",
                this->Class->GetFullyQualifiedName(),
                TObj::StaticClass().GetFullyQualifiedName()
                )
            return true;
        }
        return true;
    }

    FORCEINLINE constexpr bool operator==(std::nullptr_t) const noexcept { return this->Class == nullptr; }

private:

    Jafg::LJxxClass const* Class;
};

template<typename TObj> requires std::is_base_of_v<Jafg::JCxxClass, TObj>
FORCEINLINE bool Jafg::JCxxClass::IsA(TSubclassOf<TObj> Class) const noexcept
{
    return this->IsA(Class.GetClassOrDefault());
}

template<typename T> requires algo::is_base_of_weak_v<Jafg::JCxxClass, T>
struct TPreference<TSubclassOf<T>>: Jafg::TDefaultPreference<TSubclassOf<T>>
{
    using Jafg::TDefaultPreference<TSubclassOf<T>>::TDefaultPreference;
    using Jafg::TDefaultPreference<TSubclassOf<T>>::operator=;
};

template<typename T>
struct std::formatter<TSubclassOf<T>> : std::formatter<LString>
{
    FORCEINLINE std::format_context::iterator format(const TSubclassOf<T>& InClass, std::format_context&  InContext) const
    {
        if (InClass)
        {
            return std::formatter<LString>::format(InClass->GetNameAsString(), InContext);
        }
        return std::formatter<LString>::format(algo::sprintf("{}?", T::StaticClass().GetNameAsString()), InContext);
    }
};

template<>
struct std::formatter<Jafg::Detail::LReflectedTag> : std::formatter<LString>
{
    FORCEINLINE std::format_context::iterator format(Jafg::Detail::LReflectedTag Tag, std::format_context& Context) const
    {
        return std::formatter<LString>::format(Jafg::Detail::GetJxxTagRegistry().GetReprSafe(Tag), Context);
    }
};

template<typename TCxxClass, typename TArchive> requires std::is_base_of_v<Jafg::JCxxClass, TCxxClass>
    && serde::os_string_archive_v<TArchive>
struct serde::TSerializer<TSubclassOf<TCxxClass>, TArchive>
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
    && serde::is_string_archive_v<TArchive>
struct serde::TDeserializer<TSubclassOf<TCxxClass>, TArchive>
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
                    .Error = algo::sprintf("Package [{}] is not a class. Found [{}].",
                        Package->GetFullyQualifiedName(), LexToString(Package->GetType())
                        )
                    };
            }
        }
        else
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("No such package: [{}].", Ar.Stream)
                };
        }
        return {};
    }
};

namespace Jafg
{

namespace Detail
{

template<typename TCreator, typename TDynInit, template<typename> typename TStatInit, typename TRootNode, typename... TForbiddenNodes>
struct NewUniqueObjectFn
{
    //# Whether the typename #TCxxClass is allowed to be used as a node in this struct to create a new jxx-object.
    template<typename TCxxClass>
    static constexpr bool AllowedTreeNode{algo::allowed_tree_node<TCxxClass, TRootNode, TForbiddenNodes...>};

    FORCEINLINE TJxxUnique<TRootNode> operator()(TDynInit const& Init) const
    {
        return TJxxUnique<TRootNode>{this->Creator(Init)};
    }

    template<typename TCxxClass> requires AllowedTreeNode<TCxxClass>
    FORCEINLINE TJxxUnique<TCxxClass> operator()(CastTo<TCxxClass> Target, TDynInit const& Init) const
    {
        return TJxxUnique<TCxxClass>{this->Creator(Target, Init)};
    }

    template<typename TCxxClass, typename... TArgs> requires
           AllowedTreeNode<TCxxClass>
        && NewStaticCxxFn::is_constructible_v<TCxxClass, TStatInit<TCxxClass> const&, TArgs&&...>
    FORCEINLINE TJxxUnique<TCxxClass> operator()(TStatInit<TCxxClass> const& Init, TArgs&&... Args) const
    {
        return TJxxUnique<TCxxClass>{this->Creator(Init, std::forward<TArgs>(Args)...)};
    }

    TCreator& Creator;
};

} /* ~Namespace Detail */

//# Creates a unique object.
inline constexpr Detail::NewUniqueObjectFn<decltype(NewObject), LCxxDynamicInit, TCxxStaticInit, JCxxClass, AActor, WNode> NewUniqueObject{NewObject};

#if JAFG_WITH_EDITOR

//# If you have custom editor types that you want to present, you have to specialize this function.
template<typename T> Detail::LNodeFactoryBase GetEditorNode(TEditorNodeCreateInfo<T> const& Info) noexcept = delete;
//# These common specializations are provided by jafg.
template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LVec3F>(TEditorNodeCreateInfo<LVec3F> const& Info) noexcept;
template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LVec3D>(TEditorNodeCreateInfo<LVec3D> const& Info) noexcept;
template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LWorldTrans>(TEditorNodeCreateInfo<LWorldTrans> const& Info) noexcept;

template<>
struct TEditorNodeCreateInfo<LString> final
{
    //# The owning viewport of the new editor node.
    LViewport& Viewport;
    //# The owner that owns the data-field. This field is optional and may be null.
    JCxxClass* Owner{};
    //# The field that is made accessible.
    LString& Field;
    //# Optional default value.
    std::optional<LString> Default;
    //# Optional update function.
    TFunction2<void()>& UpdateValue;
    //# Optional human-readable name.
    LString What{"String"};
};
template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LString>(TEditorNodeCreateInfo<LString> const& Info) noexcept;

#endif /* JAFG_WITH_EDITOR */

} /* ~Namespace Jafg */
