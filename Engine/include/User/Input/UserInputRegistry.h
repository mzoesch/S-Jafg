// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/InputContext.h"

namespace Jafg
{

//# Registry for all actions a user may perform.
class LUserInputRegistry final
{
public:

    constexpr LUserInputRegistry() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInputRegistry)
    constexpr ~LUserInputRegistry() = default;

    struct ContextCreateInfo final
    {
        LStringView Identifier;
        LStringView DisplayName;
    };
    //# @return The context that was added. Nullptr if something went wrong.
    NODISCARD ENGINE_API LInputContext* RegisterContext(ContextCreateInfo Info);
    NODISCARD LInputContext* RegisterContext(auto&&... Args)
        requires(sizeof...(Args) > 0 && (std::constructible_from<ContextCreateInfo, decltype(Args)&&...>))
    {
        return this->RegisterContext(ContextCreateInfo{std::forward<decltype(Args)>(Args)...});
    }

    NODISCARD FORCEINLINE LInputContext* FindContext(LUserInputTag Tag) noexcept { return algo::find_pointer(this->Contexts, Tag, &LInputContext::GetTag); }
    NODISCARD FORCEINLINE LInputContext const* FindContext(LUserInputTag Tag) const noexcept { return algo::find_pointer(this->Contexts, Tag, &LInputContext::GetTag); }
    NODISCARD FORCEINLINE LInputContext& GetContext(LUserInputTag Tag) noexcept { auto* Result{algo::find_pointer(this->Contexts, Tag, &LInputContext::GetTag)}; check(Result) return *Result; }
    NODISCARD FORCEINLINE LInputContext const& GetContext(LUserInputTag Tag) const noexcept { auto* Result{algo::find_pointer(this->Contexts, Tag, &LInputContext::GetTag)}; check(Result) return *Result; }

    NODISCARD FORCEINLINE constexpr LMasterThreadTagRegistry<LUserInputTag> const& GetTagRegistry() const noexcept { return this->TagRegistry; }
    NODISCARD FORCEINLINE constexpr LMasterThreadTagRegistry<LUserInputTag>& GetMutableTagRegistry() noexcept { return this->TagRegistry; }
    NODISCARD FORCEINLINE constexpr TArray<LInputContext> const& GetRegisteredContexts() const noexcept { return this->Contexts; }
    NODISCARD FORCEINLINE constexpr TArray<LInputContext>& GetMutableRegisteredContexts() noexcept { return this->Contexts; }

private:

    LMasterThreadTagRegistry<LUserInputTag> TagRegistry;
    TArray<LInputContext> Contexts;
};

} /* ~Namespace Jafg */
