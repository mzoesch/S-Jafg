// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputContext.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputAction.h"

namespace Jafg
{

//# Registry for all actions a user may perform.
class LUserInputRegistry final
{
public:

    constexpr LUserInputRegistry() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInputRegistry)
    constexpr ~LUserInputRegistry() = default;

    //# @return The newly mapped action. This is not the same as the input argument. Nullptr if something went wrong.
    ENGINE_API LInputAction const* RegisterAction(LInputAction&& InAction);
    //# @return The context that was added. Nullptr if something went wrong.
    ENGINE_API LUserInputContext*  RegisterContext(LUserInputContext&& Context);

    FORCEINLINE LUserInputContext*       GetContextByName(LName const& Name) noexcept { return algo::wfind_pointer(this->Contexts, Name, &LUserInputContext::GetName); }
    FORCEINLINE LUserInputContext const* GetContextByName(LName const& Name) const noexcept { return algo::wfind_pointer(this->Contexts, Name, &LUserInputContext::GetName); }
    FORCEINLINE LUserInputContext*       GetContextByNameChecked(LName const& Name) noexceptcheck { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameChecked(LName const& Name) const noexceptcheck { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext*       GetContextByNameAsserted(LName const& Name) { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameAsserted(LName const& Name) const { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LUserInputContext*       GetContextByName(LStringView Name) noexcept { return this->GetContextByName(GET_NAME_CHECKED(Name)); }
    FORCEINLINE LUserInputContext const* GetContextByName(LStringView Name) const noexcept { return this->GetContextByName(GET_NAME_CHECKED(Name)); }
    FORCEINLINE LUserInputContext*       GetContextByNameChecked(LStringView Name) noexceptcheck { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameChecked(LStringView Name) const noexceptcheck { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext*       GetContextByNameAsserted(LStringView Name) { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameAsserted(LStringView Name) const { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LInputAction*       GetActionByName(const LName& Name) noexcept { return algo::wfind_pointer(this->Actions, Name, &LInputAction::GetName); }
    FORCEINLINE LInputAction const* GetActionByName(const LName& Name) const noexcept { return algo::wfind_pointer(this->Actions, Name, &LInputAction::GetName); }
    FORCEINLINE LInputAction*       GetActionByNameChecked(const LName& Name) noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameChecked(const LName& Name) const noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction*       GetActionByNameAsserted(const LName& Name) { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameAsserted(const LName& Name) const { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LInputAction*       GetActionByName(LStringView Name) noexcept { return this->GetActionByName(GET_NAME_CHECKED(Name)); }
    FORCEINLINE LInputAction const* GetActionByName(LStringView Name) const noexcept { return this->GetActionByName(GET_NAME_CHECKED(Name)); }
    FORCEINLINE LInputAction*       GetActionByNameChecked(LStringView Name) noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameChecked(LStringView Name) const noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction*       GetActionByNameAsserted(LStringView Name) { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameAsserted(LStringView Name) const { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE constexpr auto const& GetRegisteredActions() const noexcept { return this->Actions; }
    FORCEINLINE constexpr auto& GetMutableRegisteredActions() noexcept { return this->Actions; }

    FORCEINLINE constexpr auto const& GetRegisteredContexts() const noexcept { return this->Contexts; }
    FORCEINLINE constexpr auto& GetMutableRegisteredContexts() noexcept { return this->Contexts; }

private:

    TArray<LInputAction> Actions;
    TArray<LUserInputContext> Contexts;
};

} /* ~Namespace Jafg */
