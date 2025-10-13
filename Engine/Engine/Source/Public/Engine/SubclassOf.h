// Copyright mzoesch. All rights reserved.

#pragma once

#include "SubclassOf.h"
#include "Engine/CxxClass.h"

namespace Jafg
{

template<typename TObj>
class TSubclassOf;

template<typename TObj>
class TSubclassOf final
{
    template <typename UObj>
    friend class TSubclassOf;

public:

    // static_assert(!Lal::TIsCompleteType_v<TObj> || std::is_base_of_v<JCxxClass, TObj>);

    constexpr TSubclassOf() noexcept : Class{nullptr} { }
    constexpr TSubclassOf(LNullptrTy) noexcept : Class{nullptr} { }

    TSubclassOf(EDefaultInit) noexcept : Class{TObj::StaticClass()} { check( this->HasClass() && this->IsValidType() ) }

    TSubclassOf(LCxxClass const* InClass) noexceptcheck : Class{InClass} { check( this->IsValidType() ) }
    TSubclassOf(LCxxClass const& InClass) noexceptcheck : Class{&InClass} { check( this->IsValidType() ) }
    TSubclassOf& operator=(LCxxClass const* InClass) noexceptcheck { return this->Assign(InClass); }
    TSubclassOf& operator=(LCxxClass const& InClass) noexceptcheck { return this->Assign(InClass); }

    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf(TSubclassOf<UObj> const& Other) noexceptcheck : Class{Other.Class} { check( this->IsValidType() ) }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf& operator=(TSubclassOf<UObj> const& Other) noexceptcheck { return this->Assign(Other.Class); }

    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf(UObj const* What) noexcept : Class{What->GetVirtualTable()} { check( this->IsValidType() ) }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    TSubclassOf& operator=(UObj const* What) noexcept { return this->Assign(What->GetVirtualTable()); }

    constexpr ~TSubclassOf() = default;

    FORCEINLINE bool HasClass() const noexcept { return this->Class != nullptr; }
    template<typename UObj> requires std::is_base_of_v<TObj, UObj>
    FORCEINLINE void SetClass() noexcept { this->Assign(UObj::StaticClass()); }
    FORCEINLINE void SetClass(LNullptrTy) noexcept { this->Assign(nullptr); }

    FORCEINLINE TSubclassOf& Assign(LCxxClass const* InClass) noexceptcheck
    {
        this->Class = InClass;
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE TSubclassOf& Assign(LCxxClass const& InClass) noexceptcheck
    {
        this->Class = &InClass;
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE LCxxClass const* GetClass() const noexcept { return this->Class; }
    FORCEINLINE operator LCxxClass const*() const noexcept { return this->Class; }

    FORCEINLINE operator LCxxClass const&() const noexceptcheck { check( this->HasClass() ) return *this->Class; }

    FORCEINLINE LCxxClass const* operator->() const noexcept { check( this->HasClass() ) return this->Class; }
    FORCEINLINE LCxxClass const* operator*() const noexcept { if (this->HasClass()) { return this->Class; } return nullptr; }

    FORCEINLINE TObj const* GetCDR() noexcept
    {
        if (this->HasClass())
        {
            return this->Class->GetCDR<TObj>();
        }

        return nullptr;
    }

    FORCEINLINE bool IsValidType() const noexcept
    {
        if (this->Class)
        {
            return this->Class->DerivesFrom<TObj>();
        }

        return true;
    }

    FORCEINLINE bool operator==(const LNullptrTy) const noexcept { return this->Class == nullptr; }

private:

    LCxxClass const* Class;
};

} /* ~Namespace Jafg */
