// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/UserInput.h"
#include "Framework/PersonaController.h"
#include "Framework/LackeyForward.h"

namespace Jafg
{

namespace Detail
{

//#
//# A lackey is the top most raii entity in the hierarchy for human2engine interactions in worlds by holding
//# on optional transient persona controller.
//#
//# It describes the most basic interface for a persona to traverse the worlds in one engine, and it is persistent
//# for as long said persona exists.
//#
//# @note This is a sealed class. You are not allowed to inherit from this base.
//#
class LLackeyBase
{
public:

    constexpr LLackeyBase() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LLackeyBase)
    virtual ~LLackeyBase() = default;

    NODISCARD FORCEINLINE LLackey ToVariant(this auto&& Self) noexcept
        requires(!std::is_const_v<std::remove_reference_t<decltype(Self)>>);

    NODISCARD FORCEINLINE constexpr bool IsOwnedPersonaControllerValid() const noexcept { return !!this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController* GetOwnedPersonaController() noexcept { return this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController const* GetOwnedPersonaController() const noexcept { return this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController* GetOwnedPersonaControllerChecked() noexcept{ check(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController const* GetOwnedPersonaControllerChecked() const noexcept { check(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController* GetOwnedPersonaControllerAsserted() noexcept { jassert(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    NODISCARD FORCEINLINE APersonaController const* GetOwnedPersonaControllerAsserted() const noexcept { jassert(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }

    //#
    //# @param New The new pc or null if this persona should no longer possess.
    //# @param bReleaseOld Whether to release the old pc if exists (instead of orphaning).
    //# @return The released pc if set.
    //#
    ENGINE_API virtual TJxxUnique<APersonaController> PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld = false);

protected:

    TJxxUnique<APersonaController> Controller;
};

//# @note This is a sealed class. You are not allowed to inherit from this base.
template<ELackey T>
class TLackeyBase : public LLackeyBase
{
public:

    template<ELackey Type>
    NODISCARD FORCEINLINE constexpr bool Is() const noexcept { return T == Type; }
};

} /* ~Namespace Detail */

//#
//# For locally possessed controllers.
//# @note Unlike #Detail::TLackeyBase, this class is not sealed and is supposed to be inherited by sophisticated local lackeys.
//# @see #Detail::LLackeyBase
//#
class LLocalLackey : public Detail::TLackeyBase<ELackey::Local>
{
public:

    LLocalLackey(LViewport& Viewport) noexcept : Viewport{Viewport} {}

    typedef Detail::TLackeyBase<ELackey::Local> Super;

    LViewport& Viewport;

    NODISCARD FORCEINLINE constexpr LUserInput& GetUserInput() noexcept { return this->UserInput; }
    NODISCARD FORCEINLINE constexpr LUserInput const& GetUserInput() const noexcept { return this->UserInput; }

    virtual TJxxUnique<APersonaController> PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld = false) override;

protected:

    LUserInput UserInput;
};

//#
//# For possessed controllers that are proxying in a session.
//# @note Unlike #Detail::TLackeyBase, this class is not sealed and is supposed to be inherited by sophisticated proxying lackeys.
//# @see #Detail::LLackeyBase
//#
class LProxyLackey : public Detail::TLackeyBase<ELackey::Proxy>
{
    // TODO: Implement when the time arrives.
    LProxyLackey() = delete;
};

FORCEINLINE LLackey Detail::LLackeyBase::ToVariant(this auto&& Self) noexcept
    requires(!std::is_const_v<std::remove_reference_t<decltype(Self)>>)
{
    if (Self.template Is<ELackey::Local>())
    {
        return LLackey{static_cast<LLocalLackey*>(&Self)};
    }
    check(Self.template Is<ELackey::Proxy>())
    return LLackey{static_cast<LProxyLackey*>(&Self)};
}

FORCEINLINE constexpr bool APersonaController::IsLocallyPossessed() const noexcept
{
    return this->IsOwningLackey<ELackey::Local>();
}

} /* ~Namespace Jafg */
