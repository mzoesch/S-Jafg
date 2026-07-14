// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Framework/Pawn.h"
#include "Framework/LackeyForward.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LLocalEgo;
class LUserInput;

//# A pc is the world-entity-bridge possessed by a lackey to allow to interact with a world.
DECLARE_JAFG_CLASS()
class ENGINE_API APersonaController : public AActor
{
    GENERATED_CLASS_BODY()

    friend Detail::LLackeyBase;
    friend LLocalLackey;
    friend LProxyLackey;

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(APersonaController)
    {
        this->SetEverTickConstructorOnlyFlag();
    }

public:

    friend WWorldNode;

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    FORCEINLINE constexpr bool IsLocallyPossessed() const noexcept;

    FORCEINLINE constexpr bool IsOwningLackeyValid() const noexcept { return !std::holds_alternative<std::monostate>(this->Lackey); }
    FORCEINLINE constexpr auto GetOwningBaseLackey(this auto&& Self) noexcept
        -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(Self)>>, Detail::LLackeyBase const&, Detail::LLackeyBase&>
    {
        check(Self.IsOwningLackeyValid())
        if (std::holds_alternative<LLocalLackey*>(Self.Lackey))
        {
            auto* Result{std::get<1>(Self.Lackey)};
            check(Result)
            return *Result;
        }
        auto* Result{std::get<2>(Self.Lackey)};
        check(Result)
        return *Result;
    }
    template<ELackey Type>
    FORCEINLINE constexpr bool IsOwningLackey() const noexcept
    {
        return this->IsOwningLackeyValid()
            && !!std::holds_alternative<std::conditional_t<Type == ELackey::Local, LLocalLackey*, LProxyLackey*>>(this->Lackey);
    }
    template<ELackey Type>
    FORCEINLINE auto GetOwningLackey(this auto&& Self) noexcept
        -> std::conditional_t<Type == ELackey::Local,
        std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(Self)>>, LLocalLackey const&, LLocalLackey&>,
        std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(Self)>>, LProxyLackey const&, LProxyLackey&>
        >
    {
        check(Self.template IsOwningLackey<Type>())
        if constexpr (Type == ELackey::Local)
        {
            auto* Result{std::get<1>(Self.Lackey)};
            check(Result)
            return *Result;
        }
        else
        {
            auto* Result{std::get<2>(Self.Lackey)};
            check(Result)
            return *Result;
        }
    }
    template<ELackey Type>
    FORCEINLINE auto TryGetOwningLackey(this auto&& Self) noexcept
        -> std::conditional_t<Type == ELackey::Local,
        std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(Self)>>, LLocalLackey const*, LLocalLackey*>,
        std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(Self)>>, LProxyLackey const*, LProxyLackey*>
        >
    {
        if (Self.template IsOwningLackey<Type>())
        {
            return &Self.template GetOwningLackey<Type>();
        }
        return nullptr;
    }

    FORCEINLINE bool IsOwnedPawnValid() const noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawn() noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawn() const noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawnChecked() noexcept { check(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawnChecked() const noexcept { check(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawnAsserted() noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawnAsserted() const noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    void PossessPawn(TJxxUnique<APawn> New, bool bReleaseOld = true);

private:

    LLackey Lackey;
    TJxxUnique<APawn> Pawn;
};

} /* ~Namespace Jafg */

#include "Framework/Lackey.h"
