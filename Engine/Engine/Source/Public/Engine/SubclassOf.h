// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"

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

    TSubclassOf(Jafg::LCxxClass const* InClass) noexceptcheck : Class{InClass} { check( this->IsValidType() ) }
    TSubclassOf(Jafg::LCxxClass const& InClass) noexceptcheck : Class{&InClass} { check( this->IsValidType() ) }
    TSubclassOf& operator=(Jafg::LCxxClass const* InClass) noexceptcheck { return this->Assign(InClass); }
    TSubclassOf& operator=(Jafg::LCxxClass const& InClass) noexceptcheck { return this->Assign(InClass); }

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

    FORCEINLINE TSubclassOf& Assign(Jafg::LCxxClass const* InClass) noexceptcheck
    {
        this->Class = InClass;
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE TSubclassOf& Assign(Jafg::LCxxClass const& InClass) noexceptcheck
    {
        this->Class = &InClass;
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE Jafg::LCxxClass const& GetCLassOrDefault() const noexcept
    {
        if (this->HasClass())
        {
            return *this->Class;
        }

        return *TObj::StaticClass();
    }

    FORCEINLINE Jafg::LCxxClass const* GetClass() const noexcept { return this->Class; }
    FORCEINLINE operator Jafg::LCxxClass const*() const noexcept { return this->Class; }

    FORCEINLINE operator Jafg::LCxxClass const&() const noexceptcheck { check( this->HasClass() ) return *this->Class; }

    FORCEINLINE Jafg::LCxxClass const* operator->() const noexcept { check( this->HasClass() ) return this->Class; }
    FORCEINLINE Jafg::LCxxClass const* operator*() const noexcept { if (this->HasClass()) { return this->Class; } return nullptr; }

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
                TObj::StaticClass()->GetFullyQualifiedName()
                )
            return true;
        }

        return true;
    }

    FORCEINLINE bool operator==(const LNullptrTy) const noexcept { return this->Class == nullptr; }

private:

    Jafg::LCxxClass const* Class;
};
