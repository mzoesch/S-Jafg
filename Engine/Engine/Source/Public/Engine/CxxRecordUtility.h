// Copyright mzoesch. All rights reserved.

#pragma once

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

#ifndef PURE_VIRTUAL
    #if DO_PURE_VIRTUAL_COMPILER_CHECKS
        #define PURE_VIRTUAL(...)           = 0;
    #else /* DO_PURE_VIRTUAL_COMPILER_CHECKS */
        //# Define a RetTy for non-void members if needed.
        #define PURE_VIRTUAL(...)           { panic( "Pure virtual function was encountered." ) __VA_ARGS__; }
    #endif /* !DO_PURE_VIRTUAL_COMPILER_CHECKS */
#endif /* !PURE_VIRTUAL */

#ifndef NON_CALLABLE_MEMBER
    //# A member that was derived but is not callable.
    #define NON_CALLABLE_MEMBER(...)        { panic( "Non-callable member function was encountered." ) __VA_ARGS__; }
#endif /* !NON_CALLABLE_MEMBER */

#include "Engine/CxxClassFlags.h"

namespace Jafg
{

class LCxxClass;
class LCarnifex;
class JCxxClass;
class LClassOuter;

namespace Private
{

struct LRegistryPackage;
struct LRegistryClassPackage;

struct LRegistryPackage
{
    enum Type
    {
        None,
        Class,
    };

    LRegistryPackage(LRegistryPackage::Type InType) noexcept : PackageType(InType) { check( this->IsTypeValid() ) }
    virtual ~LRegistryPackage() noexcept = default;

    virtual LString const& GetFullyQualifiedName() const noexcept = 0;

    FORCEINLINE Type GetType() const noexcept { return this->PackageType; }
    FORCEINLINE bool IsTypeValid() const noexcept { return this->PackageType != Type::None; }
    FORCEINLINE bool IsClass() const noexcept { return this->PackageType == Type::Class; }

    NODISCARD FORCEINLINE LRegistryClassPackage const& AsClass() const noexcept;
    NODISCARD FORCEINLINE LRegistryClassPackage& AsClass() noexcept;

    LRegistryPackage::Type PackageType{ LRegistryPackage::Type::None };

    LLoadedPluginHandle Origin;
};

struct LRegistryClassPackage final : public LRegistryPackage
{
    LRegistryClassPackage() noexcept = delete;
    LRegistryClassPackage(
          LCxxClass& InStaticClass
        , ECxxClassFlags::Type InFlags
        , LStringView FullyQualifiedParentName = {}
        ) noexcept
        : LRegistryPackage(LRegistryPackage::Type::Class), StaticClass(InStaticClass), Flags(InFlags)
    {
    }

    LCxxClass& StaticClass;
    ECxxClassFlags::Type Flags{ ECxxClassFlags::None };

    NODISCARD FORCEINLINE virtual LString const& GetFullyQualifiedName() const noexcept override;
};

NODISCARD FORCEINLINE LRegistryClassPackage const& LRegistryPackage::AsClass() const noexcept
{
    check( this->IsClass() )
    return *static_cast<LRegistryClassPackage const*>(this);
}

NODISCARD FORCEINLINE LRegistryClassPackage& LRegistryPackage::AsClass() noexcept
{
    check( this->IsClass() )
    return *static_cast<LRegistryClassPackage*>(this);
}

//# Holds all registered CxxRecords for all plugins.
class LCxxRecordRegistry final
{
public:

    typedef TArray<TUnique<LRegistryPackage>> LPackages;

    ENGINE_API LCxxRecordRegistry() noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxRecordRegistry)
    ~LCxxRecordRegistry() noexcept = default;

    NODISCARD FORCEINLINE constexpr bool AreNewPendingPackagesAllowed() const noexcept { return this->bAllowNewPendingPackages; }
    NODISCARD FORCEINLINE constexpr bool IsCDRRegistrationAllowed() const noexcept { return this->bAllowCDRRegistration; }

    ENGINE_API void AddNewPendingPackage(TUnique<LRegistryPackage> Package);

