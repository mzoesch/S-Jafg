// Copyright mzoesch. All rights reserved.

#pragma once

#include "Foreign/PluginForward.h"

///////////////////////////////////////////////////////////////////////////////
// Compiler options

//#
//# Whether to double-check lifetimes of JObjectBase objects. Meaning check if an object has rightfully begun its life,
//# was marked as garbage, ended its life, then was destroyed and freed in the end.
//# These checks add a meaningful non-neglectable overhead to object creation and destruction and should therefore
//# be disabled in shipping builds.
//#
#ifndef JAFG_DO_DOUBLE_CHECK_LIFETIMES
    #define JAFG_DO_DOUBLE_CHECK_LIFETIMES                  !IN_SHIPPING
#endif /* !JAFG_DO_DOUBLE_CHECK_LIFETIMES */

// ~Compiler options
///////////////////////////////////////////////////////////////////////////////

#ifndef NON_CALLABLE_MEMBER
    //# A member that was derived but is not callable.
    #define NON_CALLABLE_MEMBER(...)        { panic("Non-callable member function was encountered.") __VA_ARGS__; }
#endif /* !NON_CALLABLE_MEMBER */

#include "CxxRecordUtility.h"
#include "Engine/CxxClassFlags.h"

namespace Jafg
{

class LCxxClass;
class LCarnifex;
class JCxxClass;
class LClassOuter;

namespace Detail
{

struct LRegistryPackage;
struct LRegistryClassPackage;

struct LRegistryPackage
{
    enum struct EType
    {
        None,
        Class,
    };

    LRegistryPackage(EType InType) noexcept : PackageType(InType) { check( this->IsTypeValid() ) }
    virtual ~LRegistryPackage() noexcept = default;

    virtual LString const& GetFullyQualifiedName() const noexcept = 0;

    FORCEINLINE constexpr auto GetType() const noexcept { return this->PackageType; }
    FORCEINLINE constexpr bool IsTypeValid() const noexcept { return this->PackageType != EType::None; }
    FORCEINLINE constexpr bool IsClass() const noexcept { return this->PackageType == EType::Class; }

    NODISCARD FORCEINLINE LRegistryClassPackage const& AsClass() const noexcept;
    NODISCARD FORCEINLINE LRegistryClassPackage& AsClass() noexcept;

    EType PackageType;
    LLoadedPluginHandle Origin;
};

struct LRegistryClassPackage final : public LRegistryPackage
{
    LRegistryClassPackage() noexcept = delete;
    LRegistryClassPackage(LCxxClass& InStaticClass) noexcept
        : LRegistryPackage(EType::Class), StaticClass(InStaticClass)
    {
    }

    LCxxClass& StaticClass;

    NODISCARD FORCEINLINE virtual LString const& GetFullyQualifiedName() const noexcept override;
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

//# Holds all registered CxxRecords for all plugins.
class LCxxRecordRegistry final
{
public:

    typedef TArray<TUnique<LRegistryPackage>> LPackages;

    constexpr LCxxRecordRegistry() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxRecordRegistry)
    ~LCxxRecordRegistry() noexcept = default;

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

    //# Prematurly free all packages and do not wait for undefined static storage for said task.
    ENGINE_API void TearDown();

    FORCEINLINE LPackages const& GetPendingPackages() const noexcept { return this->PendingPackages; }
    FORCEINLINE LPackages& GetMutablePendingPackages() noexcept { return this->PendingPackages; }

    FORCEINLINE LPackages const& GetRegisteredPackages() const noexcept { return this->RegisteredPackages; }
    FORCEINLINE LPackages& GetMutableRegisteredPackages() noexcept { return this->RegisteredPackages; }

    FORCEINLINE bool DoesPackageWithNameExist(LStringView FullyQualifiedName) const noexcept { return this->GetPackageByName(FullyQualifiedName) != nullptr; }

    FORCEINLINE LRegistryPackage const* GetPackageByName(LStringView FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryPackage const* GetPackageByNameChecked(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameAsserted(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; jassert( Out ); return Out; }

    FORCEINLINE LRegistryClassPackage const* GetClassByName(LStringView FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetClassByNameChecked(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameAsserted(LStringView FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; jassert( Out ); return Out; }

    // @return All derived classes of TCxxClass inclusive TCxxClass.
    template<typename TCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TArray<LCxxClass const*> GetClassesByBase() const noexcept { return this->GetClassesByBase(TCxxClass::StaticClass()); }
    NODISCARD FORCEINLINE TArray<LCxxClass const*> GetClassesByBase(LCxxClass const& Base) const noexcept;

    //# For internal purposes only. Do not use.
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeak(LStringView Name) const noexcept;
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeakChecked(LStringView Name) const noexcept { auto const* Out{ this->_GetPackageByNameWeak(Name) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* _GetPackageByNameWeakAsserted(LStringView Name) const noexcept { auto const* Out{ this->_GetPackageByNameWeak(Name) }; jassert( Out ); return Out; }
    //# For internal purposes only. Do not use.
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeak(LStringView Name) const noexcept;
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeakChecked(LStringView Name) const noexcept { auto const* Out{ this->_GetClassByNameWeak(Name) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* _GetClassByNameWeakAsserted(LStringView Name) const noexcept { auto const* Out{ this->_GetClassByNameWeak(Name) }; jassert( Out ); return Out; }

private:

    LClassOuter* SingletonOuter{};

    LPackages PendingPackages;
    LPackages RegisteredPackages;

    //# Default to true for core static storage packages.
    bool bAllowNewPendingPackages{ true };
};

ENGINE_API LCarnifex& GetGlobalCarnifex() noexcept;
ENGINE_API LCxxRecordRegistry& GetGlobalCxxRecordRegistry() noexcept;

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::GetPackageByName(LStringView FullyQualifiedName) const noexcept
{
    if (auto const* Out{algo::find_pointer(this->RegisteredPackages, FullyQualifiedName, [](auto const& E){ return E->GetFullyQualifiedName(); })})
    {
        return &**Out;
    }
    return nullptr;
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetClassByName(LStringView FullyQualifiedName) const noexcept
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

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::_GetPackageByNameWeak(LStringView Name) const noexcept
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

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::_GetClassByNameWeak(LStringView Name) const noexcept
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

} /* ~Namespace Detail */

inline LStringView LexToString(Detail::LRegistryPackage::EType Type) noexcept
{
    switch (Type)
    {
    case Detail::LRegistryPackage::EType::Class: { return "Class"; }
    default: { return "<error>"; }
    }
}

} /* ~Namespace Jafg */
