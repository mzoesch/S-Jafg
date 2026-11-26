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

    LLocalLackey(LViewport& Viewport) noexcept : Viewport{Viewport}, UserInput{Viewport} {}

    typedef Detail::TLackeyBase<ELackey::Local> Super;

    LViewport& Viewport;

    NODISCARD FORCEINLINE constexpr LUserInput& GetUserInput() noexcept { return this->UserInput; }
    NODISCARD FORCEINLINE constexpr LUserInput const& GetUserInput() const noexcept { return this->UserInput; }

    virtual TJxxUnique<APersonaController> PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld = false) override;

    //#
    //# The extent in pixels of the world that this lackey sees. The space of the world view is the one that the
    //# perspective of the pawn is rendered to. Zero if this lackey does not render a world.
    //#
    NODISCARD virtual LVec2F GetWorldViewExtent() const noexcept { return LVec2F{0.0f}; }
    //# The location of the shown mouse cursor in the world view. Empty if the cursor is hidden or not above the world.
    NODISCARD virtual std::optional<LVec2F> GetCursorLocationInWorldView() const noexcept { return {}; }
    //#
    //# Moves the mouse cursor to a location in the world view.
    //# @return False if the platform does not allow to move the cursor or this lackey does not render a world.
    //#
    virtual bool SetCursorLocationInWorldView(LVec2F Location) noexcept { (void)Location; return false; }
    //#
    //# The viewport of the world view: Drawn over the rendered world, in the space of the world view. For the widgets
    //# of the world (e.g., HUDs and windows of the game). Like the world, they stay inside of the world view, as in a
    //# game without the editor. They get the inputs before the user input contexts of this lackey. Null if this lackey
    //# does not render a world.
    //#
    NODISCARD virtual LViewport* GetWorldViewport() noexcept { return nullptr; }
    //#
    //# The viewport that the world view of this lackey is shown in, e.g., the editor around it (the parent of
    //# #GetWorldViewport, @see #LViewport::GetParent).
    //#
    NODISCARD virtual LViewport& GetParentViewport() noexcept { return this->Viewport; }
    //# Where the world view is in #GetParentViewport. Empty if this lackey does not render a world.
    NODISCARD virtual std::optional<LRect2F> GetWorldViewRectInParentViewport() const noexcept { return {}; }

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