    //#
    //# Registers all pending packages that are waiting for registration.
    //# Loads them into memory and initializes the default package referrer for them.
    //#
    ENGINE_API void LoadPendingPackages(const LLoadedPluginHandle Handle);

    //# Kills all pending packages that might get loaded by a malformed plugin.
    ENGINE_API void KillPendingPackages();

    //# Whether to allow now new pending packages to be loaded.
    ENGINE_API void SetAllowNewPendingPackages(const bool bAllow) noexcept;

    //# Removes all packages loaded by the provided plugin handle.
    ENGINE_API LSize RemovePackagesOf(const LLoadedPluginHandle Handle);

    ENGINE_API void TearDown();

    FORCEINLINE LPackages const& GetPendingPackages() const noexcept { return this->PendingPackages; }
    FORCEINLINE LPackages& GetMutablePendingPackages() noexcept { return this->PendingPackages; }

    FORCEINLINE LPackages const& GetRegisteredPackages() const noexcept { return this->RegisteredPackages; }
    FORCEINLINE LPackages& GetMutableRegisteredPackages() noexcept { return this->RegisteredPackages; }

    FORCEINLINE LClassOuter& GetMutableOuter() noexcept { return this->Outer; }
    FORCEINLINE LClassOuter const& GetOuter() const noexcept { return this->Outer; }

    FORCEINLINE bool DoesPackageWithNameExist(LString const& FullyQualifiedName) const noexcept { return this->GetPackageByName(FullyQualifiedName) != nullptr; }
    FORCEINLINE bool DoesPackageWithNameExist(LStringView const& FullyQualifiedName) const noexcept { return this->GetPackageByName(FullyQualifiedName) != nullptr; }

