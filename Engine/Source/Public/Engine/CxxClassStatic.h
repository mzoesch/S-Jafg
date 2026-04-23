// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxRecord.h"
#include "Engine/CxxClassFlags.h"
#include "Engine/CxxClass.h"
#include "Engine/CxxRecordUtility.h"

namespace Jafg
{

class LCxxClass;
struct LBeginClassLifeInfo;
struct LEndClassLifeInfo;

namespace Detail
{

typedef JCxxClass*(*MallocCxxFn)(LCxxDynamicInit const&);
typedef void(*BeginClassLifeFn)(LBeginClassLifeInfo const&);
typedef void(*EndClassLifeFn)(LEndClassLifeInfo const&);

} /* ~Namespace Detail */

//#
//# A reflected class.
//#
class LCxxClass final : public LCxxRecord
{
    friend Detail::LCxxRecordRegistry;

public:

    LCxxClass() = delete;
    LCxxClass(
          LString InFullyQualifiedName
        , Detail::MallocCxxFn InMallocCxx
        , Detail::BeginClassLifeFn InBeginClassLife
        , Detail::EndClassLifeFn InEndClassLife
        , LStringView InParentName
        , ECxxClassFlags::Type InFlags
        , TArray<LCxxClassField>&& InFields
        ) noexcept
        : LCxxRecord{std::move(InFullyQualifiedName)}
        , MallocCxx(InMallocCxx)
        , BeginClassLife(InBeginClassLife)
        , EndClassLife(InEndClassLife)
        , ParentName(std::move(InParentName))
        , Flags(InFlags)
        , Fields(std::move(InFields))
    {
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxClass)
    ~LCxxClass() = default;

    NODISCARD FORCEINLINE auto const& GetMallocCxxFn() const noexcept { return this->MallocCxx; }
    NODISCARD FORCEINLINE auto const& GetBeginClassLifeFn() const noexcept { return this->BeginClassLife; }
    NODISCARD FORCEINLINE auto const& GetEndClassLifeFn() const noexcept { return this->EndClassLife; }

    NODISCARD FORCEINLINE bool IsValid() const noexcept { return this->Parent != nullptr || this->GetFullyQualifiedName() == "::Jafg::JCxxClass"; }

    NODISCARD FORCEINLINE bool IsRoot() const noexcept { return this->Parent == nullptr && this->GetFullyQualifiedName() == "::Jafg::JCxxClass"; }
    NODISCARD FORCEINLINE bool IsParentValid() const noexcept { return this->Parent != nullptr; }
    NODISCARD FORCEINLINE auto GetParent()         noexcept -> LCxxClass*                { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetParent()   const noexcept -> LCxxClass const*          { check( this->Parent ) return this->Parent; }
    NODISCARD FORCEINLINE auto GetChildren()       noexcept -> TArray<LCxxClass*>&       { return this->Children; }
    NODISCARD FORCEINLINE auto GetChildren() const noexcept -> TArray<LCxxClass*> const& { return this->Children; }

    template<typename T> requires std::is_base_of_v<JCxxClass, T>
    NODISCARD FORCEINLINE bool DerivesFrom() const noexcept { return this->DerivesFrom(T::StaticClass()); }
    NODISCARD ENGINE_API  bool DerivesFrom(LCxxClass const& Parent) const noexcept;

    NODISCARD FORCEINLINE auto GetFlags()       const noexcept { return this->Flags; }
    NODISCARD FORCEINLINE bool HasAnyFlags()    const noexcept { return  this->Flags != ECxxClassFlags::None;                               }
    NODISCARD FORCEINLINE bool IsAbstract()     const noexcept { return (this->Flags  & ECxxClassFlags::Abstract)  != ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsNotAbstract()  const noexcept { return (this->Flags  & ECxxClassFlags::Abstract)  == ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsConfig()       const noexcept { return (this->Flags  & ECxxClassFlags::Config)    != ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsNotConfig()    const noexcept { return (this->Flags  & ECxxClassFlags::Config)    == ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsSingleton()    const noexcept { return (this->Flags  & ECxxClassFlags::Singleton) != ECxxClassFlags::None; }
    NODISCARD FORCEINLINE bool IsNotSingleton() const noexcept { return (this->Flags  & ECxxClassFlags::Singleton) == ECxxClassFlags::None; }

    NODISCARD FORCEINLINE TArray<LCxxClassField> const& GetFields() const noexcept { return this->Fields; }
    NODISCARD FORCEINLINE TArray<LCxxClassField>& GetMutableFieldsDangerous() noexcept { return this->Fields; }

    template<typename TCxxClass = JCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TCxxClass const& GetSingleton() const noexcept;
    template<typename TCxxClass = JCxxClass> requires std::is_base_of_v<JCxxClass, TCxxClass>
    NODISCARD FORCEINLINE TCxxClass& GetSingleton() noexcept;

private:

    Detail::MallocCxxFn MallocCxx{};
    Detail::BeginClassLifeFn BeginClassLife{};
    Detail::EndClassLifeFn EndClassLife{};

    LStringView ParentName;
    LCxxClass* Parent{};
    TArray<LCxxClass*> Children;

    LCxxClassFlags Flags;

    TArray<LCxxClassField> Fields;

    JCxxClass* Singleton{};
};

namespace Detail
{

NODISCARD FORCEINLINE LString const& LRegistryClassPackage::GetFullyQualifiedName() const noexcept
{
    return this->StaticClass.GetFullyQualifiedName();
}

} /* ~Namespace Detail */

namespace Detail
{

NODISCARD FORCEINLINE TArray<LCxxClass const*> LCxxRecordRegistry::GetClassesByBase(LCxxClass const& Base) const noexcept
{
    TArray<LCxxClass const*> Out;

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

} /* ~Namespace Detail */

} /* ~Namespace Jafg */
