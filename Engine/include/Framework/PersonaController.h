// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Framework/LackeyForward.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LLocalEgo;
class LUserInput;
class AAvatarComponent;
class AEyeComponent;

//#
//# A pc is the world-entity-bridge possessed by a lackey to allow to interact with a world.
//#
//# A pc possesses an avatar (see #AAvatarComponent), not an actor, and sees through the eye of that avatar.
//# Possession does not own anything. The lifetime of actors can be bound to a pc with #AdoptActor.
//#
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
        this->SetCanEverTick(true);
        this->SetCanEverTickInDormantTimes(true);
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

    NODISCARD FORCEINLINE constexpr bool IsPossessing() const noexcept { return !!this->Avatar; }
    NODISCARD FORCEINLINE constexpr AAvatarComponent* GetPossessedAvatar() const noexcept { return this->Avatar; }
    NODISCARD FORCEINLINE AAvatarComponent* GetPossessedAvatarChecked() const noexcept { check(this->IsPossessing()) return this->Avatar; }
    //# The actor of the possessed avatar. Null if nothing is possessed.
    NODISCARD AActor* GetPossessedActor() const noexcept;
    NODISCARD FORCEINLINE AActor* GetPossessedActorChecked() const noexcept { auto* Out{this->GetPossessedActor()}; check(Out) return Out; }
    //#
    //# Possess an avatar, or nothing with nullptr. The previously possessed avatar is released but stays in the
    //# world. An avatar that is possessed by another pc is taken from it.
    //#
    void Possess(AAvatarComponent* New);

    //# The eye that this pc sees the world through: The eye of the possessed avatar. Null if there is none.
    NODISCARD AEyeComponent* GetViewEye() const noexcept;

    //#
    //# Bind the lifetime of an actor to this pc (e.g., the character that was spawned for it). The actor is
    //# destroyed with this pc. Adopting does not possess.
    //#
    void AdoptActor(TJxxUnique<AActor> Actor);
    //# @return The actor that is now owned by the caller. Empty if this pc did not adopt it.
    TJxxUnique<AActor> AbandonActor(AActor& Actor);
    NODISCARD FORCEINLINE constexpr TArray<AActor*> const& GetAdoptedActors() const noexcept { return this->AdoptedActors; }

    //# Jafg internal method. Do not use. Called when the possessed avatar is torn down.
    void _OnAvatarGarbage(AAvatarComponent& Garbage) noexcept;

private:

    LLackey Lackey;
    AAvatarComponent* Avatar{};
    TArray<AActor*> AdoptedActors;
};

} /* ~Namespace Jafg */

#include "Framework/Lackey.h"
