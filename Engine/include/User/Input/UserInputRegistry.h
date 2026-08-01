// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/InputContext.h"
#include "User/Input/InputAction.h"
#include "Core/TagExt.h"

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

    FORCEINLINE LUserInputContext*       GetContextByName(LUserInputTag Name) noexcept { return algo::find_pointer(this->Contexts, Name, &LUserInputContext::GetTag); }
    FORCEINLINE LUserInputContext const* GetContextByName(LUserInputTag Name) const noexcept { return algo::find_pointer(this->Contexts, Name, &LUserInputContext::GetTag); }
    FORCEINLINE LUserInputContext*       GetContextByNameChecked(LUserInputTag Name) noexcept { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameChecked(LUserInputTag Name) const noexcept { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext*       GetContextByNameAsserted(LUserInputTag Name) noexcept { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameAsserted(LUserInputTag Name) const noexcept { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LUserInputContext*       GetContextByName(LStringView Name) noexcept { return this->GetContextByName(LUserInputTag::ToTag(Name)); }
    FORCEINLINE LUserInputContext const* GetContextByName(LStringView Name) const noexcept { return this->GetContextByName(LUserInputTag::ToTag(Name)); }
    FORCEINLINE LUserInputContext*       GetContextByNameChecked(LStringView Name) noexcept { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameChecked(LStringView Name) const noexcept { auto* Out{ this->GetContextByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LUserInputContext*       GetContextByNameAsserted(LStringView Name) noexcept { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LUserInputContext const* GetContextByNameAsserted(LStringView Name) const noexcept { auto* Out{ this->GetContextByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LInputAction*       GetActionByName(LUserInputTag Name) noexcept { return algo::find_pointer(this->Actions, Name, &LInputAction::GetTag); }
    FORCEINLINE LInputAction const* GetActionByName(LUserInputTag Name) const noexcept { return algo::find_pointer(this->Actions, Name, &LInputAction::GetTag); }
    FORCEINLINE LInputAction*       GetActionByNameChecked(LUserInputTag Name) noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameChecked(LUserInputTag Name) const noexceptcheck { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction*       GetActionByNameAsserted(LUserInputTag Name) noexcept { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameAsserted(LUserInputTag Name) const noexcept { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE LInputAction*       GetActionByName(LStringView Name) noexcept { return this->GetActionByName(LUserInputTag::ToTag(Name)); }
    FORCEINLINE LInputAction const* GetActionByName(LStringView Name) const noexcept { return this->GetActionByName(LUserInputTag::ToTag(Name)); }
    FORCEINLINE LInputAction*       GetActionByNameChecked(LStringView Name) noexcept { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameChecked(LStringView Name) const noexcept { auto* Out{ this->GetActionByName(Name) }; check( Out ) return Out; }
    FORCEINLINE LInputAction*       GetActionByNameAsserted(LStringView Name) noexcept { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }
    FORCEINLINE LInputAction const* GetActionByNameAsserted(LStringView Name) const noexcept { auto* Out{ this->GetActionByName(Name) }; jassert( Out ) return Out; }

    FORCEINLINE auto const& GetTagRegistry() const noexcept { return this->TagRegistry; }
    FORCEINLINE auto& GetMutableTagRegistry() noexcept { return this->TagRegistry; }

    FORCEINLINE constexpr auto const& GetRegisteredActions() const noexcept { return this->Actions; }
    FORCEINLINE constexpr auto& GetMutableRegisteredActions() noexcept { return this->Actions; }

    FORCEINLINE constexpr auto const& GetRegisteredContexts() const noexcept { return this->Contexts; }
    FORCEINLINE constexpr auto& GetMutableRegisteredContexts() noexcept { return this->Contexts; }

private:

    LMasterThreadTagRegistry<LUserInputTag> TagRegistry;
    TArray<LInputAction> Actions;
    TArray<LUserInputContext> Contexts;
};

} /* ~Namespace Jafg */