    FORCEINLINE LRegistryPackage const* GetPackageByName(LString const& FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryPackage const* GetPackageByNameChecked(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameAsserted(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; jassert( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByName(LStringView const& FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryPackage const* GetPackageByNameChecked(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameAsserted(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByName(FullyQualifiedName) }; jassert( Out ); return Out; }

    FORCEINLINE LRegistryPackage const* GetPackageByNameWeak(LString const& Name) const noexcept;
    FORCEINLINE LRegistryPackage const* GetPackageByNameWeakChecked(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByNameWeak(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameWeakAsserted(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByNameWeak(FullyQualifiedName) }; jassert( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameWeak(LStringView const& Name) const noexcept;
    FORCEINLINE LRegistryPackage const* GetPackageByNameWeakChecked(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByNameWeak(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryPackage const* GetPackageByNameWeakAsserted(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetPackageByNameWeak(FullyQualifiedName) }; jassert( Out ); return Out; }

    FORCEINLINE LRegistryClassPackage const* GetClassByName(LString const& FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetClassByNameChecked(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameAsserted(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; jassert( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByName(LStringView const& FullyQualifiedName) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetClassByNameChecked(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameAsserted(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByName(FullyQualifiedName) }; jassert( Out ); return Out; }


    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeak(LString const& Name) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeakChecked(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByNameWeak(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeakAsserted(LString const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByNameWeak(FullyQualifiedName) }; jassert( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeak(LStringView const& Name) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeakChecked(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByNameWeak(FullyQualifiedName) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetClassByNameWeakAsserted(LStringView const& FullyQualifiedName) const noexcept { auto const* Out{ this->GetClassByNameWeak(FullyQualifiedName) }; jassert( Out ); return Out; }

    FORCEINLINE LRegistryClassPackage const* GetPackageByCDR(void const* CDR) const noexcept;
    FORCEINLINE LRegistryClassPackage const* GetPackageByCDRChecked(void const* CDR) const noexcept { auto const* Out{ this->GetPackageByCDR(CDR) }; check( Out ); return Out; }
    FORCEINLINE LRegistryClassPackage const* GetPackageByCDRAsserted(void const* CDR) const noexcept { auto const* Out{ this->GetPackageByCDR(CDR) }; jassert( Out ); return Out; }

    template<typename TObj> requires std::is_base_of_v<JCxxClass, TObj>
    FORCEINLINE TArray<LRegistryClassPackage const*> GetClassesByBase() noexcept { return this->GetClassesByBase(*TObj::StaticClass()); }
    FORCEINLINE TArray<LRegistryClassPackage const*> GetClassesByBase(LCxxClass const& Base) noexcept;

private:

    LPackages PendingPackages;
    LPackages RegisteredPackages;

    //# Default to true for core static storage packages.
    bool bAllowNewPendingPackages{ true };
    bool bAllowCDRRegistration{ false };

    //# Outer for CDRs.
    LClassOuter& Outer;
};

ENGINE_API LCarnifex& GetGlobalCarnifex() noexcept;
ENGINE_API LCxxRecordRegistry& GetGlobalCxxRecordRegistry() noexcept;

struct LCxxRecordMiscellaneousAccessor final
{
    UTILITY_STRUCT(LCxxRecordMiscellaneousAccessor)

    FORCEINLINE static JCxxClass* MallocClass(JCxxClass const& Class);
    FORCEINLINE static void ChangeOuter(JCxxClass* Obj, LClassOuter* NewOuter) noexcept;
};

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::GetPackageByName(LString const& FullyQualifiedName) const noexcept
{
    if (auto const* Out{ algo::find_pointer(this->RegisteredPackages, FullyQualifiedName, [](auto const& E){ return E->GetFullyQualifiedName(); }) })
    {
        return Out->get();
    }

    return nullptr;
}

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::GetPackageByName(LStringView const& FullyQualifiedName) const noexcept
{
    if (auto const* Out{ algo::find_pointer(this->RegisteredPackages, FullyQualifiedName, [](auto const& E){ return E->GetFullyQualifiedName(); }) })
    {
        return Out->get();
    }

    return nullptr;
}

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::GetPackageByNameWeak(LString const& Name) const noexcept
{
    if (LRegistryPackage const* Out{ this->GetPackageByName(Name) })
    {
        return Out;
    }

    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E){ return E->GetFullyQualifiedName().ends_with(Name); }) })
    {
        return Out->get();
    }

    return nullptr;
}

FORCEINLINE LRegistryPackage const* LCxxRecordRegistry::GetPackageByNameWeak(LStringView const& Name) const noexcept
{
    if (LRegistryPackage const* Out{ this->GetPackageByName(Name) })
    {
        return Out;
    }

    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E){ return E->GetFullyQualifiedName().ends_with(Name); }) })
    {
        return Out->get();
    }

    return nullptr;
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetClassByName(LString const& FullyQualifiedName) const noexcept
{
    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&FullyQualifiedName](auto const& E)
    {
        return E->IsClass() && E->GetFullyQualifiedName() == FullyQualifiedName;
    })})
    {
        return &Out->get()->AsClass();
    }

    return nullptr;
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetClassByName(LStringView const& FullyQualifiedName) const noexcept
{
    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&FullyQualifiedName](auto const& E)
    {
        return E->IsClass() && E->GetFullyQualifiedName() == FullyQualifiedName;
    })})
    {
        return &Out->get()->AsClass();
    }

    return nullptr;
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetClassByNameWeak(LString const& Name) const noexcept
{
    if (LRegistryClassPackage const* Out{ this->GetClassByName(Name) })
    {
        return Out;
    }

    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E)
    {
        return E->IsClass() && E->GetFullyQualifiedName().ends_with(Name);
    })})
    {
        return &Out->get()->AsClass();
    }

    return nullptr;
}

FORCEINLINE LRegistryClassPackage const* LCxxRecordRegistry::GetClassByNameWeak(LStringView const& Name) const noexcept
{
    if (LRegistryClassPackage const* Out{ this->GetClassByName(Name) })
    {
        return Out;
    }

    if (auto const* Out{ algo::find_pointer_if(this->RegisteredPackages, [&Name](auto const& E)
    {
        return E->IsClass() && E->GetFullyQualifiedName().ends_with(Name);
    })})
    {
        return &Out->get()->AsClass();
    }

    return nullptr;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg */
