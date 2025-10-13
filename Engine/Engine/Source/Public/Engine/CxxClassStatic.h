// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxRecord.h"
#include "Engine/CxxClassFlags.h"
#include "Engine/CxxClass.h"
#include "Engine/CxxRecordUtility.h"

namespace Jafg
{

namespace Private
{

typedef JCxxClass* (*GetCDR)(void);


} /* ~Namespace Private */

//#
//# A reflected class.
//#
class LCxxClass final : public LCxxRecord
{
    friend Private::LCxxRecordRegistry;

public:

    LCxxClass() = delete;
    LCxxClass(
          Private::GetCDR InGetCDR
        , LStringView InFullyQualifiedParentName
        , TArray<LCxxClassField>&& InFields
        ) noexcept
        : GetCDRFunction(InGetCDR)
        , FullyQualifiedParentName(std::move(InFullyQualifiedParentName))
        , Fields(std::move(InFields))
        { }
    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxClass)
    ~LCxxClass() = default;

    NODISCARD FORCEINLINE bool IsValid() const noexcept { return this->Parent != nullptr || this->GetFullyQualifiedName() == "Jafg::JObjectBase"; }

    NODISCARD FORCEINLINE bool IsRoot() const noexcept { return this->Parent == nullptr && this->GetFullyQualifiedName() == "Jafg::JObjectBase"; }
    NODISCARD FORCEINLINE bool IsParentValid() const noexcept { return this->Parent != nullptr; }
    NODISCARD FORCEINLINE auto GetParent()         noexcept -> LCxxClass*                { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetParent()   const noexcept -> LCxxClass const*          { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetChildren()       noexcept -> TArray<LCxxClass*>&       { return this->Children; }
    NODISCARD FORCEINLINE auto GetChildren() const noexcept -> TArray<LCxxClass*> const& { return this->Children; }

    template<typename T> requires std::is_base_of_v<JCxxClass, T>
    NODISCARD FORCEINLINE bool DerivesFrom() const noexcept { return this->DerivesFrom(*T::StaticClass()); }
    NODISCARD ENGINE_API  bool DerivesFrom(LCxxClass const& Parent) const noexcept;

    NODISCARD FORCEINLINE auto GetFlags()      const noexcept -> LCxxClassFlags { return this->Flags; }
    NODISCARD FORCEINLINE bool HasAnyFlags()   const noexcept { return  this->Flags != ECxxClassFlags::None;                              }
    NODISCARD FORCEINLINE bool IsAbstract()    const noexcept { return (this->Flags  & ECxxClassFlags::Abstract) != ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsNotAbstract() const noexcept { return (this->Flags  & ECxxClassFlags::Abstract) == ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsConfig()      const noexcept { return (this->Flags  & ECxxClassFlags::Config)   != ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsNotConfig()   const noexcept { return (this->Flags  & ECxxClassFlags::Config)   == ECxxClassFlags::None; }

    NODISCARD FORCEINLINE bool IsCDRValid() const noexcept { return this->CDR.get() != nullptr; }
    NODISCARD FORCEINLINE JCxxClass const* GetCDR() const noexceptcheck { check( this->IsCDRValid() ) return this->CDR.get(); }
    NODISCARD FORCEINLINE JCxxClass* GetMutableCDR() noexceptcheck { check( this->IsCDRValid() ) return this->CDR.get(); }
    template<typename T> requires std::is_base_of_v<JCxxClass, T>
    NODISCARD FORCEINLINE T const* GetCDR() const noexcept;
    template<typename T> requires std::is_base_of_v<JCxxClass, T>
    NODISCARD FORCEINLINE T* GetMutableCDR() noexcept;

    NODISCARD FORCEINLINE TArray<LCxxClassField> const& GetFields() const noexcept { return this->Fields; }
    NODISCARD FORCEINLINE TArray<LCxxClassField>& GetMutableFieldsDangerous() noexcept { return this->Fields; }

private:

    Private::GetCDR GetCDRFunction{ nullptr };

    LStringView FullyQualifiedParentName;
    LCxxClass* Parent{ nullptr };
    TArray<LCxxClass*> Children{ nullptr };

    LCxxClassFlags Flags{ ECxxClassFlags::None };

    TUnique<JCxxClass> CDR;
    TArray<LCxxClassField> Fields;
};

namespace Private
{

FORCEINLINE TArray<LRegistryClassPackage const*> LCxxRecordRegistry::GetClassesByBase(LCxxClass const& Base) noexcept
{
    TArray<LRegistryClassPackage const*> Out;

    for (auto& E : this->RegisteredPackages)
    {
        if (E->IsClass() && E->AsClass().StaticClass.DerivesFrom(Base))
        {
            Out.push_back(&E->AsClass());
        }

        continue;
    }

    return Out;
}


} /* ~Namespace Private */

} /* ~Namespace Jafg */
